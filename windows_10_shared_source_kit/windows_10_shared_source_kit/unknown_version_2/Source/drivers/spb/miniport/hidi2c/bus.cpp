stEnv.WriteToLog("Unable to copy the 'foreground image' onto the source surface.\n");
				else
				{
					bInitialized = true;
					d3dfmtSetFormat = d3dfmt;
					dwMainSurfaceSpec = dwSurfaceSpec;
					return true;
				}
			}

			texsurfSourceLoader.ReleaseDX8Objects();
		}

		delete pSourceTestSurface;
		pSourceTestSurface = NULL;
	}

	return false;
}

// clsDestSurfaceSet methods

void clsDestSurfaceSet::ReleaseAllSurfaces()
{
	texsurfDestLoader.ReleaseDX8Objects();
	texsurfReference.ReleaseDX8Objects();
	texsurfDestCopy.ReleaseDX8Objects();

	if (pDestTestSurface)
	{
		delete pDestTestSurface;
		pDestTestSurface = NULL;
	}

	bInitialized = false;
	d3dfmtSetFormat = D3DFMT_UNKNOWN;
	dwMainSurfaceSpec = 0;
}

bool clsDestSurfaceSet::PrepareFromDWORD(DWORD dwSurfaceSpec, D3DFORMAT d3dfmt)
{
	if (bInitialized)
		if ((d3dfmtSetFormat == d3dfmt) && (dwMainSurfaceSpec == dwSurfaceSpec))
			return true;
		else
			ReleaseAllSurfaces();

	if (!(pDestTestSurface = CreateSurfaceFromDWORD(dwSurfaceSpec, d3dfmt)))
		MainTestEnv.WriteToLog("Unable to create destination surface.\n");
	else
	{
		CSurface *lpd3dsurf8Dest;

		pDestTestSurface -> Describe(szDescription);

		// Retrieve a pointer to the DX8 surface interface
		// for the destination surface.
		lpd3dsurf8Dest = pDestTestSurface -> GetSurfacePointer();

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
			!CreateCompatibleSMSurface(lpd3dsurf8Dest, &texsurfDestLoader) ||
			!CreateCompatibleSMSurface(lpd3dsurf8Dest, &texsurfReference) ||
			!CreateCompatibleSMSurface(lpd3dsurf8Dest, &texsurfDestCopy)
		)
			MainTestEnv.WriteToLog("Unable to create all of the destination-compatible auxiliary surfaces.\n");
		else
/*

*/
		{
			CSurface *lpd3dsurf8DestLoader;
			HRESULT hresult;
			D3DXIMAGE_INFO d3dxii;

			// Retrieve a pointer to the "loader surface".
			lpd3dsurf8DestLoader = texsurfDestLoader.GetSurfacePointer();

			// Load the "background" image onto the loader surface.
			if
			(
				FAILED
				(
					(
						hresult =
						(MainTestEnv.m_pSrcDevice) ->
						D3DXLoadSurfaceFromFileInMemory
						(
							lpd3dsurf8DestLoader,
							NULL,
							NULL,
							MainTestEnv.BitmapDataBackground,
							((PBITMAPFILEHEADER)(MainTestEnv.BitmapDataBackground)) -> bfSize,
							NULL,
							D3DX_FILTER_LINEAR | D3DX_FILTER_MIRROR,
							0,
							&d3dxii
						)
					)
				)
			)
				MainTestEnv.WriteToLog("Unable to load the destination-compatible 'loader' surface with the 'background image'.\n");
			else
			{
				bInitialized = true;
				d3dfmtSetFormat = d3dfmt;
				dwMainSurfaceSpec = dwSurfaceSpec;
				return true;
			}
		}

		texsurfDestLoader.ReleaseDX8Objects();
		texsurfReference.ReleaseDX8Objects();
		texsurfDestCopy.ReleaseDX8Objects();

		delete pDestTestSurface;
		pDestTestSurface = NULL;
	}

	return false;
}

CCopyRects::CCopyRects()
{
	// Set the test properties.
	SetTestType(TESTTYPE_CONF);

	m_szCommandKey=	_T("CopyRects");
	m_szLogName = _T("CopyRects");
	m_szTestName=	_T("CopyRects");

	(m_Options.D3DOptions).fMinDXVersion = 8.1f;
	(m_Options.D3DOptions).fMaxDXVersion = 8.1f;
	(m_Options.TestOptions).bCompareSrcRef = false;

	BitmapDataForeground = (BitmapDataBackground = NULL);

	// This is necessary to avoid false failures when surface data is composed entirely of zeroes.
	m_pFramework -> GetImageCmp() -> SetMinPixels(0);
}

CCopyRects::~CCopyRects()
{
}

