#include "GetDC.h"

// ----------------------------------------------------------------------------

bool CGetDC::CapsCheck()
{
	ValidatePixelFormat();
	ValidateTextureUsage();
	ValidatePools();
	ValidateSizes();
	ValidateTestCases();

	return true;
}

// ----------------------------------------------------------------------------

void CGetDC::CommandLineHelp(void)
{
	WriteCommandLineHelp("\n$gTEST CASES");
	WriteCommandLineHelp("\n$g\tFor all the following flag, if '!' is put before 'TEST', the test case will");
	WriteCommandLineHelp("\n$g\tbe excluded(eg. -!TEST_SETPIXEL).  Many test cases can be excluded.  Otherwise,");
	WriteCommandLineHelp("\n$g\tONLY the test case of the TEST parameter will be executed.\n");
	WriteCommandLineHelp("$y-TEST_SETPIXEL\t$wSetPixel");
	WriteCommandLineHelp("$y-TEST_BLTDXTOGDI\t$wBlt DX DC to GDI DC");
	WriteCommandLineHelp("$y-TEST_BLTGDITODX\t$wBlt GDI DC to DX DC");
	WriteCommandLineHelp("$y-TEST_BLTDXTODX\t$wBlt DX DC to DX DC");
	WriteCommandLineHelp("$y-TEST_RENDERTHENGETDC\t$wRender 3D then GetDC");
	WriteCommandLineHelp("$y-TEST_GETDCTHENRENDER\t$wGetDC then Render 3D");
	WriteCommandLineHelp("$y-TEST_CREATEDCONLOSTDEV\t$wCreate a DC on Lost Device");
	WriteCommandLineHelp("$y-TEST_USEDCONLOSTDEV\t$wUse a DC on Aliased VidMem");
	WriteCommandLineHelp("$y-TEST_USEDCAFTERBEGINSCENE\t$wUse a DC between BeginScene and EndScene");
}

