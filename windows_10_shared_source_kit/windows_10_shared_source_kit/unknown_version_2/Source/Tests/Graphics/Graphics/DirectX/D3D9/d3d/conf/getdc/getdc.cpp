// ============================================================================
//
//  GetDC.cpp
//
// ============================================================================
#include "GetDC.h"

//
// Declare the framework
//

CD3DWindowFramework	theApp;
//
// Define the groups here
//
CGetDC	tstGetDC;
#if FAST_TEST
CCase CGetDC::m_FormatList[MAXFORMAT] =
{
    //CCase( (DWORD)FMT_R8G8B8,          		_T("FMT_R8G8B8") ),          		
    CCase( (DWORD)FMT_X8R8G8B8,        		_T("FMT_X8R8G8B8") ),        		
    //CCase( (DWORD)FMT_R5G6B5,          		_T("FMT_R5G6B5") ),          		
    //CCase( (DWORD)FMT_X1R5G5B5,        		_T("FMT_X1R5G5B5") ),        		
};	  

CSizeCase CGetDC::m_SizeList[MAXSIZES] =
{
    //CSizeCase( SIZE_1X1			,"1X1",		1, 1),
    //CSizeCase( SIZE_2X2			,"2X2",		2, 2),
    //CSizeCase( SIZE_55X55		,"55X55",	55, 55),
    //CSizeCase( SIZE_63X64		,"63X64",	63, 64),
    CSizeCase(SIZE_64X64, "64X64", 64, 64),
    //CSizeCase( SIZE_257X64		,"257X64",	257, 64),
    //	CSizeCase( SIZE_1256X1256	,"1256X1256", 1256, 1256)
};
#else //FAST_TEST
CCase CGetDC::m_FormatList[MAXFORMAT] =
{
	CCase( (DWORD)FMT_R8G8B8,          		_T("FMT_R8G8B8") ),          		
	CCase( (DWORD)FMT_X8R8G8B8,        		_T("FMT_X8R8G8B8") ),        		
	CCase( (DWORD)FMT_R5G6B5,          		_T("FMT_R5G6B5") ),          		
	CCase( (DWORD)FMT_X1R5G5B5,        		_T("FMT_X1R5G5B5") ),        		
};	  

CSizeCase CGetDC::m_SizeList[MAXSIZES] = 
{
	CSizeCase( SIZE_1X1			,"1X1",		1, 1),
	CSizeCase( SIZE_2X2			,"2X2",		2, 2),
	CSizeCase( SIZE_55X55		,"55X55",	55, 55),
	CSizeCase( SIZE_63X64		,"63X64",	63, 64),
	CSizeCase( SIZE_64X64		,"64X64",	64, 64),
	CSizeCase( SIZE_257X64		,"257X64",	257, 64),
//	CSizeCase( SIZE_1256X1256	,"1256X1256", 1256, 1256)
};
#endif//FAST_TEST
	  
// ----------------------------------------------------------------------------

CGetDC::CGetDC()
{ 
  	m_szCommandKey=	_T("GetDC");
	m_szLogName = _T("GetDC");
	m_szTestName=	_T("GetDC");

 	SetTestType(TESTTYPE_CONF);

	m_Options.D3DOptions.dwWinMode = WINMODE_ALL;
	m_Options.D3DOptions.bRefEqualsSrc = false;
    m_Options.TestOptions.bCompareSrcRef = false;
	m_Options.D3DOptions.bReference = false;

	//m_Options.D3DOptions
	//m_Options.D3DOptions.dwWinMode = WINMODE_ALL;

	//Use these flags so that no backbuffer or DepthStencil surface are in D3DPOOL_DEFAULT for the reset
	m_Options.D3DOptions.bCachedBackBuffer = false;
	m_Options.D3DOptions.bZBuffer = false;

    m_Options.D3DOptions.fMinDXVersion = 9.0f;
	m_pFramework->GetImageCmp()->SetPassRequired(0.99f);
	//Determine which tests to run from the command line params
	//SetTestsToRun();
}

// ----------------------------------------------------------------------------