void CCopyRects::FindValidTextureSpecs(D3DFORMAT d3dfmt, clsSurfaceSpecList *pSpecList)
{
	UINT uiUsageSetIndex;
	FORMAT FrameworkFormat((FMT)d3dfmt);
	FORMAT DesktopFrameworkFormat((FMT)(pd3dfmtrecDesktop -> d3dformat));

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
						uiTestedAdapterOrdinal,
						d3ddevtypeTestedType,
						DesktopFrameworkFormat,
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
				if ((dwUsageSets[uiUsageSetIndex] != D3DUSAGE_RENDERTARGET) ||
					(DX8PoolTypes[uiPoolTypeIndex].d3dpool == D3DPOOL_DEFAULT))
				{
					DWORD dwSurfaceSpec;

					dwSurfaceSpec = (((DWORD)uiUsageSetIndex) << 4) |
					(((DWORD)uiPoolTypeIndex) << 2);
					pSpecList -> AddSurfaceSpec(dwSurfaceSpec);
					dwSurfaceSpec |= (DWORD)1;
					pSpecList -> AddSurfaceSpec(dwSurfaceSpec);

					/*
					clsTextureSpec *pNewTextureSpec;

					pNewTextureSpec = new clsTextureSpec;
					pNewTextureSpec -> Initialize(cuiMinSurfaceWidth, cuiMinSurfaceHeight, d3dfmt,
					dwUsageSets[uiUsageSetIndex], DX8PoolTypes[uiPoolTypeIndex].d3dpool);
					pSpecList ->AddSurfaceSpec(pNewTextureSpec);

					pNewTextureSpec = new clsTextureSpec;
					pNewTextureSpec -> Initialize((cuiMinSurfaceWidth << 1), (cuiMinSurfaceHeight << 1), d3dfmt,
					dwUsageSets[uiUsageSetIndex], DX8PoolTypes[uiPoolTypeIndex].d3dpool);
					pSpecList ->AddSurfaceSpec(pNewTextureSpec);
					*/
				}
			}
		}
	}
}

void CCopyRects::FindValidRTSpecs(D3DFORMAT d3dfmt, clsSurfaceSpecList *pSpecList)
{
	HRESULT hresult;
	FORMAT FrameworkFormat((FMT)d3dfmt);
	FORMAT DesktopFrameworkFormat((FMT)(pd3dfmtrecDesktop -> d3dformat));

	// Determine whether the format is a valid render target format.
	if
	(
		SUCCEEDED
		(
			(
				hresult =
				m_pD3D -> CheckDeviceFormat
				(
					uiTestedAdapterOrdinal,
					d3ddevtypeTestedType,
					DesktopFrameworkFormat,
					D3DUSAGE_RENDERTARGET,
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
						m_pD3D -> CheckDeviceMultiSampleType
						(
							uiTestedAdapterOrdinal,
							d3ddevtypeTestedType,
							FrameworkFormat,
							TRUE,
							(MULTISAMPLE_TYPE)((DX8MultiSampleTypes[uiMultiSampleTypeIndex]).d3dmst)
						)
					)
				)
			)
			if ((d3dfmt != D3DFMT_A8) || (uiMultiSampleTypeIndex == 0))
			{
				DWORD dwSurfaceSpec;

				dwSurfaceSpec = (((DWORD)uiMultiSampleTypeIndex) << 3) | 64;
				// (Bits 6 through 9 denote the surface type.  '0001' denotes a render target.
				// Bits 3 through 5 denote the multisample type.)

				pSpecList -> AddSurfaceSpec(dwSurfaceSpec);
				pSpecList -> AddSurfaceSpec(dwSurfaceSpec | 1);
				if ((MULTISAMPLE_TYPE)((DX8MultiSampleTypes[uiMultiSampleTypeIndex]).d3dmst) == D3DMULTISAMPLE_NONE)
				{
					pSpecList -> AddSurfaceSpec(dwSurfaceSpec | 4);
					pSpecList -> AddSurfaceSpec(dwSurfaceSpec | 5);
				}
				// The four surface type variants account for two different surface types
				// and lockable vs non-lockable render targets.
			}
		}

	}
}

void CCopyRects::InitializeSurfaceList(D3DFORMAT d3dfmt, clsSurfaceSpecArray *pSpecArray)
{
	clsSurfaceSpecList CompatibleSurfaceSpecList;

	FindValidTextureSpecs(d3dfmt, &CompatibleSurfaceSpecList);
	FindValidRTSpecs(d3dfmt, &CompatibleSurfaceSpecList);

	CompatibleSurfaceSpecList.InitializeSurfaceSpecArray(pSpecArray);
}

UINT CCopyRects::TestInitialize(void)
{
	//D3DDISPLAYMODE d3ddisplaymodeAdapter;
	HRESULT hresult;
	DISPLAYMODE d3ddisplaymodeAdapter;
	D3DDEVICE_CREATION_PARAMETERS d3ddcpSrcDevice8;

	UINT uiFormatIndex, uiTotalTestCases = 0;

	CSurface *psurfSrcBackBuffer;

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
			rectReferenceDisplay.left = (uiPanelWidth >> 1) - (uiDisplayEdgeLength >> 1);
			rectReferenceDisplay.top = (uiPanelHeight >> 1) - (uiDisplayEdgeLength >> 1);
			rectReferenceDisplay.right = rectReferenceDisplay.left + uiDisplayEdgeLength;
			rectReferenceDisplay.bottom = rectReferenceDisplay.top + uiDisplayEdgeLength;

			rectTestDisplay.left = ((sdSrcBackBuffer.Width + 1) >> 1) + (uiPanelWidth >> 1) - (uiDisplayEdgeLength >> 1);
			rectTestDisplay.top = rectReferenceDisplay.top;
			rectTestDisplay.right = rectTestDisplay.left + uiDisplayEdgeLength;
			rectTestDisplay.bottom = rectReferenceDisplay.bottom;
			*/
		}
	}

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

	// Determine the adapter ordinal and the type of the Direct3D device, i.e. HAL, REF, etc.
	GetCreationParameters(&d3ddcpSrcDevice8);
	uiTestedAdapterOrdinal = d3ddcpSrcDevice8.AdapterOrdinal;
	d3ddevtypeTestedType = d3ddcpSrcDevice8.DeviceType;

	m_pD3D -> GetAdapterDisplayMode(uiTestedAdapterOrdinal, &d3ddisplaymodeAdapter);

	if (!(pd3dfmtrecDesktop = FindFormatRecord((d3ddisplaymodeAdapter.Format).d3dfFormat)))
	{
		/*
		LogAppendLine("Unable to identify the format of the current desktop mode.");
		strcpy(szTestRunSummary, "FAIL: The application was unable to identify the format of the current desktop mode.");
		*/
		return D3DTESTINIT_SKIPALL;
	}

	// Determine the set of testable surface types for each format.

	for (uiFormatIndex = 0; uiFormatIndex < cuiNoOfFormats; ++uiFormatIndex)
	{
		D3DFORMAT d3dfmtTestFormat;
		UINT uiNoOfSurfaceSpecs;

		d3dfmtTestFormat = DX8GraphicsFormats[uiFormatIndex].d3dformat;

		InitializeSurfaceList(d3dfmtTestFormat, &(TestableSurfaceTypesByFormat[uiFormatIndex]));

		uiNoOfSurfaceSpecs = (TestableSurfaceTypesByFormat[uiFormatIndex]).GetNumberOfElements();

		uiTotalTestCases += (uiNoOfSurfaceSpecs * uiNoOfSurfaceSpecs * cuiNoOfRectSets);
	}

	if (!uiTotalTestCases)
		return D3DTESTINIT_SKIPALL;

	SetTestRange(1, uiTotalTestCases);

	return D3DTESTINIT_RUN;
}

