#include <d3d9.h>
#include <d3dx9.h>
#include "MipGen.h"

#if FAST_TEST
//No need to test DepthStencil
TEXTURE_USAGE g_usages[] =
{
    { 9.0f, 9.0f, USAGE_AUTOGENMIPMAP,		_T("AutoGenMipMap") },

    { 9.0f, 9.0f, USAGE_AUTOGENMIPMAP | USAGE_RENDERTARGET,		_T("AutoGenMipMap | RenderTarget") },
    { 9.0f, 9.0f, USAGE_AUTOGENMIPMAP | USAGE_DYNAMIC,			_T("AutoGenMipMap | Dynamic") },
};	  
const UINT cUsage = sizeof(g_usages) / sizeof(g_usages[0]);

TEXTURE_POOL g_pools[] =
{
    { 8.0f, 9.0f,	POOL_DEFAULT,		_T("Default") },
    //{ 8.0f, 9.0f,	POOL_MANAGED,		_T("Managed") },
};
const UINT cPool = sizeof(g_pools) / sizeof(g_pools[0]);

TEXTURE_GENFUNC g_genFuncs[] =
{
    { 8.0f, 9.0f, FUNC_LOCK,				_T("LockRect") },
    //{ 8.0f, 9.0f, FUNC_GETDC,				_T("GetDC") },
    { 8.0f, 9.0f, FUNC_UPDATESURFACE,		_T("UpdateSurface") },
    { 9.0f, 9.0f, FUNC_STRETCHRECT,			_T("StretchRect") },
    //{ 9.0f, 9.0f, FUNC_COLORFILL,			_T("ColorFill") },
    { 8.0f, 9.0f, FUNC_UPDATETEXTURE,		_T("UpdateTexture") },
    { 8.0f, 9.0f, FUNC_SETRENDERTARGET,		_T("SetRenderTarget") },
    //{ 9.0f, 9.0f, FUNC_LOSTDEVICE,			_T("LostDevice") },
    //{ 9.0f, 9.0f, FUNC_EVICTMANAGED,		_T("EvictManagedRessources") },
};	  
const UINT cGenFunc = sizeof(g_genFuncs) / sizeof(g_genFuncs[0]);
#else //FAST_TEST
//No need to test DepthStencil
TEXTURE_USAGE g_usages[] =
{
	{ 9.0f, 9.0f, USAGE_AUTOGENMIPMAP,		_T("AutoGenMipMap") },

	{ 9.0f, 9.0f, USAGE_AUTOGENMIPMAP | USAGE_RENDERTARGET,		_T("AutoGenMipMap | RenderTarget") },
	{ 9.0f, 9.0f, USAGE_AUTOGENMIPMAP | USAGE_DYNAMIC,			_T("AutoGenMipMap | Dynamic") },
};	  
const UINT cUsage = sizeof(g_usages) / sizeof(g_usages[0]);

TEXTURE_POOL g_pools[] =
{
	{ 8.0f, 9.0f,	POOL_DEFAULT,		_T("Default") },
	{ 8.0f, 9.0f,	POOL_MANAGED,		_T("Managed") },
};
const UINT cPool = sizeof(g_pools) / sizeof(g_pools[0]);

TEXTURE_GENFUNC g_genFuncs[] =
{
	{ 8.0f, 9.0f, FUNC_LOCK,				_T("LockRect") },
	//{ 8.0f, 9.0f, FUNC_GETDC,				_T("GetDC") },
	{ 8.0f, 9.0f, FUNC_UPDATESURFACE,		_T("UpdateSurface") },
	{ 9.0f, 9.0f, FUNC_STRETCHRECT,			_T("StretchRect") },
	//{ 9.0f, 9.0f, FUNC_COLORFILL,			_T("ColorFill") },
	{ 8.0f, 9.0f, FUNC_UPDATETEXTURE,		_T("UpdateTexture") },
	{ 8.0f, 9.0f, FUNC_SETRENDERTARGET,		_T("SetRenderTarget") },
	//{ 9.0f, 9.0f, FUNC_LOSTDEVICE,			_T("LostDevice") },
	{ 9.0f, 9.0f, FUNC_EVICTMANAGED,		_T("EvictManagedRessources") },
};	  
const UINT cGenFunc = sizeof(g_genFuncs) / sizeof(g_genFuncs[0]);
#endif //FAST_TEST