CGetDC::~CGetDC()
{
}

// ----------------------------------------------------------------------------

// Sets the current surface type, pixel format and usage
void CGetDC::SetCurrentSurfaceType()
{
	if ( m_rPixelFormatRT->Active() )
	{
		m_dwSurfaceType = SURF_RENDERTARGET;
		_tcscpy( m_szSurfaceType, _T("RenderTarget"));
		m_dwSurfaceFormat = m_casesPixelFormatRT[ m_rPixelFormatRT->Value() ]->GetValue();
		_tcscpy( m_szSurfaceFormat, m_casesPixelFormatRT[ m_rPixelFormatRT->Value() ]->GetDesc() );
	}
	else if ( m_rPixelFormatTexture->Active() )
	{
		m_dwSurfaceType = SURF_TEXTURE;
		_tcscpy( m_szSurfaceType, _T("Texture"));

		m_dwSurfaceFormat = m_casesPixelFormatTexture[ m_rPixelFormatTexture->Value() ]->GetValue();
		_tcscpy( m_szSurfaceFormat, m_casesPixelFormatTexture[ m_rPixelFormatTexture->Value() ]->GetDesc());

		m_dwUsage = m_casesTexUsage[ m_rTexUsage->Value() ]->GetValue();
		_tcscpy( m_szUsage, m_casesTexUsage[ m_rTexUsage->Value() ]->GetDesc());
	}
	else if ( m_rPixelFormatOffscreenPlain->Active() )
	{
		m_dwSurfaceType = SURF_OFFSCREENPLAIN;
		_tcscpy( m_szSurfaceType, _T("OffscreenPlain"));

		m_dwSurfaceFormat = m_casesPixelFormatOffscreenPlain[ m_rPixelFormatOffscreenPlain->Value() ]->GetValue();
		_tcscpy( m_szSurfaceFormat, m_casesPixelFormatOffscreenPlain[ m_rPixelFormatOffscreenPlain->Value() ]->GetDesc());
	}
	else if ( m_rPixelFormatSwapChain->Active() )
	{
		m_dwSurfaceType = SURF_SWAPCHAIN;
		_tcscpy( m_szSurfaceType, _T("SwapChain"));

		m_dwSurfaceFormat = m_casesPixelFormatSwapChain[ m_rPixelFormatSwapChain->Value() ]->GetValue();
		_tcscpy( m_szSurfaceFormat, m_casesPixelFormatSwapChain[ m_rPixelFormatSwapChain->Value() ]->GetDesc());
	}
	else if ( m_rBackBuffer->Active() )
	{
		m_dwSurfaceType = SURF_BACKBUFFER;
		_tcscpy( m_szSurfaceType, _T("BackBuffer"));

		m_dwSurfaceFormat = m_DeviceList[0]->GetPresParams()->BackBufferFormat.d3dfFormat;
		FmtToString(FORMAT_TEXTURE, &m_DeviceList[0]->GetPresParams()->BackBufferFormat, m_szSurfaceFormat);
	}
}

// ----------------------------------------------------------------------------