bool CCopyRects::ExecuteTest(UINT uiTestCaseId)
{
	bool bTestExecuted;

	UINT uiFormatSpecificId = uiTestCaseId - 1, uiFormatIndex = 0, uiNoOfSurfaceSpecs,
		uiNoOfTestCases, uiRectangleSetIndex, uiDestSpecIndex, uiSourceSpecIndex,
		uiSystemIndependentTCID;

	if (!cuiNoOfFormats)
	{
		ReportInvalidTCID(uiTestCaseId);
		return false;
	}

	// Determine the test format and the offset of the test case within the set of test cases
	// associated with the format.
	uiNoOfSurfaceSpecs = (TestableSurfaceTypesByFormat[uiFormatIndex]).GetNumberOfElements();
	uiNoOfTestCases = (uiNoOfSurfaceSpecs * uiNoOfSurfaceSpecs * cuiNoOfRectSets);
	if (uiFormatSpecificId >= uiNoOfTestCases)
	{
		do
		{
			if ((++uiFormatIndex) == cuiNoOfFormats)
			{
				ReportInvalidTCID(uiTestCaseId);
				return false;
			}
			uiFormatSpecificId -= uiNoOfTestCases;
			uiNoOfSurfaceSpecs = (TestableSurfaceTypesByFormat[uiFormatIndex]).GetNumberOfElements();
			uiNoOfTestCases = (uiNoOfSurfaceSpecs * uiNoOfSurfaceSpecs * cuiNoOfRectSets);
		}
		while (uiFormatSpecificId >= uiNoOfTestCases);
	}

	// From the offset of the test case within the set of test cases associated with the format,
	// determine the indices (within the array associated with the format) of the specifications
	// of the source and destination surfaces and the index of the rectangle set.
	uiRectangleSetIndex = uiFormatSpecificId % cuiNoOfRectSets;
	uiFormatSpecificId /= cuiNoOfRectSets;
	uiDestSpecIndex = uiFormatSpecificId % uiNoOfSurfaceSpecs;
	uiSourceSpecIndex = uiFormatSpecificId / uiNoOfSurfaceSpecs;

	//WriteToLog("%d %d %d %d\n", uiFormatIndex, uiSourceSpecIndex, uiDestSpecIndex, uiRectangleSetIndex);

	// Compose a "system independent test case ID".
	uiSystemIndependentTCID =
		((DWORD)uiFormatIndex << 26) |
		(((TestableSurfaceTypesByFormat[uiFormatIndex]).GetElementValue(uiSourceSpecIndex)) << 16) |
		(((TestableSurfaceTypesByFormat[uiFormatIndex]).GetElementValue(uiDestSpecIndex)) << 6) |
		((DWORD)uiRectangleSetIndex);

	//WriteToLog("System Independent Test Case ID Extracted: %d\n", uiSystemIndependentTCID);

	BeginTestCase("Test Case", uiTestCaseId);

	bTestExecuted = ExecuteNumberedTest(uiSystemIndependentTCID);

	//EndTestCase();

	if (!bTestExecuted)
		SkipTestRange(uiTestCaseId, uiTestCaseId);

	return bTestExecuted;
}