CMipGenHardware::CMipGenHardware()
{
	SetTestType(TESTTYPE_CONF);

	// Init framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
	m_Options.D3DOptions.dwVPEmuRegChange = VPEMUREGCHANGE_NONE;
	m_Options.D3DOptions.dwPPEmuRegChange = VPEMUREGCHANGE_NONE;

	m_Options.D3DOptions.dwRefDevType = DEVICETYPE_HAL;
	m_Options.D3DOptions.dwRefVPEmuRegChange = VPEMUREGCHANGE_NONE;
	m_Options.D3DOptions.dwRefPPEmuRegChange = VPEMUREGCHANGE_NONE;

	m_Options.D3DOptions.bRefEqualsSrc = true;
	m_Options.D3DOptions.bZBuffer = false;

	// Initialize some variables
	m_pSrcTexture = NULL;
	m_pRefTexture = NULL;
	m_pColorFilledTexture = NULL;
	m_pColorFilledSurface = NULL;
	m_bCreatePatternFailed = false;
	m_bColorFillTopLevelFailed = false;
	m_bSetAutoGenFailed = false;
}

CMipGenHardware::~CMipGenHardware()
{
}

bool CMipGenHardware::CapsCheck()
{
	if (! CD3DTest::CapsCheck() )
		return false;

	bool bGen = (0 != (m_pSrcDevice->GetCaps()->dwCaps2 & D3DCAPS2_CANAUTOGENMIPMAP));
	if (!bGen)
		return false;

	return true;
}

bool CMipGenHardware::ExecuteTest(UINT uTestNum)
{
	HRESULT hr;

	m_uCurrentFrame = (uTestNum - 1) % 9;
	m_uCurrentCase = (uTestNum - 1) / 9;

	m_pCurrentCase = (TEXTURE_CASE*)m_listCases[ m_uCurrentCase ];

	TCHAR szTestName[512];
	sprintf(szTestName, "%s, %s, %s, %s, %s, %s, %s", m_pCurrentCase->szPool, 
												m_pCurrentCase->szUsage, 
												m_pCurrentCase->szFormat, 
												m_pCurrentCase->szPattern,
												m_pCurrentCase->szAutoGenFilter,
												m_pCurrentCase->szGenFunc,
												m_pCurrentCase->szDisplayFilter);

	// Begin the test case
    BeginTestCase(szTestName, uTestNum);

	Pass(); //To make sure Skip is not set after a D3DERR_OUTOFVIDMEM

	if (!m_bRenderEachMip && m_uCurrentFrame == 1)
	{
		MuteLog();
		SkipTests(8);
		UnMuteLog();
		return false;
	}

	//Only change the texture on frame 0
	if (m_uCurrentFrame == 0)
	{
		//Set this so that if texture creation fails, the use of colorfilled and 
		//pattern surfaces won't fault
		if (m_pCurrentCase->bCreatePatternSurface) 
			m_bCreatePatternFailed = true;

		if (m_pCurrentCase->bColorFillTopLevel) 
			m_bColorFillTopLevelFailed = true;

		if (m_pCurrentCase->bSetAutoGenFilter) 
			m_bSetAutoGenFailed = true;

		//Create the texture if needed
		if (m_pCurrentCase->bCreateTexture || !m_pSrcTexture || !m_pRefTexture || !m_pColorFilledTexture || !m_pColorFilledSurface)
		{
			//When the format changes, create a new color filled surface
			if (! CreateColorFilledSurface() )
				goto Fail;

			//without this, the texture will only be released after the next SetTexture, causing memory problems
			m_pSrcDevice->SetTexture(0, NULL);
			m_pRefDevice->SetTexture(0, NULL);

			// Create the texture
			if (! CreateBaseTexture() )
			{
				if (m_pSrcTexture)
					m_pSrcDevice->SetTexture(0, m_pSrcTexture);
				goto Fail;
			}
			
			m_pSrcDevice->SetTexture(0, m_pSrcTexture);
			m_pRefDevice->SetTexture(0, m_pRefTexture);
		}

		if (m_pCurrentCase->bCreatePatternSurface || m_bCreatePatternFailed)
		{
			if (! CreatePatternSurface() )
				goto Fail;
		}

		if (m_pCurrentCase->bColorFillTopLevel || m_bColorFillTopLevelFailed)
		{
			if (! ColorFillTopLevel())
				goto Fail;

			RenderScene();
			DWORD dwPasses = 0;
			ValidateDevice(&dwPasses);
		}

		//Set the AutoGenFilter if needed
		if (m_pCurrentCase->bSetAutoGenFilter || m_bSetAutoGenFailed)
		{
			if (!SetAutoGenFilter())
				goto Fail;

			bool bResult = false;
			switch (m_pCurrentCase->dwGenFunc)
			{
				case FUNC_LOCK :
					bResult = MethodLock();
					break;

				case FUNC_COLORFILL :
					bResult = MethodColorFill();
					break;

				case FUNC_GETDC :
					bResult = MethodGetDC();
					break;

				case FUNC_UPDATESURFACE :
					bResult = MethodUpdateSurface();
					break;

				case FUNC_UPDATETEXTURE :
					bResult = MethodUpdateTexture();
					break;

				case FUNC_STRETCHRECT :
					bResult = MethodStretchRect();
					break;

				case FUNC_SETRENDERTARGET :
					bResult = MethodSetRenderTarget();
					break;

				case FUNC_EVICTMANAGED :
					bResult = MethodEvictManagedResources();
					break;

				default :
					WriteToLog("Unsupported Generation function");
					break;
			}
			if (! bResult )
 			{
				WriteToLog("ExecuteTest() - Method%s failed\n", m_pCurrentCase->szGenFunc);
				goto Fail;
			}

			//render the miplevels on Ref using technique 3
			if (! RenderRefMipLevels() )
				goto Fail;
		}

		SetSamplerState(0, SAMP_MIPFILTER, TEXF_POINT);
		SetSamplerState(0, SAMP_MINFILTER, m_pCurrentCase->dwDisplayFilter);
		if ( m_bMagTexFilSupported[m_pCurrentCase->uDisplayFilterIndex] && m_bMagTexFilSupportedRef[m_pCurrentCase->uDisplayFilterIndex] )
			SetSamplerState(0, SAMP_MAGFILTER, m_pCurrentCase->dwDisplayFilter);
	}

	RenderScene();

	//DisplayFrame();
	//RenderScene();
	//DisplayFrame();

	GetLastError();

	return true;	

Fail:

	if (m_pFramework->m_DXGLog.GetCurrentResult() != SKIP)
		Fail();
	DumpTestDetails();
	return false;
}