// Sets the current test case pool, size and type of test
bool CGetDC::SetTestCase(UINT uTestNumber)
{
	CSizeCase* pSizeCase = NULL;
	m_nSurfNeededForTest = 1;
	if ( m_rOtherAPITestCase->Active() )
	{
		//We don't need to cycle every pixelformat, size and pool.
		//Set the pixel format
		m_dwSurfaceType = SURF_TEXTURE;
		_tcscpy( m_szSurfaceType, _T("Texture") );
		m_dwSurfaceFormat = m_casesPixelFormatTexture[ 0 ]->GetValue();
		_tcscpy( m_szSurfaceFormat, m_casesPixelFormatTexture[ 0 ]->GetDesc() );

		//Set the pool
		m_dwPool = POOL_MANAGED;
		_tcscpy( m_szPool, _T("Managed") );

		//Set the size
		pSizeCase = (CSizeCase*)m_casesSizeTexture[ 0 ];
		m_uSurfaceWidth = pSizeCase->GetWidth();
		m_uSurfaceHeight = pSizeCase->GetHeight();
		_tcscpy( m_szSize, pSizeCase->GetDesc() );

		//Set the test case
		m_dwTestCase = m_casesOtherAPITestCase[ m_rOtherAPITestCase->Value() ]->GetValue();
		_tcscpy( m_szTestCase, m_casesOtherAPITestCase[ m_rOtherAPITestCase->Value() ]->GetDesc() );

		return true;
	}

	//Set the pixel format, surface type and usage
	SetCurrentSurfaceType();

	//Set the pool
	m_dwPool = 0;
	_tcscpy( m_szPool, _T("Default") );
	if ( m_rPool->Active() )
	{
		m_dwPool = m_casesPool[ m_rPool->Value() ]->GetValue();
		_tcscpy( m_szPool, m_casesPool[ m_rPool->Value() ]->GetDesc() );
	}
	else if ( m_rPoolOffscreenPlain->Active() )
	{
		m_dwPool = m_casesPoolOffscreenPlain[ m_rPoolOffscreenPlain->Value() ]->GetValue();
		_tcscpy( m_szPool, m_casesPoolOffscreenPlain[ m_rPoolOffscreenPlain->Value() ]->GetDesc() );
	}

	//Set the size
	m_uSurfaceWidth = 0;
	m_uSurfaceHeight = 0;
	if ( m_rSize->Active() || m_rSizeTexture->Active() )
	{
		if (SURF_TEXTURE == m_dwSurfaceType)
		{
			pSizeCase = (CSizeCase*)m_casesSizeTexture[ m_rSizeTexture->Value() ];
		}
		else
		{
			pSizeCase = (CSizeCase*)m_casesSize[ m_rSize->Value() ];
		}
		m_uSurfaceWidth = pSizeCase->GetWidth();
		m_uSurfaceHeight = pSizeCase->GetHeight();

		_tcscpy( m_szSize, pSizeCase->GetDesc() );
	}

	if ( m_rPixelFormatSwapChain->Active() || m_rBackBuffer->Active() )
	{
		m_uSurfaceWidth = m_DeviceList[0]->GetPresParams()->uWidth;
		m_uSurfaceHeight = m_DeviceList[0]->GetPresParams()->uHeight;

		_tcscpy( m_szSize, _T("ScreenSize") );
	}

	//Set the test type
	if ( m_rTestCase->Active() )
	{
		m_dwTestCase = m_casesTestCase[ m_rTestCase->Value() ]->GetValue();
		_tcscpy( m_szTestCase, m_casesTestCase[ m_rTestCase->Value() ]->GetDesc() );

		if (m_dwTestCase == TEST_BLTDXTODX && m_dwSurfaceType != SURF_BACKBUFFER)
		{
			m_nSurfNeededForTest = 2;
		}
	}
	else if ( m_rAPITestCase->Active() )
	{
		m_dwTestCase = m_casesAPITestCase[ m_rAPITestCase->Value() ]->GetValue();
		_tcscpy( m_szTestCase, m_casesAPITestCase[ m_rAPITestCase->Value() ]->GetDesc() );

		if (m_dwTestCase == TEST_RELEASEDCONOTHERSURF && m_dwSurfaceType != SURF_BACKBUFFER)
		{
			m_nSurfNeededForTest = 2;
		}
	}

	return true;
}

// ----------------------------------------------------------------------------