bool CCopyRects::ExecuteNumberedTest(UINT uiSystemIndependentTCID)
{
	// The bits of the "test ID" are interpreted as follows:
	// 0--5:	the set of rectangles used in the test,
	// 6--15:	a specification of the destination surface,
	// 16--25:	a specification of the source surface and
	// 26--31:	the format of the source and destination surfaces.

	static CONST UINT uiRectangleSetLSB = 0;
	static CONST UINT uiRectangleSetMSB = 5;

	static CONST UINT uiDestSurfSpecLSB = 6;
	static CONST UINT uiDestSurfSpecMSB = 15;

	static CONST UINT uiSourceSurfSpecLSB = 16;
	static CONST UINT uiSourceSurfSpecMSB = 25;

	static CONST UINT uiFormatLSB = 26;
	static CONST UINT uiFormatMSB = 31;

	UINT uiFormatRecordIndex, uiRectSetIndex;
	D3DFORMAT d3dfmtTestFormat;

	DPF(2, _T("\n"));
	DPF(2, _T("Executing Test Case: %d\n"), uiSystemIndependentTCID);

	if ((uiRectSetIndex = (UINT)ExtractBitsRange((DWORD)uiSystemIndependentTCID, uiRectangleSetLSB, uiRectangleSetMSB)) >= cuiNoOfRectSets)
		WriteToLog("The test case identifier does not specify a valid rectangle set.\n");
	else
		if ((uiFormatRecordIndex = (UINT)ExtractBitsRange((DWORD)uiSystemIndependentTCID, uiFormatLSB, uiFormatMSB)) >= cuiNoOfFormats)
			WriteToLog("The test case identifier does not specify a valid surface format.\n");
		else
		{
			d3dfmtTestFormat = DX8GraphicsFormats[uiFormatRecordIndex].d3dformat;

			if (SourceSurfaceSet.PrepareFromDWORD(ExtractBitsRange((DWORD)uiSystemIndependentTCID, uiSourceSurfSpecLSB, uiSourceSurfSpecMSB), d3dfmtTestFormat))
				if (DestSurfaceSet.PrepareFromDWORD(ExtractBitsRange((DWORD)uiSystemIndependentTCID, uiDestSurfSpecLSB, uiDestSurfSpecMSB), d3dfmtTestFormat))
				{
					return TestCopyOperation(&SourceSurfaceSet, &DestSurfaceSet, uiRectSetIndex, (DWORD)uiSystemIndependentTCID);
				}
		}


	//"The application cannot perform a test based on the test case identifier."

	return false;

	/*
	if (PrepareSourceSurface((DWORD)(HIWORD((DWORD)uiSystemIndependentTCID))))
		if (PrepareDestSurface(ExtractBitsRange((DWORD)uiSystemIndependentTCID, 6, 15) || (ExtractBitsRange((DWORD)uiSystemIndependentTCID, 26, 31) << 10)))
		{
		}
	*/

	//(DWORD)(HIWORD((DWORD)uiSystemIndependentTCID))
	//ExtractBitsRange((DWORD)uiSystemIndependentTCID, 6, 15) || (ExtractBitsRange((DWORD)uiSystemIndependentTCID, 26, 31) << 10)
}

HRESULT CCopyRects::LogCreateTexture
(
	UINT Width,
	UINT Height,
	UINT  Levels,
	DWORD Usage,
	D3DFORMAT Format,
	D3DPOOL Pool,
	CTexture **ppTexture
	//LPDIRECT3DTEXTURE8 *ppTexture
)
{
	HRESULT hresult;
	FORMAT FrameworkFormat((FMT)Format);

	if
	(
		FAILED
		(
			(
				hresult =
				m_pSrcDevice -> CreateTexture
				(
					Width,
					Height,
					Levels,
					Usage,
					FrameworkFormat,
					(DWORD)Pool,
					ppTexture
				)
			)
		)
	)
	{
		char szErrorMessage[cuiMaxErrorMessageLength];
		char szUsageString[cuiMaxErrorMessageLength];

		ListUsages(Usage, szUsageString);
		sprintf(szErrorMessage,
		"Texture Creation Failed.  Width: %d; Height: %d; Levels: %d; Usage: %s; Format: %s; Pool: %s\n",
		Width, Height, Levels, szUsageString, (FindFormatRecord(Format)) -> szName, (FindPoolRecord(Pool)) -> szName);
		WriteToLog(szErrorMessage);
	}

	return hresult;
}

HRESULT CCopyRects::LogCreateRenderTarget
(
	UINT Width,
	UINT Height,
	D3DFORMAT Format,
	D3DMULTISAMPLE_TYPE MultiSample,
	BOOL Lockable,
	CSurface **ppSurface
)
{
	HRESULT hresult;
	FORMAT FrameworkFormat((FMT)Format);

	if
	(
		FAILED
		(
			(
				hresult =
				m_pSrcDevice -> CreateRenderTarget
				(
					Width,
					Height,
					FrameworkFormat,
					(MULTISAMPLE_TYPE)MultiSample,
					Lockable,
					ppSurface
				)
			)
		)
	)
	{
		char szErrorMessage[cuiMaxErrorMessageLength];

		sprintf(szErrorMessage,
		"Render Target Creation Failed.  Width: %d; Height: %d; Format: %s; MultiSample: %s; Lockable: %s\n",
		Width, Height, (FindFormatRecord(Format)) -> szName, (FindMultiSampleRecord(MultiSample)) -> szName, Lockable ? "TRUE" : "FALSE");
		WriteToLog(szErrorMessage);
	}

	return hresult;
}