bool CMipGenHardware::TestTerminate(void)
{
	// Cleanup textures
	SetTexture(0, NULL);
    RELEASE(m_pSrcTexture);
    RELEASE(m_pRefTexture);
	RELEASE(m_pColorFilledTexture);
	RELEASE(m_pColorFilledSurface);
	ReleasePatternSurface();

	return CMipGenTest::TestTerminate();
}

void CMipGenHardware::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();

	// Tell status window what Filters we are using
	WriteStatus("$wPool\t",m_pCurrentCase->szPool);
	WriteStatus("$wUsage\t",m_pCurrentCase->szUsage);
	WriteStatus("$wFormat\t",m_pCurrentCase->szFormat);
	WriteStatus("$wAutoGen Filter",m_pCurrentCase->szAutoGenFilter);
	WriteStatus("$wDisplay Filter",m_pCurrentCase->szDisplayFilter);
	WriteStatus("$wGen Function",m_pCurrentCase->szGenFunc);
	//WriteStatus("$wTexture Type",m_szTextureType);
	WriteStatus("$wPattern\t",m_pCurrentCase->szPattern);
	if (m_uCurrentFrame == 0)
		WriteStatus("$wMipLevel",_T("All"));
	else
		WriteStatus("$wMipLevel",_T("%d"), m_uCurrentMipLevel);
}

