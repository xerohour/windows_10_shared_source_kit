#include <d3d9.h>
#include <d3dx9.h>
#include "MipGen.h"

CMipGenEmulation::CMipGenEmulation()
{
    // Init framework options
    SetTestType(TESTTYPE_CONF);
	m_Options.D3DOptions.fMinDXVersion = 8.0f;

    //m_Options.D3DOptions.bReference = false;
    m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
	m_Options.D3DOptions.bZBuffer = false;

	// Initialize some variables
    m_pTextures = NULL;
}

CMipGenEmulation::~CMipGenEmulation()
{
}

bool CMipGenEmulation::ExecuteTest(UINT uTestNum)
{

	HRESULT hr;
	m_uCurrentFrame = (uTestNum - 1) % 9;
	m_uCurrentCase = (uTestNum - 1) / 9;

	m_pCurrentCase = (TEXTURE_CASE*)m_listCases[ m_uCurrentCase ];

	TCHAR szTestName[512];
	sprintf(szTestName, "%s, %s, %s",	m_pCurrentCase->szFormat, 
											m_pCurrentCase->szPattern,
											m_pCurrentCase->szAutoGenFilter);

	// Begin the test case
    BeginTestCase(szTestName, uTestNum);

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
		//Create the texture if needed
		if (m_pCurrentCase->bCreateTexture || !m_pTextures)
		{
			//without this, the texture will only be released after the next SetTexture, causing memory problems
			SetTexture(0, NULL);

			//Create the texture
			if (! CreateBaseTexture() )
				goto Fail;
			
			SetTexture(0, m_pTextures);
		}

		if (m_pCurrentCase->bCreatePatternSurface)
		{
			if (! CreatePatternSurface() )
				goto Fail;
		}

		//Set the AutoGenFilter if needed
		if (m_pCurrentCase->bSetAutoGenFilter)
		{
			//render the miplevels
			if (! RenderMipLevels() )
				goto Fail;
		}

		SetSamplerState(0, SAMP_MIPFILTER, TEXF_LINEAR);
		SetSamplerState(0, SAMP_MINFILTER, m_pCurrentCase->dwAutoGenFilter);
		if ( m_bMagTexFilSupported[m_pCurrentCase->uAutoGenFilterIndex] && m_bMagTexFilSupportedRef[m_pCurrentCase->uAutoGenFilterIndex] )
			SetSamplerState(0, SAMP_MAGFILTER, m_pCurrentCase->dwAutoGenFilter);
	}

	RenderScene();

	GetLastError();

	return true;	

Fail:

	Fail();
	DumpTestDetails();
	return false;
}

bool CMipGenEmulation::TestTerminate(void)
{
	// Cleanup textures
	SetTexture(0, NULL);
    RELEASE(m_pTextures);
	ReleasePatternSurface();

	return CMipGenTest::TestTerminate();
}

void CMipGenEmulation::UpdateStatus(void)
{
	// Get the default status
    CD3DTest::UpdateStatus();

	// Tell status window what Filters we are using
	WriteStatus("$wFormat\t",m_pCurrentCase->szFormat);
	WriteStatus("$wAutoGen Filter",m_pCurrentCase->szAutoGenFilter);
	//WriteStatus("$wTexture Type",m_szTextureType);
	WriteStatus("$wPattern\t",m_pCurrentCase->szPattern);
	if (m_uCurrentFrame == 0)
		WriteStatus("$wMipLevel",_T("All"));
	else
		WriteStatus("$wMipLevel",_T("%d"), m_uCurrentMipLevel);
}

void CMipGenEmulation::GenerateCaseList()
{
	std::vector <TEXTURE_CASE *>::iterator caseIter;

	TEXTURE_CASE *pTexCase = NULL;

	bool bCreateTexture = false;
	bool bSetAutoGenFilter = false;
	bool bCreatePatternSurface = false;
	bool bUseOnlyPointFilter;

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

	for (int iFormat=0; iFormat<cFormat; iFormat++)
	{
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
												USAGE_RENDERTARGET | dwAdditionalUsageFlags,
												m_RTYPECurrent,
												g_formats[iFormat].format);

		//If the format is not available for this usage, don't test it
		if (FAILED(hr))
			continue;

		//If the format is not supported as a render target for the ref dev, don't test it
		hr = m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(),
										m_pRefDevice->GetDevType(),
										m_pCurrentMode->pDisplay->Format,
										USAGE_RENDERTARGET,
										m_RTYPECurrent,
										g_formats[iFormat].format);

		//If the format is not available for this usage, don't test it
		if (FAILED(hr))
			continue;

		//Is format supported for filter types other than TEXF_POINT
		hr = m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(),
												m_pSrcDevice->GetDevType(),
												m_pCurrentMode->pDisplay->Format,
												USAGE_QUERY_FILTER,
												m_RTYPECurrent,
												g_formats[iFormat].format);

		bUseOnlyPointFilter = FAILED(hr);

		bCreateTexture = true;

		for (int iPattern=0; iPattern<cPattern; iPattern++)
		{
			bCreatePatternSurface = true;

			//Make sure we want to test this pattern
			if ( ! (g_patterns[iPattern].dwPattern & dwPatternToTest) )
				continue;

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

				//Create a new Texture case
				pTexCase = new TEXTURE_CASE;
				ZeroMemory(pTexCase, sizeof(pTexCase));

				pTexCase->format = g_formats[iFormat].format;
				pTexCase->uBPP = g_formats[iFormat].uBPP;
				_tcscpy(pTexCase->szFormat, g_formats[iFormat].szFormat);

				pTexCase->dwPattern = g_patterns[iPattern].dwPattern;
				_tcscpy(pTexCase->szPattern, g_patterns[iPattern].szPattern);

				pTexCase->dwAutoGenFilter = g_filters[iAutoGenFilter].dwFilter;
				pTexCase->uAutoGenFilterIndex = g_filters[iAutoGenFilter].uIndex;
				_tcscpy(pTexCase->szAutoGenFilter, g_filters[iAutoGenFilter].szFilter);

				pTexCase->bCreateTexture = bCreateTexture;
				pTexCase->bCreatePatternSurface = bCreatePatternSurface;
				pTexCase->bSetAutoGenFilter = bSetAutoGenFilter;

				bSetAutoGenFilter = false;
				bCreatePatternSurface = false;
				bCreateTexture = false;

				//Add the case to the list
				m_listCases.push_back(pTexCase);
			}
		}
	}
}

void CMipGenEmulation::DumpTestDetails()
{
	WriteToLog( "Format       : %s\n", m_pCurrentCase->szFormat );
	WriteToLog( "Gen Filter   : %s\n", m_pCurrentCase->szAutoGenFilter );
	WriteToLog( "Pattern      : %s\n", m_pCurrentCase->szPattern );
	if (m_uCurrentFrame == 0)
		WriteToLog( "MipLevel     : All\n" );
	else
		WriteToLog( "MipLevel     : %d\n", m_uCurrentMipLevel );
}


void CMipGenEmulation::CommandLineHelp()
{
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