bool CCopyRects::TestCopyOperation(clsSourceSurfaceSet *pSourceSurfaceSet, clsDestSurfaceSet *pDestSurfaceSet, UINT uiRectSetIndex, DWORD dwTestCaseID)
{
	HRESULT hresult;

	CSurface *lpd3dsurf8Source, *lpd3dsurf8Dest, *lpd3dsurf8DestLoader,
	*lpd3dsurf8SourceRef, *lpd3dsurf8DestRef, *lpd3dsurf8DestCopy;

	static char szLogLine[cuiMaxLogLineLength];

	TestCaseOutcomeType TestCaseOutcome = TCAborted;

	DPF(2, _T("Source: %s\n"), pSourceSurfaceSet -> szDescription);
	DPF(2, _T("Destination: %s\n"), pDestSurfaceSet -> szDescription);
	DPF(2, _T("Rectangle set: %d (%s)\n"), uiRectSetIndex, RectangleSets[uiRectSetIndex].szDescription);

	//WriteToLog("In TestCopyOperation");

	// Obtain pointers to the surface interfaces.
	lpd3dsurf8Source = (pSourceSurfaceSet -> pSourceTestSurface) -> GetSurfacePointer();
	lpd3dsurf8Dest = (pDestSurfaceSet -> pDestTestSurface) -> GetSurfacePointer();
	lpd3dsurf8DestLoader = (pDestSurfaceSet -> texsurfDestLoader).GetSurfacePointer();
	lpd3dsurf8SourceRef = (pSourceSurfaceSet -> texsurfSourceLoader).GetSurfacePointer();
	lpd3dsurf8DestRef = (pDestSurfaceSet -> texsurfReference).GetSurfacePointer();
	lpd3dsurf8DestCopy = (pDestSurfaceSet -> texsurfDestCopy).GetSurfacePointer();

	// Load the background image onto the destination surface.
	if
	(
		FAILED
		(
			(
				hresult =
				//lpd3ddevice8 -> CopyRects
				m_pSrcDevice -> CopyRects
				(
					lpd3dsurf8DestLoader,
					NULL,
					0,
					lpd3dsurf8Dest,
					NULL
				)
			)
		)
	)
		WriteToLog("Unable to copy the 'background image' onto the destination surface.\n");
	else
		// Perform the tested CopyRects operation.
		if
		(
			FAILED
			(
				(
					hresult =
					//lpd3ddevice8 -> CopyRects
					m_pSrcDevice -> CopyRects
					(
						lpd3dsurf8Source,
						RectangleSets[uiRectSetIndex].rects,
						RectangleSets[uiRectSetIndex].iNoOfRects,
						lpd3dsurf8Dest,
						RectangleSets[uiRectSetIndex].points
					)
				)
			)
		)
		{
			WriteToLog("The tested CopyRects operation failed.\n");
			//strcpy(szTestRunSummary, "FAIL: A CopyRects operation failed unexpectedly.");
			//++uiTestCasesFailed;
			TestCaseOutcome = TCFailed;
		}
		else
			// Load the background image onto the destination surface for the reference copy.
			if
			(
				FAILED
				(
					(
						hresult =
						//lpd3ddevice8 -> CopyRects
						m_pSrcDevice -> CopyRects
						(
							lpd3dsurf8DestLoader,
							NULL,
							0,
							lpd3dsurf8DestRef,
							NULL
						)
					)
				)
			)
				WriteToLog("Unable to load the destination surface for the reference copy.\n");
			else
				// Perform the reference copy.
				if
				(
					!
					ReferenceCopyRects
					(
						lpd3dsurf8SourceRef,
						RectangleSets[uiRectSetIndex].rects,
						RectangleSets[uiRectSetIndex].iNoOfRects,
						lpd3dsurf8DestRef,
						RectangleSets[uiRectSetIndex].points
					)
				)
					WriteToLog("Unable to perform the reference copy owing to surface-locking failure.\n");
				else
					// Copy the result of the tested CopyRect operation onto a lockable surface
					// so that it can be compared with the result of the reference copy.
					if
					(
						FAILED
						(
							(
								hresult =
								//lpd3ddevice8 -> CopyRects
								m_pSrcDevice -> CopyRects
								(
									lpd3dsurf8Dest,
									NULL,
									0,
									lpd3dsurf8DestCopy,
									NULL
								)
							)
						)
					)
						WriteToLog("Unable to retrieve the result of the tested copy for comparison with the reference copy.\n");
					else
					{
						/*
						WriteToLog("");
						sprintf(szLogLine, "Source: %s", pSourceSurfaceSet -> szDescription);
						WriteToLog(szLogLine);
						sprintf(szLogLine, "Destination: %s", pDestSurfaceSet -> szDescription);
						WriteToLog(szLogLine);
						sprintf(szLogLine, "Rectangle set: %d (%s)", uiRectSetIndex, RectangleSets[uiRectSetIndex].szDescription);
						WriteToLog(szLogLine);
						*/

						// Compare the result of the tested CopyRects operation with that of the reference
						// copy operation.

						// AndrewLu: 03/10/2004: Use a "pixel-perfect" comparison
						// (CompareSurfaces), unless one or both surfaces are multisampled.
						// In the latter case, use the D3D framework's surface comparison
						// facilities.
						if
						(
						(((pSourceSurfaceSet -> pSourceTestSurface) -> GetMultisampleType()) != D3DMULTISAMPLE_NONE)
						||
						(((pDestSurfaceSet -> pDestTestSurface) -> GetMultisampleType()) != D3DMULTISAMPLE_NONE)
						)
						// Framework Compare
						{
							if (FAILED((TestApp.m_ImageCmp).CompareImages(lpd3dsurf8DestRef, lpd3dsurf8DestCopy, NULL)))
							{
								TestCaseOutcome = TCAborted;
								WriteToLog("Unable to compare the test and reference images.\n");
							}
							else
							{
								float fSurfaceConformanceFraction;

								fSurfaceConformanceFraction = (TestApp.m_ImageCmp).GetCompareResult();
								if (fSurfaceConformanceFraction >= fMinimumConformanceFraction)
									TestCaseOutcome = TCPassed;
								else
								{
									WriteToLog("\n");
									sprintf(szLogLine, "The test and reference images differ significantly.  (Conformance: %.4f)\n", fSurfaceConformanceFraction);
									WriteToLog(szLogLine);
									sprintf(szLogLine, "Source: %s\n", pSourceSurfaceSet -> szDescription);
									WriteToLog(szLogLine);
									sprintf(szLogLine, "Destination: %s\n", pDestSurfaceSet -> szDescription);
									WriteToLog(szLogLine);
									sprintf(szLogLine, "Rectangle set: %d (%s)\n", uiRectSetIndex, RectangleSets[uiRectSetIndex].szDescription);
									WriteToLog(szLogLine);
									TestCaseOutcome = TCFailed;
								}
							}
						}
						else
						// "Pixel Perfect" Compare
						if (!CompareSurfaces(lpd3dsurf8DestCopy, lpd3dsurf8DestRef))
						{
							WriteToLog("\n");
							WriteToLog("Discrepancy between CopyRects result and reference copy result.\n");
							sprintf(szLogLine, "Source: %s\n", pSourceSurfaceSet -> szDescription);
							WriteToLog(szLogLine);
							sprintf(szLogLine, "Destination: %s\n", pDestSurfaceSet -> szDescription);
							WriteToLog(szLogLine);
							sprintf(szLogLine, "Rectangle set: %d (%s)\n", uiRectSetIndex, RectangleSets[uiRectSetIndex].szDescription);
							WriteToLog(szLogLine);

							//sprintf(szLogLine, "Repro: Execute 'copyrects -M%d -T%d' in a %dbpp desktop mode.",
							//uiTestedAdapterOrdinal + 1, (UINT)dwTestCaseID, (pd3dfmtrecDesktop -> uiBytesPerPixel) * 8);
							//WriteToLog(szLogLine);
							//strcpy(szTestRunSummary, "FAIL: At least one test image differed from its corresponding reference image.");
							//++uiTestCasesFailed;

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

	switch (TestCaseOutcome)
	{
	case TCPassed:
		//WriteToLog("It passed!\n");
		Pass();
		break;
	case TCFailed:
		//WriteToLog("Calling Fail().");
		Fail();
		break;
	case TCAborted:
		return false;
		break;
	}

	return true;
}

bool CCopyRects::TestTerminate(void)
{
	SourceSurfaceSet.ReleaseAllSurfaces();
	DestSurfaceSet.ReleaseAllSurfaces();
	return true;
}

static bool ReferenceCopyRects
(
  CSurface *pSourceSurface,
  CONST RECT* pSourceRectsArray,
  UINT cRects,
  CSurface *pDestinationSurface,
  CONST POINT* pDestPointsArray
)
{
	bool bReturn = false;
	SURFACEDESC d3dsd;
	const D3DFORMAT_RECORD *pFormatRecord;
	UINT uiBytesPerPixel, uiCoordinatesPerBlockEdge, uiRectIndex;

	pSourceSurface -> GetDesc(&d3dsd);
	//(d3dsd.Format).d3dfFormat
	pFormatRecord = FindFormatRecord((d3dsd.Format).d3dfFormat);
	uiBytesPerPixel = pFormatRecord -> uiBytesPerPixel;
	uiCoordinatesPerBlockEdge = pFormatRecord -> uiCoordinatesPerBlockEdge;

	for (uiRectIndex = 0; uiRectIndex < cRects; ++uiRectIndex)
	{
		HRESULT hresult;
		RECT rectSource, rectDest;
		LOCKED_RECT d3dlrSource, d3dlrDest;
		UINT uiBytesPerRow, uiNoOfRows;

		rectSource = pSourceRectsArray[uiRectIndex];
		uiBytesPerRow =
		(rectSource.right - rectSource.left) * uiBytesPerPixel / uiCoordinatesPerBlockEdge;

		if (pDestPointsArray)
		{
			rectDest.left = pDestPointsArray[uiRectIndex].x;
			rectDest.top = pDestPointsArray[uiRectIndex].y;
			rectDest.right = rectDest.left + (rectSource.right - rectSource.left);
			rectDest.bottom = rectDest.top + (rectSource.bottom - rectSource.top);
		}
		else
			rectDest = rectSource;

		if (SUCCEEDED((hresult = pSourceSurface -> LockRect(&d3dlrSource, &rectSource, 0))))
		{
			if (SUCCEEDED((hresult = pDestinationSurface -> LockRect(&d3dlrDest, &rectDest, 0))))
			{
				BYTE *pvSource = (BYTE *)(d3dlrSource.pBits), *pvDest = (BYTE *)(d3dlrDest.pBits);

				for (uiNoOfRows = (rectSource.bottom - rectSource.top) / uiCoordinatesPerBlockEdge;
				uiNoOfRows; --uiNoOfRows)
				{
					memcpy((void *)pvDest, (void *)pvSource, uiBytesPerRow);
					pvSource += d3dlrSource.iPitch;
					pvDest += d3dlrDest.iPitch;
				}

				bReturn = true;

				pDestinationSurface -> UnlockRect();
			}

			pSourceSurface -> UnlockRect();
		}
	}

	return bReturn;
}

static bool CompareRows(BYTE *pRow1, BYTE *pRow2, UINT uiByteCount, DWORD dwMask)
{
	if (uiByteCount)
	{
		UINT uiMaskShift = 0;

		do
		{
			BYTE byteEffectiveMask = (BYTE)((dwMask >> uiMaskShift) & 0xff);

			if (((*pRow1) & byteEffectiveMask) != ((*pRow2) & byteEffectiveMask))
			{
				/*
				char szSeeDiscrepancy[256];

				sprintf(szSeeDiscrepancy, "\n%d vs %d\n", (int)(*pRow1), (int)(*pRow2));
				MainTestEnv.WriteToLog(szSeeDiscrepancy);
				*/
				return false;
			}
			++pRow1; ++pRow2;
			uiMaskShift += 8;
			if (uiMaskShift == 32)
				uiMaskShift = 0;
		}
		while (--uiByteCount);
	}

	return true;
}

static bool CompareSurfaces(CSurface *lpd3dsurf81, CSurface *lpd3dsurf82)
{
	bool bReturn = false;
	HRESULT hresult;
	SURFACEDESC d3dsd1, d3dsd2;

	lpd3dsurf81 -> GetDesc(&d3dsd1);
	lpd3dsurf82 -> GetDesc(&d3dsd2);

	if ((d3dsd1.dwWidth == d3dsd2.dwWidth) && (d3dsd1.dwHeight == d3dsd2.dwHeight) &&
	((d3dsd1.Format).d3dfFormat == (d3dsd2.Format).d3dfFormat))
	{
		LOCKED_RECT d3dlr1;

		if (SUCCEEDED((hresult = lpd3dsurf81 -> LockRect(&d3dlr1, NULL, 0))))
		{
			LOCKED_RECT d3dlr2;

			if (SUCCEEDED((hresult = lpd3dsurf82 -> LockRect(&d3dlr2, NULL, 0))))
			{
				const D3DFORMAT_RECORD *pFormatRecord;
				UINT uiBytesPerRow, uiCoordinatesPerBlockEdge, uiNoOfRows;
				BYTE *pv1 = (BYTE *)(d3dlr1.pBits), *pv2 = (BYTE *)(d3dlr2.pBits);
				DWORD dwMask;

				switch ((d3dsd1.Format).d3dfFormat)
				{
				case D3DFMT_X8R8G8B8:
				case D3DFMT_X8L8V8U8:
					dwMask = 0xffffff;
					break;
				case D3DFMT_X1R5G5B5:
					dwMask = 0x7fff7fff;
					break;
				case D3DFMT_X4R4G4B4:
					dwMask = 0x0fff0fff;
					break;
				default:
					dwMask = 0xffffffff;
					break;
				}

				pFormatRecord = FindFormatRecord((d3dsd1.Format).d3dfFormat);
				uiCoordinatesPerBlockEdge = pFormatRecord -> uiCoordinatesPerBlockEdge;
				uiBytesPerRow =
				d3dsd1.dwWidth * (pFormatRecord -> uiBytesPerPixel) / uiCoordinatesPerBlockEdge;

				bReturn = true;

				for (uiNoOfRows = d3dsd1.dwHeight / uiCoordinatesPerBlockEdge; uiNoOfRows; --uiNoOfRows)
				{
					/*
					if (memcmp((void *)pv1, (void *)pv2, uiBytesPerRow))
					{
						bReturn = false;
						break;
					}
					*/

					if (!CompareRows(pv1, pv2, uiBytesPerRow, dwMask))
					{
						bReturn = false;
						break;
					}

					pv1 += d3dlr1.iPitch;
					pv2 += d3dlr2.iPitch;
				}

				lpd3dsurf82 -> UnlockRect();
			}

			lpd3dsurf81 -> UnlockRect();
		}
	}

	return bReturn;
}

bool CCopyRects::DisplayFrame(void)
{
	HRESULT hresult;
	CSurface *psurfSrcBackBuffer = NULL;

	if (SUCCEEDED((hresult = m_pSrcDevice -> /*GetBackBuffer8*/GetBackBuffer(0, BACKBUFFER_TYPE_MONO, &psurfSrcBackBuffer))))
	{
		// Copy the appropriate reference image(s) to the back buffer for display.

		hresult =
		m_pSrcDevice -> D3DXLoadSurfaceFromSurface
		(
			psurfSrcBackBuffer, // -> /*GetD3DSurface8*/GetD3DSurface9(),
			NULL,
			&rectReferenceDisplay,
			((DestSurfaceSet.texsurfReference).GetSurfacePointer()), // -> /*GetD3DSurface8*/GetD3DSurface9(),
			NULL,
			NULL,
			D3DX_FILTER_LINEAR,
			0
		);

		// Copy the appropriate reference image(s) to the back buffer for display.

		hresult =
		m_pSrcDevice -> D3DXLoadSurfaceFromSurface
		(
			psurfSrcBackBuffer, // -> /*GetD3DSurface8*/GetD3DSurface9(),
			NULL,
			&rectTestDisplay,
			((DestSurfaceSet.texsurfDestCopy).GetSurfacePointer()), // -> /*GetD3DSurface8*/GetD3DSurface9(),
			NULL,
			NULL,
			D3DX_FILTER_LINEAR,
			0
		);

		psurfSrcBackBuffer -> Release();
	}

	m_pSrcDevice -> Present(NULL, NULL, NULL, NULL);

	return true;
}

// Override this to avoid incurring failures when a failing return code is actually expected.
void CCopyRects::ProcessError(HRESULT hr)
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
/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    bus.cpp

Abstract:

   Contains definitions for SPB function and callbacks

Author:

    Arvind Aiyar March 2011

Environment:

    Kernel-mode Driver Framework

--*/

#include "internal.h"

#include "bus.tmh"

NTSTATUS
SpbInitialize(
    _In_ PDEVICE_CONTEXT    FxDeviceContext,
    _In_ WDFCMRESLIST       FxResourcesRaw,
    _In_ WDFCMRESLIST       FxResourcesTranslated
    )
/*++
 
  Routine Description:

    This routine parses the list of SPB resources needed to find the
    controller driver. If successful, it opens an I/O
    target to the controller driver.

  Arguments:

    FxDeviceContext - Pointer to the current device context 
    FxResourcesRaw - list of translated hardware resources that 
        the PnP manager has assigned to the device
    FxResourcesTranslated - list of raw hardware resources that 
        the PnP manager has assigned to the device

  Return Value:

    NTSTATUS Status indicating success or failure

--*/
{
    FuncEntry(TRACE_FLAG_SPB);

    UNREFERENCED_PARAMETER(FxResourcesRaw);
    
    NTSTATUS status = STATUS_SUCCESS;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR pDescriptor = NULL;

    ULONG interruptIndex = 0;
    
    //
    // Check for SPB(I2C) and Interrupt resources
    //
    {
        BOOLEAN connFound = FALSE;
        BOOLEAN interruptFound = FALSE;
        
        ULONG resourceCount = WdfCmResourceListGetCount(FxResourcesTranslated);

        for (ULONG i=0; ((connFound == FALSE) || (interruptFound == FALSE)) && (i < resourceCount); i++)
        {
            pDescriptor = WdfCmResourceListGetDescriptor(FxResourcesTranslated, i);

            NT_ASSERTMSG("Resource descriptor is NULL", pDescriptor != NULL);

            switch(pDescriptor->Type)
            {
                case CmResourceTypeConnection:

                    //
                    // Check for I2C resource
                    //
                    if (pDescriptor->u.Connection.Class == CM_RESOURCE_CONNECTION_CLASS_SERIAL &&
                        pDescriptor->u.Connection.Type == CM_RESOURCE_CONNECTION_TYPE_SERIAL_I2C)
                    {
                        FxDeviceContext->SpbConnectionId.LowPart = pDescriptor->u.Connection.IdLowPart;
                        FxDeviceContext->SpbConnectionId.HighPart = pDescriptor->u.Connection.IdHighPart; 

                        connFound = TRUE;

                        TraceEvents(
                                TRACE_LEVEL_INFORMATION,
                                TRACE_FLAG_SPB,
                                "I2C resource found with connection id: 0x%llx",
                                FxDeviceContext->SpbConnectionId.QuadPart);
                    }

                    break;
                    
                    //
                    // Check for I2C resource
                    //
                case CmResourceTypeInterrupt:

                    interruptFound = TRUE;
                    interruptIndex = i;
                    
                    TraceEvents(
                            TRACE_LEVEL_INFORMATION,
                            TRACE_FLAG_SPB,
                            "Interrupt resource found at index: %d",interruptIndex);

                    break;

                default:
                    // Don't care about other descriptors
                    break;
            }
        }
    
        //
        // Fail if either connection or interrupt resource
        // was not found
        //
        if ((connFound == FALSE) || (interruptFound == FALSE))
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "Failed finding required resources "
                "status:%!STATUS!",
                status);
            
            DeviceReportPnpProblem(FxDeviceContext->FxDevice, HIDI2C_RESOURCE_MISSING_ERROR);

            goto exit;
        }
    }

    //
    // Create the SPB I/O target
    //
    {
        WDF_OBJECT_ATTRIBUTES targetAttributes;
        WDF_OBJECT_ATTRIBUTES_INIT(&targetAttributes);
        
        status = WdfIoTargetCreate(
                        FxDeviceContext->FxDevice,
                        &targetAttributes,
                        &FxDeviceContext->SpbIoTarget);

        if (!NT_SUCCESS(status)) 
        {
            TraceEvents(
                    TRACE_LEVEL_ERROR,
                    TRACE_FLAG_SPB,
                    "WdfIoTargetCreate failed creating SPB target "
                    "status:%!STATUS!",
                    status);

            WdfDeleteObjectSafe(FxDeviceContext->SpbIoTarget);
            goto exit;
        }

        //
        // Use the connectionId to create the full device path
        //
        DECLARE_UNICODE_STRING_SIZE(spbDevicePath, RESOURCE_HUB_PATH_SIZE);

        status = RESOURCE_HUB_CREATE_PATH_FROM_ID(
                                    &spbDevicePath,
                                    FxDeviceContext->SpbConnectionId.LowPart,
                                    FxDeviceContext->SpbConnectionId.HighPart);

        if (!NT_SUCCESS(status))
        {
            TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "ResourceHub failed to create device path (%wZ) "
                "status:%!STATUS!",
                &spbDevicePath,
                status);
         
            goto exit;
        }

        //
        // Open the SPB IoTarget
        //
        WDF_IO_TARGET_OPEN_PARAMS  openParams;
        WDF_IO_TARGET_OPEN_PARAMS_INIT_OPEN_BY_NAME(
                                    &openParams,
                                    &spbDevicePath,
                                    (GENERIC_READ | GENERIC_WRITE));

        openParams.ShareAccess = 0;
        openParams.CreateDisposition = FILE_OPEN;
        openParams.FileAttributes = FILE_ATTRIBUTE_NORMAL;

        status = WdfIoTargetOpen(
                        FxDeviceContext->SpbIoTarget,
                        &openParams);

        if (!NT_SUCCESS(status)) 
        {
            TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "WdfIoTargetOpen failed to open SPB target:0x%p "
                "status:%!STATUS!",
                FxDeviceContext->SpbIoTarget,
                status);

            goto exit;
        }
    }

    //
    // Create a SPB request for writes and reads to the SpbIoTarget
    //
    {
        //
        // Associate a request context with the request
        // SpbRequest is used for sending R/W/IOCTL to the SpbIoTarget, parent it such
        //
        WDF_OBJECT_ATTRIBUTES requestAttributes;
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&requestAttributes, REQUEST_CONTEXT);

        requestAttributes.ParentObject = FxDeviceContext->SpbIoTarget;
    
        status = WdfRequestCreate(
                            &requestAttributes,
                            FxDeviceContext->SpbIoTarget,
                            &FxDeviceContext->SpbRequest);

        if (!NT_SUCCESS(status))
        {
            TraceEvents(
 