void CGetDC::ValidateTestCases()
{
	DWORD dwCases = 0xffffffff;
	if ( KeySet(_T("TEST_SETPIXEL")) )
		dwCases = TEST_SETPIXEL;
	if ( KeySet(_T("!TEST_SETPIXEL")) )
		dwCases &= ~TEST_SETPIXEL;

	if ( KeySet(_T("TEST_BLTDXTOGDI")) )
		dwCases = TEST_BLTDXTOGDI;
	if ( KeySet(_T("!TEST_BLTDXTOGDI")) )
		dwCases &= ~TEST_BLTDXTOGDI;

	if ( KeySet(_T("TEST_BLTGDITODX")) )
		dwCases = TEST_BLTGDITODX;
	if ( KeySet(_T("!TEST_BLTGDITODX")) )
		dwCases &= ~TEST_BLTGDITODX;

	if ( KeySet(_T("TEST_BLTDXTODX")) )
		dwCases = TEST_BLTDXTODX;
	if ( KeySet(_T("!TEST_BLTDXTODX")) )
		dwCases &= ~TEST_BLTDXTODX;

	if ( KeySet(_T("TEST_RENDERTHENGETDC")) )
		dwCases = TEST_RENDERTHENGETDC;
	if ( KeySet(_T("!TEST_RENDERTHENGETDC")) )
		dwCases &= ~TEST_RENDERTHENGETDC;

	if ( KeySet(_T("TEST_GETDCTHENRENDER")) )
		dwCases = TEST_GETDCTHENRENDER;
	if ( KeySet(_T("!TEST_GETDCTHENRENDER")) )
		dwCases &= ~TEST_GETDCTHENRENDER;

	if ( KeySet(_T("TEST_CREATEDCONLOSTDEV")) )
		dwCases = TEST_CREATEDCONLOSTDEV;
	if ( KeySet(_T("!TEST_CREATEDCONLOSTDEV")) )
		dwCases &= ~TEST_CREATEDCONLOSTDEV;

	if ( KeySet(_T("TEST_USEDCONLOSTDEV")) )
		dwCases = TEST_USEDCONLOSTDEV;
	if ( KeySet(_T("!TEST_USEDCONLOSTDEV")) )
		dwCases &= ~TEST_USEDCONLOSTDEV;

	if ( KeySet(_T("TEST_USEDCAFTERBEGINSCENE")) )
		dwCases = TEST_USEDCAFTERBEGINSCENE;
	if ( KeySet(_T("!TEST_USEDCAFTERBEGINSCENE")) )
		dwCases &= ~TEST_USEDCAFTERBEGINSCENE;

#if FAST_TEST
    m_casesTestCase.AddCase( TEST_BLTDXTOGDI,			"Blt DX DC to GDI DC" );
    m_casesTestCase.AddCase( TEST_BLTGDITODX,			"Blt GDI DC to DX DC" );
    m_casesTestCase.AddCase( TEST_RENDERTHENGETDC,		"Render 3D then GetDC" );
    m_casesTestCase.AddCase( TEST_GETDCTHENRENDER,		"GetDC then Render 3D" );

#else//FAST_TEST
	if(!KeySet(_T("WHQL")))
	{
		//Following cases should not be added if not running API tests
		m_casesAPITestCase.AddCase( TEST_LOCKTHENGETDC,		"Lock surface then GetDC" );
		m_casesAPITestCase.AddCase( TEST_GETDCTHENLOCK,		"GetDC then Lock surface" );
		m_casesAPITestCase.AddCase( TEST_GETMULTIPLEDCS,		"Get multiple DCs from one surface" );
		m_casesAPITestCase.AddCase( TEST_RELEASEDDCUNUSABLE,	"Released DC unusable" );
		m_casesAPITestCase.AddCase( TEST_RELEASEDCTWICE,		"ReleaseDC twice" );
		m_casesAPITestCase.AddCase( TEST_RELEASEDCONOTHERSURF,	"ReleaseDC with DC from other surface" );

		m_casesOtherAPITestCase.AddCase( TEST_RELEASEBOGUSDC,	"ReleaseDC with a bogus DC" );
		m_casesOtherAPITestCase.AddCase( TEST_MEMORYLEAKS,		"Check for memory leaks" );
	    if(KeySet(_T("INV")))
		{
			m_casesOtherAPITestCase.AddCase( TEST_GETDCBADHANDLEPTR,"GetDC with a bad handle pointer" );
		}
	}

	if (dwCases & TEST_SETPIXEL)
		m_casesTestCase.AddCase( TEST_SETPIXEL,				"SetPixel" );
	if (dwCases & TEST_BLTDXTOGDI)
		m_casesTestCase.AddCase( TEST_BLTDXTOGDI,			"Blt DX DC to GDI DC" );
	if (dwCases & TEST_BLTGDITODX)
		m_casesTestCase.AddCase( TEST_BLTGDITODX,			"Blt GDI DC to DX DC" );
	if (dwCases & TEST_BLTDXTODX)
		m_casesTestCase.AddCase( TEST_BLTDXTODX,			"Blt DX DC to DX DC" );
	//m_casesTestCase.AddCase( TEST_BLTDXTOSCREEN,		"Blt DX DC to Screen DC" );
	//m_casesTestCase.AddCase( TEST_BLTSCREENTODX,		"Blt Screen DC to DX DC" );
	if (dwCases & TEST_RENDERTHENGETDC)
		m_casesTestCase.AddCase( TEST_RENDERTHENGETDC,		"Render 3D then GetDC" );
	if (dwCases & TEST_GETDCTHENRENDER)
		m_casesTestCase.AddCase( TEST_GETDCTHENRENDER,		"GetDC then Render 3D" );
	if (dwCases & TEST_CREATEDCONLOSTDEV)
		m_casesTestCase.AddCase( TEST_CREATEDCONLOSTDEV,	"Create a DC on Lost Device" );
	if (dwCases & TEST_USEDCONLOSTDEV)
		m_casesTestCase.AddCase( TEST_USEDCONLOSTDEV,		"Use a DC on Aliased VidMem" );
	if (dwCases & TEST_USEDCAFTERBEGINSCENE)
		m_casesTestCase.AddCase( TEST_USEDCAFTERBEGINSCENE,	"Use a DC between BeginScene and EndScene" );
	//m_casesTestCase.AddCase( TEST_GDIATTRIBUTES,		"Check GDI DC attributes" );
	//m_casesTestCase.AddCase( TEST_GDICLIPPING,			"GDI Clipping" );
	//m_casesTestCase.AddCase( TEST_HIGHLEVELGDIOPER,		"High-Level GDI operations on DC" );
#endif//FAST_TEST
}

// ----------------------------------------------------------------------------