bool CGetDC::CreateTestFactors()
{
	//Do not run SwapChain tests on Fullscreen
	UINT uNbSwapChain = 0;
	if ( m_DeviceList[0]->GetPresParams()->bWindowed )
		uNbSwapChain = m_casesPixelFormatSwapChain.GetNbCases();

	if ( !CHECKREF(m_rPixelFormatRT = NewFactor( m_casesPixelFormatRT.GetNbCases() )) ||	
		 !CHECKREF(m_rPixelFormatTexture = NewFactor( m_casesPixelFormatTexture.GetNbCases() )) ||	
		 !CHECKREF(m_rPixelFormatOffscreenPlain = NewFactor( m_casesPixelFormatOffscreenPlain.GetNbCases() )) ||	
		 !CHECKREF(m_rPixelFormatSwapChain = NewFactor( uNbSwapChain )) ||	
		 !CHECKREF(m_rPool = NewFactor( m_casesPool.GetNbCases() )) ||	
		 !CHECKREF(m_rPoolOffscreenPlain = NewFactor( m_casesPoolOffscreenPlain.GetNbCases() )) ||	
		 !CHECKREF(m_rSize = NewFactor( m_casesSize.GetNbCases() )) ||	
		 !CHECKREF(m_rSizeTexture = NewFactor( m_casesSizeTexture.GetNbCases() )) ||	
		 !CHECKREF(m_rTexUsage = NewFactor( m_casesTexUsage.GetNbCases() )) ||	
		 !CHECKREF(m_rUseMipLevel = NewFactor( m_bMipMapSupported ? 2 : 1 )) ||	
		 !CHECKREF(m_rTestCase = NewFactor( m_casesTestCase.GetNbCases() )) ||	
		 !CHECKREF(m_rAPITestCase = NewFactor( m_casesAPITestCase.GetNbCases() )) ||	
		 !CHECKREF(m_rOtherAPITestCase = NewFactor( m_casesOtherAPITestCase.GetNbCases() )) ||	
		 !CHECKREF(m_rBackBuffer = NewFactor( 1 )))//m_casesPixelFormatBuffers.GetNbCases() )) ||	
	{
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------

UINT CGetDC::TestInitialize(void)
{     
	//Repro for manbug 64529
	//LPSURFACES pSurf = NULL;
	//CreateOffscreenPlainSurface(64, 64, FMT_X8R8G8B8, POOL_DEFAULT, &pSurf);
	//RELEASE(pSurf);

	//LPTEXTURE pTex = NULL;
	//LPTEXTURE pTexB = NULL;
	//LPSURFACE pSurf = NULL;
	//LPSURFACE pSurfB = NULL;
	//HDC hDC = NULL;
	//m_pSrcDevice->CreateTexture(64,64,0,0,FMT_X8R8G8B8, POOL_SYSTEMMEM, &pTex);
	//pTex->GetSurfaceLevel(0, &pSurf);
	//pTex->GetSurfaceLevel(1, &pSurfB);
	////m_pSrcDevice->GetBackBuffer(0, 0, &pSurf);

	////m_pSrcDevice->CreateTexture(64,64,0,USAGE_DYNAMIC,FMT_X8R8G8B8, POOL_DEFAULT, &pTexB);
	////pTexB->GetSurfaceLevel(1, &pSurfB);

	//LOCKED_RECT rect, rectb;
	//pSurf->LockRect(&rect, NULL, 0);
	//pSurfB->GetDC(&hDC);

	
	UINT uResult = CD3DTest::TestInitialize();

	if(uResult != D3DTESTINIT_RUN)
		return uResult;

	CreateTestFactors();

	if (!SetTestFactors())
	{
		SetTestRange(1,1);
		return D3DTESTINIT_SKIPALL;
	}

	if (!m_rCasesManager->Valid() || m_rCasesManager->Count() == 0)
	{
		SetTestRange(1,1);
		return D3DTESTINIT_SKIPALL;
	}

	//Set the Test Range here
	SetTestRange(1, m_rCasesManager->Count());

    return uResult;
}

// ----------------------------------------------------------------------------

bool CGetDC::ExecuteTest(UINT uTestNumber)
{
	m_rCasesManager->Set(uTestNumber - 1);

	m_bSkipTest = false;

	// Set the Present Parameters
/*	memcpy(&m_PresParams, m_DeviceList[0]->GetPresParams(), sizeof(PRESENT_PARAMETERS));
    m_PresParams.SwapEffect = D3DSWAPEFFECT_COPY;  // Must be copy to compare the back buffers 
	m_PresParams.dwFlags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
*/
	if ( !SetTestCase(uTestNumber) )
	{
		return false;
	}

	sprintf(m_szFullName, "G%d T%d %s", m_pFramework->GetCurrentTestNum(), uTestNumber, m_szTestCase);

	//DumpTestDetails();

	OutputDebugString(m_szFullName);
	OutputDebugString("\n");

	// Begin the test case
    BeginTestCase(m_szFullName, uTestNumber);

	//Hack to erase the menu
	//m_pSrcDevice->Present(NULL, NULL, NULL, NULL);

	//Create the surfaces
	if ( !CreateSurface() || !m_pSurfaces[0] || (m_nSurfNeededForTest == 2 && !m_pSurfaces[1]) )
	{
		//We don't want to abort if we only want to skip the test
		if (m_bSkipTest)
		{
			Skip();
			return true;
		}

		DumpTestDetails();
		Fail();
		return false;
	}

	HRESULT hr = NULL;
	bool result = true;

	//Get the surface(s)
	for (int i=0; i<m_nSurfNeededForTest; i++)
	{
		if( !m_pSurfaces[i]->GetSurface( &m_pSurf[i], 0 ) )
		{
			DPF( 1, "ExecuteTest() Could not GetSurface from the wrapper" );
			return false;
		}
	}

	// Perform the test
	if (false == RunTest())
	{
		DumpTestDetails();
		Fail();
	}

	//Release the surface(s)
 	for (int i=0; i<m_nSurfNeededForTest; i++)
	{
		RELEASE( m_pSurf[i] );
	}

	//No need to present, so return false;
    return false;
}

void CGetDC::EndTestCase(void)
{
	for (int iNbSurfNeeded=0; iNbSurfNeeded<m_nSurfNeededForTest; iNbSurfNeeded++)
	{
		RELEASE(m_pSurfaces[iNbSurfNeeded]);
		RELEASE(m_pTextures[iNbSurfNeeded]);
		RELEASE(m_pSwapChains[iNbSurfNeeded]);
	}
	CD3DTest::EndTestCase();
}

bool CGetDC::TestTerminate(void)
{
	m_rCasesManager.Release();
	m_casesSurface.Empty();
	m_casesPool.Empty();
	m_casesPoolOffscreenPlain.Empty();
	m_casesSize.Empty();
	m_casesSizeTexture.Empty();
	m_casesPixelFormatRT.Empty();
	m_casesPixelFormatTexture.Empty();
	m_casesPixelFormatOffscreenPlain.Empty();
	m_casesPixelFormatSwapChain.Empty();
	m_casesBackBuffer.Empty();
	m_casesTexUsage.Empty();
	m_casesTestCase.Empty();
	m_casesAPITestCase.Empty();
	m_casesOtherAPITestCase.Empty();

	return true;
}

bool CGetDC::SetTestFactors()
{
	RFactor rTexture = m_rPixelFormatTexture * m_rPool * m_rTexUsage * m_rSizeTexture * m_rUseMipLevel;
	RFactor rOffscreen = m_rPixelFormatOffscreenPlain * m_rPoolOffscreenPlain * m_rSize;
	RFactor rBuffers = m_rPixelFormatSwapChain + m_rBackBuffer;
	RFactor rRT = m_rPixelFormatRT * m_rSize;

	m_rCasesManager =	( ( rTexture + rOffscreen + rRT + rBuffers ) * 
							(m_rTestCase + m_rAPITestCase) ) + 
						m_rOtherAPITestCase;

	return 0 != m_rCasesManager->Valid();
}

void CGetDC::DumpTestDetails(void)
{
	DPF(1, "    SurfaceType :  %s\n", m_szSurfaceType);
	DPF(1, "	PixelFormat :  %s\n", m_szSurfaceFormat);
	DPF(1, "	Pool :         %s\n", m_szPool);
	DPF(1, "	Size :         %s\n", m_szSize);
	DPF(1, "	Usage :        %s\n", m_szUsage);
	if (m_rUseMipLevel->Active())
	{
		if (m_rUseMipLevel->Value() == 1)
		{
			DPF(1, "	Level :        Top\n");
		}
		else //if (m_rUseMipLevel->Value() == 2)
		{
			DPF(1, "	Level :        First Mip\n");
		}
	}
}