bool CMipGenHardware::CreateColorFilledSurface()
{
	RELEASE(m_pColorFilledSurface);
	RELEASE(m_pColorFilledTexture);

	if (FAILED(m_pSrcDevice->CreateTexture(256, 256, 1, 0, m_pCurrentCase->format, POOL_SYSTEMMEM, &m_pColorFilledTexture)))
	{
		WriteToLog("CreateColorFilledSurface() - CreateTexture(m_pColorFilledTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		return false;
	}
	//Get the top level of the texture
	if (FAILED(m_pColorFilledTexture->GetSurfaceLevel(0, &m_pColorFilledSurface)))
	{
		WriteToLog("CreateColorFilledSurface() - GetSurfaceLevel(m_pColorFilledSurface) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		RELEASE(m_pColorFilledTexture);
		return false;
	}

	if (!WritePatternOnSurface(m_pColorFilledSurface, PATTERN_COLORFILL))
	{
		WriteToLog("CreateColorFilledSurface() - WritePatternOnSurface() failed.\n");
		return false;
	}

	return true;
}

bool CMipGenHardware::CopyFromSurfAToSurfB(LPSURFACE pSrcSurf, LPSURFACE pDestSurf, bool bUseSubRect)
{
    LOCKED_RECT RectA, RectB;
	RECT rect = {0, 0, 256, 256};
	if (bUseSubRect)
	{
		rect.left = rect.top = 64;
		rect.right = rect.bottom = 192;
	}

    if ( FAILED( pSrcSurf->LockRect(&RectA, &rect, 0) ) )
	{
		WriteToLog("CopyFromSurfAToSurfB() - LockRect(pSrc) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		return false;
	}

    if ( FAILED( pDestSurf->LockRect(&RectB, &rect, 0) ) )
	{
		WriteToLog("CopyFromSurfAToSurfB() - LockRect(pDest) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		return false;
	}

	//Copy A to B
	DWORD   dwY=0;
	DWORD   dwX;
	PBYTE   pSrc=(PBYTE)RectA.pBits;
	PBYTE	pDest=(PBYTE)RectB.pBits;
	DWORD	dwWidth = rect.right - rect.left;
	DWORD	WidthSize = dwWidth * m_pCurrentCase->uBPP;

	do
	{
		if ( !memcpy(pDest, pSrc, WidthSize) )
		{
			return false;
		}

		// Increment to the next line.

		pSrc += RectA.iPitch;
		pDest += RectB.iPitch;

		dwY++;
	}
	while (dwY < dwWidth);

    if ( FAILED( pSrcSurf->UnlockRect() ) )
	{
		WriteToLog("CopyFromSurfAToSurfB() - UnlockRect(pSrc) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		return false;
	}

    if ( FAILED( pDestSurf->UnlockRect() ) )
	{
		WriteToLog("CopyFromSurfAToSurfB() - UnlockRect(pDest) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		return false;
	}

	return true;
}

bool FormatSupportedByColorFill(FMT format)
{
	switch(format)
	{
		case FMT_X1R5G5B5 : 
		case FMT_A1R5G5B5 : 
		case FMT_R5G6B5 : 
		case FMT_X8R8G8B8 : 
		case FMT_A8R8G8B8 : 
		case FMT_YUY2 :
		case FMT_G8R8_G8B8 :
		case FMT_UYVY :
		case FMT_R8G8_B8G8 : 
		case FMT_R16F :
		case FMT_G16R16F : 
		case FMT_A16B16G16R16F : 
		case FMT_R32F : 
		case FMT_G32R32F :
		case FMT_A32B32G32R32F :
			return true;

		default :
			return false;
	}

	return false;
}


void CMipGenHardware::GenerateCaseList()
{
	std::vector <TEXTURE_CASE *>::iterator caseIter;

	TEXTURE_CASE *pTexCase = NULL;

	bool bCreateTexture = false;
	bool bSetAutoGenFilter = false;
	bool bCreatePatternSurface = false;
	bool bColorFillTopLevel = false;
	bool bLockable = true;
	bool bUseSubRect = false;
	bool bUseOffscreenPlain;
	bool bUseRTTexture;
	bool bUsePlainRT;
	bool bUseFormatConversion;
	bool bUseOnlyPointFilter;

	//Check if we override the pool to test
	TCHAR	szPool[512] = "";
	DWORD	dwPoolToTest = 0xffffffff;
	if(ReadString(_T("POOL"), szPool, ARRAYSIZE(szPool)))
	{
		// Convert to uppercase
		_tcsupr(szPool);

		if(_tcsstr(szPool, _T("DEFAULT")))
		{
			dwPoolToTest = POOL_DEFAULT;
		}
		else if(_tcsstr(szPool, _T("MANAGED")))
		{
			dwPoolToTest = POOL_MANAGED;
		}
	}

	//Check if we override the usage to test
	TCHAR	szUsage[512] = "";
	DWORD	dwUsageToTest = 0xffffffff;
	if(ReadString(_T("USAGE"), szUsage, ARRAYSIZE(szUsage)))
	{
		// Convert to uppercase
		_tcsupr(szUsage);

		if(_tcsstr(szUsage, _T("RENDERTARGET")))
		{
			dwUsageToTest = USAGE_RENDERTARGET;
		}
		else if(_tcsstr(szUsage, _T("DYNAMIC")))
		{
			dwUsageToTest = USAGE_DYNAMIC;
		}
	}

	//Check if we override the pattern to test
	TCHAR	szPattern[512] = "";
	DWORD	dwPatternToTest = 0xffffffff;
	if(ReadString(_T("PATTERN"), szPattern, ARRAYSIZE(szPattern)))
	{
		// Convert to uppercase
		_tcsupr(szPattern);

		if(_tcsstr(szPattern, _T("1X1")))
		{
			dwPatternToTest = PATTERN_1X1;
		}
		else if(_tcsstr(szPattern, _T("16X16")))
		{
			dwPatternToTest = PATTERN_16X16;
		}
		else if(_tcsstr(szPattern, _T("17X17")))
		{
			dwPatternToTest = PATTERN_17X17;
		}
		else if(_tcsstr(szPattern, _T("FUNKYCHECKER")))
		{
			dwPatternToTest = PATTERN_FUNKYCHECKER;
		}
		else if(_tcsstr(szPattern, _T("DIAGONALS")))
		{
			dwPatternToTest = PATTERN_DIAGONALS;
		}
	}

	//Check if we override the gen function to test
	TCHAR	szGenFunc[512] = "";
	DWORD	dwGenFuncToTest = 0xffffffff;
	if(ReadString(_T("GENFUNC"), szGenFunc, ARRAYSIZE(szGenFunc)))
	{
		// Convert to uppercase
		_tcsupr(szGenFunc);

		if(_tcsstr(szGenFunc, _T("LOCK")))
		{
			dwGenFuncToTest = FUNC_LOCK;
		}
		else if(_tcsstr(szGenFunc, _T("GETDC")))
		{
			dwGenFuncToTest = FUNC_GETDC;
		}
		else if(_tcsstr(szGenFunc, _T("UPDATESURFACE")))
		{
			dwGenFuncToTest = FUNC_UPDATESURFACE;
		}
		else if(_tcsstr(szGenFunc, _T("UPDATETEXTURE")))
		{
			dwGenFuncToTest = FUNC_UPDATETEXTURE;
		}
		else if(_tcsstr(szGenFunc, _T("SETRENDERTARGET")))
		{
			dwGenFuncToTest = FUNC_SETRENDERTARGET;
		}
		else if(_tcsstr(szGenFunc, _T("STRETCHRECT")))
		{
			dwGenFuncToTest = FUNC_STRETCHRECT;
		}
		else if(_tcsstr(szGenFunc, _T("COLORFILL")))
		{
			dwGenFuncToTest = FUNC_COLORFILL;
		}
		else if(_tcsstr(szGenFunc, _T("n")))
		{
			dwGenFuncToTest = FUNC_EVICTMANAGED;
		}
		else if(_tcsstr(szGenFunc, _T("LOSTDEVICE")))
		{
			dwGenFuncToTest = FUNC_LOSTDEVICE;
		}
	}

	//Check if we override the format to test
	TCHAR	szFormat[512] = "";
	DWORD	dwFormatToTest = 0xffffffff;
	if(ReadString(_T("FORMAT"), szFormat, ARRAYSIZE(szFormat)))
	{
		// Convert to uppercase
		_tcsupr(szFormat);

		for (int iFormat=0; iFormat<cFormat; iFormat++)
		{
			if(_tcsstr(szFormat, g_formats[iFormat].szFormat))
			{
				dwFormatToTest = g_formats[iFormat].format;
				break;
			}
		}
	}

	for (int iPool=0; iPool<cPool; iPool++)
	{
		//Make sure the poole is available on this DX version
		if ( (m_fDXVersion < g_pools[iPool].fMinDX) || (m_fDXVersion > g_pools[iPool].fMaxDX) )
			continue;

		//Make sure we want to test this pool
		if ( (dwPoolToTest != 0xffffffff) && (g_pools[iPool].dwPool != dwPoolToTest) )
			continue;

		for (int iUsage=0; iUsage<cUsage; iUsage++)
		{
			//Make sure the usage is available on this DX version
			if ( (m_fDXVersion < g_usages[iUsage].fMinDX) || (m_fDXVersion > g_usages[iUsage].fMaxDX) )
				continue;

			//Make sure we want to test this usage
			if ( (dwUsageToTest != 0xffffffff) && !(g_usages[iUsage].dwUsage & dwUsageToTest) )
				continue;

			//Dynamic textures not allowed on POOL_MANAGED
			if ( (g_usages[iUsage].dwUsage & USAGE_DYNAMIC) && (iPool == POOL_MANAGED) )
				continue;

			//RenderTarget and DepthStencil must be in POOL_DEFAULT
			if ( (g_usages[iUsage].dwUsage & (USAGE_RENDERTARGET | USAGE_DEPTHSTENCIL)) && (iPool != POOL_DEFAULT) )
				continue;

			//Make sure that the hardware supports Dynamic textures
			if ( (g_usages[iUsage].dwUsage & USAGE_DYNAMIC) && (0 == (m_pSrcDevice->GetCaps()->dwCaps2 & D3DCAPS2_DYNAMICTEXTURES)) )
				continue;

			//On pool Default, only Dynamic textures are lockable
			if ( !(g_usages[iUsage].dwUsage & USAGE_DYNAMIC) && (iPool == POOL_DEFAULT) )
				bLockable = false;

			for (int iFormat=0; iFormat<cFormat; iFormat++)
			{
				//Check if format was specified on command line
				if ( (dwFormatToTest != 0xffffffff) && (dwFormatToTest != g_formats[iFormat].format) )
					continue;

				//Make sure the usage is available on this DX version
				if ( (m_fDXVersion < g_formats[iFormat].fMinDX) || (m_fDXVersion > g_formats[iFormat].fMaxDX) )
					continue;

				//Check if we only want to test 1 format (from command line)
				if ( (m_Options.ModeOptions.TextureFmt != FMT_ANY) && (m_Options.ModeOptions.TextureFmt != g_formats[iFormat].format) )
					continue;

				//Multiple element textures cannot be mip-maps
				if ( g_formats[iFormat].format == FMT_MULTI2_ARGB8 )
					continue;

				// AndrewLu; 07/02/2004; If the runtime version is DX9.0c or later, check the
				// D3DUSAGE_QUERY_WRAPANDMIP flag too.
				DWORD dwAdditionalUsageFlags = 0;
				if ((m_pD3D -> GetRuntimeVersion()) >= 9.029f)
					dwAdditionalUsageFlags = USAGE_QUERY_WRAPANDMIP;

				//Is format supported for this usage
				HRESULT hr = m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(),
														m_pSrcDevice->GetDevType(),
														m_pCurrentMode->pDisplay->Format,
														g_usages[iUsage].dwUsage | dwAdditionalUsageFlags,
														m_RTYPECurrent,
														g_formats[iFormat].format);

				//If the format is not available for this usage, don't test it
				if (FAILED(hr) || hr == D3DOK_NOAUTOGEN)
					continue;

				//If the format is not supported as a render target for the ref dev, don't test it
				hr = m_pD3D->CheckDeviceFormat( m_pRefDevice->GetAdapterID(),
												m_pRefDevice->GetDevType(),
												m_pCurrentMode->pDisplay->Format,
												USAGE_RENDERTARGET,
												RTYPE_TEXTURE,
												g_formats[iFormat].format);

				//If the format is not available for this usage, don't test it
				if (FAILED(hr))
					continue;

				//Create a new texture every time the format, pool or usage changes
				bCreateTexture = true;

				//Is format supported for OffscreenPlain
				hr = m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(),
												m_pSrcDevice->GetDevType(),
												m_pCurrentMode->pDisplay->Format,
												0,
												RTYPE_SURFACE,
												g_formats[iFormat].format);

				bUseOffscreenPlain = SUCCEEDED(hr);

				//Is format supported for plain rendertarget
				hr = m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(),
												m_pSrcDevice->GetDevType(),
												m_pCurrentMode->pDisplay->Format,
												USAGE_RENDERTARGET,
												RTYPE_SURFACE,
												g_formats[iFormat].format);

				bUsePlainRT = SUCCEEDED(hr);

				//Is format supported for rendertarget texture
				hr = m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(),
												m_pSrcDevice->GetDevType(),
												m_pCurrentMode->pDisplay->Format,
												USAGE_RENDERTARGET,
												RTYPE_TEXTURE,
												g_formats[iFormat].format);

				bUseRTTexture = SUCCEEDED(hr);

				//Is format supported for filter types other than TEXF_POINT
				hr = m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(),
														m_pSrcDevice->GetDevType(),
														m_pCurrentMode->pDisplay->Format,
														USAGE_QUERY_FILTER,
														m_RTYPECurrent,
														g_formats[iFormat].format);

				bUseOnlyPointFilter = FAILED(hr);

				//Is format conversion supported
				hr = m_pD3D->CheckDeviceFormatConversion( m_pSrcDevice->GetAdapterID(),
												m_pSrcDevice->GetDevType(),
												g_formats[iFormat].format,
												g_formats[iFormat].format);

				bUseFormatConversion = SUCCEEDED(hr);

				for (int iPattern=0; iPattern<cPattern; iPattern++)
				{
					bCreatePatternSurface = true;

					//Make sure we want to test this pattern
					if ( ! (g_patterns[iPattern].dwPattern & dwPatternToTest) )
						continue;

					for (int iGenFunc=0; iGenFunc<cGenFunc; iGenFunc++)
					{
						//Make sure the generation function is available on this DX version
						if ( (m_fDXVersion < g_genFuncs[iGenFunc].fMinDX) || (m_fDXVersion > g_genFuncs[iGenFunc].fMaxDX) )
							continue;
						
						//Make sure we want to test this gen function
						if ( ! (g_genFuncs[iGenFunc].dwGenFunc & dwGenFuncToTest) )
							continue;

						//Fill the top level with a color before each generation function 
						//(to make sure the mips are generated)
						bColorFillTopLevel = true;

						switch (g_genFuncs[iGenFunc].dwGenFunc)
						{
							case FUNC_LOCK :
							case FUNC_GETDC :
								bUseSubRect = false;
								//for the texture to be lockable, it must be dynamic or in managed pool
								if ( !(g_usages[iUsage].dwUsage & USAGE_DYNAMIC) && (iPool == POOL_DEFAULT) )
									continue;
								break;

							case FUNC_COLORFILL :
								bUseSubRect = true;
								//Make sure the format is supported by ColorFill
								if ( !FormatSupportedByColorFill(g_formats[iFormat].format) )
									continue;
								//ColorFill and StretchRect needs RENDERTARGET texture for dest.
								if ( !(g_usages[iUsage].dwUsage & USAGE_RENDERTARGET) )
									continue;
								if ( iPool != POOL_DEFAULT )
									continue;
								break;
							case FUNC_STRETCHRECT :
								bUseSubRect = true;
								//ColorFill and StretchRect needs RENDERTARGET texture for dest.
								if ( !(g_usages[iUsage].dwUsage & USAGE_RENDERTARGET) )
									continue;
								if ( !bUseFormatConversion )
									continue;
								if ( iPool != POOL_DEFAULT )
									continue;
								break;

							case FUNC_UPDATESURFACE :
								bUseSubRect = true;
								//These 5 methods are only valid on Default pool
								if ( iPool != POOL_DEFAULT )
									continue;
								break;

							case FUNC_UPDATETEXTURE :
								bUseSubRect = false;
								//These 5 methods are only valid on Default pool
								if ( iPool != POOL_DEFAULT )
									continue;
								break;

							case FUNC_SETRENDERTARGET :
								bUseSubRect = false;
								//Obviously needs RENDERTARGET usage.
								if ( !(g_usages[iUsage].dwUsage & USAGE_RENDERTARGET) )
									continue;
								//These 5 methods are only valid on Default pool
								if ( iPool != POOL_DEFAULT )
									continue;
								break;

							case FUNC_EVICTMANAGED :
								bUseSubRect = false;
								if ( iPool != POOL_MANAGED )
									continue;
								break;

							default :
								continue;
						}

						for (int iAutoGenFilter=0; iAutoGenFilter<cFilter; iAutoGenFilter++)
						{
							//Make sure the filter is available on this DX version
							if ( (m_fDXVersion < g_filters[iAutoGenFilter].fMinDX) || (m_fDXVersion > g_filters[iAutoGenFilter].fMaxDX) )
								continue;

							//If the filter is not supported by driver or ref, don't test it
							if (!m_bTexFilSupported[iAutoGenFilter] || !m_bTexFilSupportedRef[iAutoGenFilter])
								continue;

							//Does the format supports only point filter
							if (bUseOnlyPointFilter && g_filters[iAutoGenFilter].dwFilter != TEXF_POINT)
								continue;

							//Do not test R3G3B2 with Pyramidal or Gaussian Filter, There are precision problems
							if ( ((g_filters[iAutoGenFilter].dwFilter == TEXF_GAUSSIANQUAD) || (g_filters[iAutoGenFilter].dwFilter == TEXF_PYRAMIDALQUAD)) &&
								((g_formats[iFormat].format == FMT_R3G3B2) || (g_formats[iFormat].format == FMT_A8R3G3B2)) )
								continue;

							//Set the auto gen filter every time the filter changes.
							bSetAutoGenFilter = true;

							for (int iDisplayFilter=0; iDisplayFilter<cFilter; iDisplayFilter++)
							{
								//Make sure the filter is available on this DX version
								if ( (m_fDXVersion < g_filters[iDisplayFilter].fMinDX) || (m_fDXVersion > g_filters[iDisplayFilter].fMaxDX) )
									continue;

								//If the filter is not supported by driver or ref, don't test it
								if (!m_bTexFilSupported[iDisplayFilter] || !m_bTexFilSupportedRef[iDisplayFilter])
									continue;

								//Does the format supports only point filter
								if (bUseOnlyPointFilter && g_filters[iDisplayFilter].dwFilter != TEXF_POINT)
									continue;

								//Create a new Texture case
								pTexCase = new TEXTURE_CASE;
								ZeroMemory(pTexCase, sizeof(pTexCase));

								pTexCase->dwUsage = g_usages[iUsage].dwUsage;
								_tcscpy(pTexCase->szUsage, g_usages[iUsage].szUsage);

								pTexCase->format = g_formats[iFormat].format;
								pTexCase->uBPP = g_formats[iFormat].uBPP;
								_tcscpy(pTexCase->szFormat, g_formats[iFormat].szFormat);

								pTexCase->dwPool = g_pools[iPool].dwPool;
								_tcscpy(pTexCase->szPool, g_pools[iPool].szPool);

								pTexCase->dwPattern = g_patterns[iPattern].dwPattern;
								_tcscpy(pTexCase->szPattern, g_patterns[iPattern].szPattern);

								pTexCase->dwAutoGenFilter = g_filters[iAutoGenFilter].dwFilter;
								pTexCase->uAutoGenFilterIndex = g_filters[iAutoGenFilter].uIndex;
								_tcscpy(pTexCase->szAutoGenFilter, g_filters[iAutoGenFilter].szFilter);

								pTexCase->dwDisplayFilter = g_filters[iDisplayFilter].dwFilter;
								pTexCase->uDisplayFilterIndex = g_filters[iDisplayFilter].uIndex;
								_tcscpy(pTexCase->szDisplayFilter, g_filters[iDisplayFilter].szFilter);

								pTexCase->dwGenFunc = g_genFuncs[iGenFunc].dwGenFunc;
								_tcscpy(pTexCase->szGenFunc, g_genFuncs[iGenFunc].szGenFunc);

								pTexCase->bCreateTexture = bCreateTexture;
								pTexCase->bCreatePatternSurface = bCreatePatternSurface;
								pTexCase->bSetAutoGenFilter = bSetAutoGenFilter;
								pTexCase->bLockable = bLockable;
								pTexCase->bColorFillTopLevel = bColorFillTopLevel;
								pTexCase->bUseRTTexture = bUseRTTexture;
								pTexCase->bUsePlainRT = bUsePlainRT;
								pTexCase->bUseOffscreenPlain = bUseOffscreenPlain;
								pTexCase->bUseSubRect = bUseSubRect;

								bLockable = true;
								bSetAutoGenFilter = false;
								bCreateTexture = false;
								bCreatePatternSurface = false;
								bColorFillTopLevel = false;

								//Add the case to the list
								m_listCases.push_back(pTexCase);
							}
						}
					}
				}
			}
		}
	}
}

void CMipGenHardware::DumpTestDetails()
{
	WriteToLog( "Pool            : %s\n", m_pCurrentCase->szPool );
	WriteToLog( "Usage           : %s\n", m_pCurrentCase->szUsage );
	WriteToLog( "Format          : %s\n", m_pCurrentCase->szFormat );
	WriteToLog( "AutoGen Filter  : %s\n", m_pCurrentCase->szAutoGenFilter );
	WriteToLog( "Display Filter  : %s\n", m_pCurrentCase->szDisplayFilter );
	WriteToLog( "Gen Function    : %s\n", m_pCurrentCase->szGenFunc );
	WriteToLog( "Pattern         : %s\n", m_pCurrentCase->szPattern );
	if (m_uCurrentFrame == 0)
		WriteToLog( "MipLevel        : All\n" );
	else
		WriteToLog( "MipLevel        : %d\n", m_uCurrentMipLevel );
}


void CMipGenHardware::CommandLineHelp()
{
	WriteCommandLineHelp("$y-POOL:$rpool\t$wOnly run on this pool.");
	WriteCommandLineHelp("   $rpool $wcan be :");
	WriteCommandLineHelp("\t$rDefault\t\t$wDefault pool");
	WriteCommandLineHelp("\t$rManaged\t$wManaged pool");
	WriteCommandLineHelp("$y-!POOL:$rpool\t$wExclude the pool.");
	WriteCommandLineHelp("\n");

	WriteCommandLineHelp("$y-GENFUNC:$rfunction\t$wOnly generate top level with this function.");
	WriteCommandLineHelp("   $rfunction $wcan be :");
	WriteCommandLineHelp("\t$rLock\t\t$wLock and copy");
	WriteCommandLineHelp("\t$rGetDC\t\t$wGetDC");
	WriteCommandLineHelp("\t$rUpdateSurface\t$wUpdateSurface");
	WriteCommandLineHelp("\t$rUpdateTexture\t$wUpdateTexture");
	WriteCommandLineHelp("\t$rStretchRect\t$wStretchRect");
//	WriteCommandLineHelp("\t$rColorFill\t\t$wColorFill");
	WriteCommandLineHelp("\t$rEvictManagedResources\t$wLock and Copy then evict the default");
//	WriteCommandLineHelp("\t$rLostDevice\t$wLock and Copy then lose the device");
	WriteCommandLineHelp("$y-!GENFUNC:$rfunction\t$wExclude this function to generate the top level.");
	WriteCommandLineHelp("\n");

	WriteCommandLineHelp("$y-PATTERN:$rpattern\t$wUse this pattern for the top level.");
	WriteCommandLineHelp("   $rpattern $wcan be :");
	WriteCommandLineHelp("\t$r1x1\t\t$wUse a 1x1 pattern");
//	WriteCommandLineHelp("\t$r16x16\t\t$wUse a 16x16 pattern");
	WriteCommandLineHelp("\t$r17x17\t\t$wUse a 17x17 pattern");
//	WriteCommandLineHelp("\t$rFunkyChecker\t\t$wUse a Funky Checker pattern");
//	WriteCommandLineHelp("\t$rDiagonals\t\t$wUse a Diagonals pattern");
	WriteCommandLineHelp("$y-!PATTERN:$rpattern\t$wExclude this pattern.");
	WriteCommandLineHelp("\n");

    WriteCommandLineHelp("$y-ShowTopLevel\t$wShow the top level");
}