// Used to Add only the pixel formats valid for this test
void CGetDC::ValidatePixelFormat()
{	
	FORMAT adapterFormat = m_pSrcDevice->GetPresParams()->BackBufferFormat;
	DWORD dwDevType = m_pSrcDevice->GetDevType();
	//TODO: Needs different cases list for each adapter
	for (int i=0; i<MAXFORMAT; i++)
	{
		FMT format = (FMT)m_FormatList[i].GetValue();

		if (SUCCEEDED( m_pD3D->CheckDeviceFormat(	m_pSrcDevice->GetAdapterID(), 
								dwDevType, 
								adapterFormat,
								0,
								RTYPE_TEXTURE,
								format ) ) )
		{
			m_casesPixelFormatTexture.AddCase(&m_FormatList[i]);
		}

		//RenderTarget pixel formats
		if (SUCCEEDED( m_pD3D->CheckDeviceFormat(	m_pSrcDevice->GetAdapterID(), 
								dwDevType, 
								adapterFormat,
								D3DUSAGE_RENDERTARGET,
								RTYPE_SURFACE,
								format ) ) )
		{
			m_casesPixelFormatRT.AddCase(&m_FormatList[i]);
		}

		//OffscreenPlain Surface pixel formats
		if (SUCCEEDED( m_pD3D->CheckDeviceFormat(	m_pSrcDevice->GetAdapterID(), 
								dwDevType, 
								adapterFormat,
								0,
								RTYPE_SURFACE,
								format ) ) )
		{
			m_casesPixelFormatOffscreenPlain.AddCase(&m_FormatList[i]);
		}

		//Buffer Surfaces pixel formats
		if (SUCCEEDED( m_pD3D->CheckDeviceType(	m_pSrcDevice->GetAdapterID(), 
								dwDevType, 
								adapterFormat,
								format,
								true ) ) )
		{
			m_casesPixelFormatSwapChain.AddCase(&m_FormatList[i]);
		}
	}
}

// ----------------------------------------------------------------------------

// Used to Add only the pools valid for this test
void CGetDC::ValidatePools()
{
#if FAST_TEST
    m_casesPool.AddCase(POOL_DEFAULT,		"DEFAULT");
    m_casesPool.AddCase(POOL_SYSTEMMEM,		"SYSTEMMEM");
    m_casesPoolOffscreenPlain.AddCase(POOL_DEFAULT,		"DEFAULT");
    m_casesPoolOffscreenPlain.AddCase(POOL_SYSTEMMEM,	"SYSTEMMEM");
#else//FAST_TEST
	m_casesPool.AddCase(POOL_DEFAULT,		"DEFAULT");
	m_casesPool.AddCase(POOL_SYSTEMMEM,		"SYSTEMMEM");
	m_casesPool.AddCase(POOL_MANAGED,		"MANAGED");

	m_casesPoolOffscreenPlain.AddCase(POOL_DEFAULT,		"DEFAULT");
	m_casesPoolOffscreenPlain.AddCase(POOL_SYSTEMMEM,	"SYSTEMMEM");
	//TODO: add Managed if driver supports it
	//m_casesPoolOffscreenPlain.AddCase(D3DPOOL_MANAGED,	"MANAGED"); 
#endif//FAST_TEST
}

// ----------------------------------------------------------------------------

// Used to Add only the texture usage valid for this test
void CGetDC::ValidateTextureUsage()
{
	//Check if Dynamic textures are supported
	CAPS    	DevCaps;
	if ( FAILED(m_pSrcDevice->GetDeviceCaps(&DevCaps)) )
	{
	}
	m_bDynamicSupported = 0 != (DevCaps.Caps2 & D3DCAPS2_DYNAMICTEXTURES);
	m_bMipMapSupported = 0 != (DevCaps.TextureCaps & D3DPTEXTURECAPS_MIPMAP);

	m_casesTexUsage.AddCase(0,	"NONE");
}

// ----------------------------------------------------------------------------

bool IsPow2(UINT uSize)
{
	//Check if the size is a power of 2
	if (uSize < 2)
		return false;

	UINT uTemp = uSize;
	UINT uCount = 0;
	for (int i=0; i<sizeof(UINT)*8; i++)
	{
		if ( (uTemp >> i) & 0x00000001 )
			uCount ++;
	}

	return (uCount == 1);
}

// ----------------------------------------------------------------------------

// Used to Add only the valid sizes for this test
void CGetDC::ValidateSizes()
{
	//Check if textures requires a size of power of 2
    CAPS    	DevCaps;
	if ( FAILED(m_pSrcDevice->GetDeviceCaps(&DevCaps)) )
	{
	}
	bool bPow2Only = 0 != (DevCaps.TextureCaps & D3DPTEXTURECAPS_POW2);

	for (int i=0; i<MAXSIZES; i++)
	{
		CSizeCase* pSizeCase = &m_SizeList[ i ];
		//Add the size to all surfaces
		m_casesSize.AddCase(pSizeCase);

		//For textures, make sure the size is pow 2 if it's required by driver
		if ( bPow2Only && ( !IsPow2(pSizeCase->GetWidth()) || !IsPow2(pSizeCase->GetHeight()) ) )
			continue;

		m_casesSizeTexture.AddCase(pSizeCase);
	}
}

