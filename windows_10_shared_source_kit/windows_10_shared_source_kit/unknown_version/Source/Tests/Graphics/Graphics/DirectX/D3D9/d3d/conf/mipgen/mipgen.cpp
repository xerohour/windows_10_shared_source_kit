#include <d3d9.h>
#include <d3dx9.h>
#include <CBuffers.h>
#include "MipGen.h"

// Global Test and App instance
CD3DWindowFramework		App;

CMipGenEmuTexture		EmuTex;
CMipGenEmuCubeTexture	EmuCubeTex;

CMipGenHWTexture		HWTex;
CMipGenHWCubeTexture	HWCubeTex;

#if FAST_TEST
TEXTURE_FORMAT g_formats[] =
{
	{ 8.0f, 9.0f,	4,	FMT_A8R8G8B8             	,	_T("FMT_A8R8G8B8")		 },
	{ 8.0f, 9.0f,	1,	FMT_A8                   	,	_T("FMT_A8")			 },
	{ 8.1f, 9.0f,	4,	FMT_G16R16               	,	_T("FMT_G16R16")		 },
	{ 9.0f, 9.0f,	8,	FMT_A16B16G16R16			,	_T("FMT_A16B16G16R16")	 },
	{ 8.0f, 9.0f,	1,	FMT_L8                   	,	_T("FMT_L8")			 },
	{ 9.0f, 9.0f,	2,	FMT_R16F                 	,	_T("FMT_R16F")			 },
};
const UINT cFormat = sizeof(g_formats) / sizeof(g_formats[0]);

TEXTURE_FILTER g_filters[] =
{
	{ 8.0f, 9.0f, 0, TEXF_POINT,			_T("Point") },
	{ 8.0f, 9.0f, 2, TEXF_ANISOTROPIC,		_T("Anisotropic") },
};	  
const UINT cFilter = sizeof(g_filters) / sizeof(g_filters[0]);

TEXTURE_PATTERN g_patterns[] =
{
	{ PATTERN_1X1,			_T("1x1 Checker") },
	{ PATTERN_17X17,		_T("17x17 Checker") },
};	  
const UINT cPattern = sizeof(g_patterns) / sizeof(g_patterns[0]);


#else //FAST_TEST

TEXTURE_FORMAT g_formats[] =
{
	{ 8.0f, 9.0f,	3,	FMT_R8G8B8               	,	_T("FMT_R8G8B8")		 },
	{ 8.0f, 9.0f,	4,	FMT_A8R8G8B8             	,	_T("FMT_A8R8G8B8")		 },
	{ 8.0f, 9.0f,	4,	FMT_X8R8G8B8             	,	_T("FMT_X8R8G8B8")		 },
	{ 8.0f, 9.0f,	2,	FMT_R5G6B5               	,	_T("FMT_R5G6B5")		 },
	{ 8.0f, 9.0f,	2,	FMT_X1R5G5B5             	,	_T("FMT_X1R5G5B5")		 },
	{ 8.0f, 9.0f,	2,	FMT_A1R5G5B5             	,	_T("FMT_A1R5G5B5")		 },
	{ 8.0f, 9.0f,	2,	FMT_A4R4G4B4             	,	_T("FMT_A4R4G4B4")		 },
	{ 8.0f, 9.0f,	1,	FMT_R3G3B2               	,	_T("FMT_R3G3B2")		 },
	{ 8.0f, 9.0f,	1,	FMT_A8                   	,	_T("FMT_A8")			 },
	{ 8.0f, 9.0f,	2,	FMT_A8R3G3B2             	,	_T("FMT_A8R3G3B2")		 },
	{ 8.0f, 9.0f,	2,	FMT_X4R4G4B4             	,	_T("FMT_X4R4G4B4")		 },
	{ 8.1f, 9.0f,	4,	FMT_A2B10G10R10          	,	_T("FMT_A2B10G10R10")	 },
	{ 9.0f, 9.0f,	4,	FMT_A8B8G8R8             	,	_T("FMT_A8B8G8R8")		 },
	{ 9.0f, 9.0f,	4,	FMT_X8B8G8R8             	,	_T("FMT_X8B8G8R8")		 },
	{ 8.1f, 9.0f,	4,	FMT_G16R16               	,	_T("FMT_G16R16")		 },
	{ 9.0f, 9.0f,	4,	FMT_A2R10G10B10          	,	_T("FMT_A2R10G10B10")	 },
	{ 9.0f, 9.0f,	8,	FMT_A16B16G16R16			,	_T("FMT_A16B16G16R16")	 },
	  
	{ 8.0f, 9.0f,	2,	FMT_A8P8                 	,	_T("FMT_A8P8")			 },
	{ 8.0f, 9.0f,	1,	FMT_P8                   	,	_T("FMT_P8")			 },
	  
	//8.0f, 9.0f,	{ FMT_P4					,	_T("//FMT_P4")			 },
	  
	{ 8.0f, 9.0f,	1,	FMT_L8                   	,	_T("FMT_L8")			 },
	{ 8.0f, 9.0f,	2,	FMT_A8L8                 	,	_T("FMT_A8L8")			 },
	{ 8.0f, 9.0f,	1,	FMT_A4L4                 	,	_T("FMT_A4L4")			 },
	{ 9.0f, 9.0f,	2,	FMT_L16                  	,	_T("FMT_L16")			 },
	  
	{ 8.0f, 9.0f,	2,	FMT_V8U8                 	,	_T("FMT_V8U8")			 },
	{ 8.0f, 9.0f,	2,	FMT_L6V5U5               	,	_T("FMT_L6V5U5")		 },
	{ 8.0f, 9.0f,	4,	FMT_X8L8V8U8             	,	_T("FMT_X8L8V8U8")		 },
	{ 8.0f, 9.0f,	4,	FMT_Q8W8V8U8             	,	_T("FMT_Q8W8V8U8")		 },
	{ 8.0f, 9.0f,	4,	FMT_V16U16               	,	_T("FMT_V16U16")		 },
	{ 8.0f, 8.1f,	4,	FMT_W11V11U10            	,	_T("FMT_W11V11U10")	 },
	//8.0f, 9.0f,	{ FMT_W10V11U11          	,	_T("//FMT_W10V11U11")	 },
	{ 8.1f, 9.0f,	4,	FMT_A2W10V10U10          	,	_T("FMT_A2W10V10U10")	 },
	  
	//8.0f, 9.0f,	{ FMT_S1D15              	,	_T("//FMT_S1D15")		 },
	//8.0f, 9.0f,	{ FMT_S8D24              	,	_T("//FMT_S8D24")		 },
	//8.0f, 9.0f,	{ FMT_X8D24              	,	_T("//FMT_X8D24")		 },
	//8.0f, 9.0f,	{ FMT_X4S4D24            	,	_T("//FMT_X4S4D24")	 },
	  
	  
	{ 9.0f, 9.0f,	8,	FMT_Q16W16V16U16         	,	_T("FMT_Q16W16V16U16")	 },
	  
	{ 8.0f, 9.0f,	2,	FMT_UYVY                 	,	_T("FMT_UYVY")			 },
	{ 8.0f, 9.0f,	2,	FMT_YUY2                 	,	_T("FMT_YUY2")			 },
	//Do not test DXTn
	//{ 8.0f, 9.0f,	FMT_DXT1                 	,	_T("FMT_DXT1")			 },
	//{ 8.0f, 9.0f,	FMT_DXT2                 	,	_T("FMT_DXT2")			 },
	//{ 8.0f, 9.0f,	FMT_DXT3                 	,	_T("FMT_DXT3")			 },
	//{ 8.0f, 9.0f,	FMT_DXT4                 	,	_T("FMT_DXT4")			 },
	//{ 8.0f, 9.0f,	FMT_DXT5                 	,	_T("FMT_DXT5")			 },

	//DepthStencil not suppported for AUTOGENMIPMAP
	//{ 8.0f, 9.0f,	2,	FMT_D16_LOCKABLE         	,	_T("FMT_D16_LOCKABLE")	 },
	//{ 8.0f, 9.0f,	4,	FMT_D32                  	,	_T("FMT_D32")			 },
	//{ 8.0f, 9.0f,	2,	FMT_D15S1                	,	_T("FMT_D15S1")		 },
	//{ 8.0f, 9.0f,	3,	FMT_D24S8                	,	_T("FMT_D24S8")		 },
	//{ 8.0f, 9.0f,	3,	FMT_D24X8                	,	_T("FMT_D24X8")		 },
	//{ 8.0f, 9.0f,	3,	FMT_D24X4S4              	,	_T("FMT_D24X4S4")		 },
	//{ 8.0f, 9.0f,	2,	FMT_D16                  	,	_T("FMT_D16")			 },
	  

	//Don't test these
	//{ 8.0f, 9.0f,	FMT_VERTEXDATA           	,	_T("FMT_VERTEXDATA")	 },
	//{ 8.0f, 9.0f,	FMT_INDEX16              	,	_T("FMT_INDEX16")		 },
	//{ 8.0f, 9.0f,	FMT_INDEX32              	,	_T("FMT_INDEX32")		 },
	  
	//Not supported for AutoGenMipMap
	//{ 9.0f, 9.0f,	FMT_MULTI2_ARGB8         	,	_T("FMT_MULTI2_ARGB8")	 },
	  
	{ 9.0f, 9.0f,	2,	FMT_R16F                 	,	_T("FMT_R16F")			 },
	{ 9.0f, 9.0f,	4,	FMT_G16R16F              	,	_T("FMT_G16R16F")		 },
	{ 9.0f, 9.0f,	8,	FMT_A16B16G16R16F        	,	_T("FMT_A16B16G16R16F") },
	  
	{ 9.0f, 9.0f,	4,	FMT_R32F                 	,	_T("FMT_R32F")			 },
	{ 9.0f, 9.0f,	8,	FMT_G32R32F              	,	_T("FMT_G32R32F")		 },
	{ 9.0f, 9.0f,	16,	FMT_A32B32G32R32F        	,	_T("FMT_A32B32G32R32F") },
	  
	{ 9.0f, 9.0f,	2,	FMT_CxV8U8               	,	_T("FMT_CxV8U8")		 },
	  
	//{ 8.0f, 9.0f,	FMT_UNKNOWN              	,	_T("FMT_UNKNOWN")		 },
	//{ 8.0f, 9.0f,	FMT_FORCE_DWORD				,	_T("FMT_FORCE_DWORD")	 }
};
const UINT cFormat = sizeof(g_formats) / sizeof(g_formats[0]);

TEXTURE_FILTER g_filters[] =
{
	{ 8.0f, 9.0f, 0, TEXF_POINT,			_T("Point") },
	{ 8.0f, 9.0f, 1, TEXF_LINEAR,			_T("Linear") },
	{ 8.0f, 9.0f, 2, TEXF_ANISOTROPIC,		_T("Anisotropic") },
	{ 9.0f, 9.0f, 3, TEXF_PYRAMIDALQUAD,	_T("PyramidalQuad") },
	{ 9.0f, 9.0f, 4, TEXF_GAUSSIANQUAD,	_T("GaussianQuad") },
};	  
const UINT cFilter = sizeof(g_filters) / sizeof(g_filters[0]);

TEXTURE_PATTERN g_patterns[] =
{
	{ PATTERN_1X1,			_T("1x1 Checker") },
	//{ PATTERN_16X16,		_T("16x16 Checker") },  //This is not a really good case.
	{ PATTERN_17X17,		_T("17x17 Checker") },
	//{ PATTERN_FUNKYCHECKER,	_T("Funky Checker") },
	//{ PATTERN_DIAGONALS,	_T("Diagonals") },
};	  
const UINT cPattern = sizeof(g_patterns) / sizeof(g_patterns[0]);

#endif //FAST_TEST

CMipGenTest::CMipGenTest()
{
}

CMipGenTest::~CMipGenTest()
{
}

bool CMipGenTest::CheckTextureCaps()
{
	if ( ( m_pSrcDevice->GetCaps()->dwTextureCaps & D3DPTEXTURECAPS_MIPMAP ) == 0 )
	{
		WriteToLog
		(
			_T( "D3DPTEXTURECAPS_MIPMAP not supported." )
		);

		return false;
	};


	for (int iFilter=0; iFilter<cFilter; iFilter++)
	{
		//Make sure the filter is available on this DX version
		if ( (m_fDXVersion < g_filters[iFilter].fMinDX) && (m_fDXVersion > g_filters[iFilter].fMaxDX) )
			continue;

		switch(g_filters[iFilter].dwFilter)
		{
			case TEXF_POINT :
				m_bTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT) );
				m_bTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT) );
				m_bMagTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT) );
				m_bMagTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT) );
				break;

			case TEXF_LINEAR :
				m_bTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR) );
				m_bTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR) );
				m_bMagTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR) );
				m_bMagTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR) );
				break;

			case TEXF_ANISOTROPIC :
				m_bTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC) );
				m_bTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC) );
				m_bMagTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC) );
				m_bMagTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC) );
				break;

			case TEXF_GAUSSIANQUAD :
				m_bTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MINFGAUSSIANQUAD) );
				m_bTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MINFGAUSSIANQUAD) );
				m_bMagTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFGAUSSIANQUAD) );
				m_bMagTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFGAUSSIANQUAD) );
				break;

			case TEXF_PYRAMIDALQUAD :
				m_bTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MINFPYRAMIDALQUAD) );
				m_bTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MINFPYRAMIDALQUAD) );
				m_bMagTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD) );
				m_bMagTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD) );
				break;
		}
	}

	return true;
}

bool CMipGenTest::CheckCubeTextureCaps()
{
	if ( ( m_pSrcDevice->GetCaps()->dwTextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP) == 0 )
	{
		WriteToLog
		(
			_T( "D3DPTEXTURECAPS_MIPCUBEMAP not supported." )
		);

		return false;
	};


	for (int iFilter=0; iFilter<cFilter; iFilter++)
	{
		//Make sure the filter is available on this DX version
		if ( (m_fDXVersion < g_filters[iFilter].fMinDX) && (m_fDXVersion > g_filters[iFilter].fMaxDX) )
			continue;

		switch(g_filters[iFilter].dwFilter)
		{
			case TEXF_POINT :
				m_bTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT) );
				m_bTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT) );
				m_bMagTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT) );
				m_bMagTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT) );
				break;

			case TEXF_LINEAR :
				m_bTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR) );
				m_bTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR) );
				m_bMagTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR) );
				m_bMagTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR) );
				break;

			case TEXF_ANISOTROPIC :
				m_bTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC) );
				m_bTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC) );
				m_bMagTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC) );
				m_bMagTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC) );
				break;

			case TEXF_GAUSSIANQUAD :
				m_bTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MINFGAUSSIANQUAD) );
				m_bTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MINFGAUSSIANQUAD) );
				m_bMagTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MAGFGAUSSIANQUAD) );
				m_bMagTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MAGFGAUSSIANQUAD) );
				break;

			case TEXF_PYRAMIDALQUAD :
				m_bTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MINFPYRAMIDALQUAD) );
				m_bTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MINFPYRAMIDALQUAD) );
				m_bMagTexFilSupported[iFilter] = (0 != (m_pSrcDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD) );
				m_bMagTexFilSupportedRef[iFilter] = (0 != (m_pRefDevice->GetCaps()->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD) );
				break;
		}
	}

	return true;
}

bool CMipGenTest::ParseCmdLine()
{
	DWORD dwRefDevType = m_Options.D3DOptions.dwRefDevType;

	if (!CD3DTest::ParseCmdLine())
		return false;

	////If -SRC:xxx is specified on commandline but not -REF:xxx, set the ref to the same dev type as the src
	//if ( m_pFramework->m_dwCmdDevTypes && (m_Options.D3DOptions.dwRefDevType == dwRefDevType) )
	//{
	//	m_Options.D3DOptions.dwRefDevType = m_pFramework->m_dwCmdDevTypes;
	//}

	return true;
}


UINT CMipGenTest::TestInitialize(void)
{
    UINT uResult = CD3DTest::TestInitialize();
	if(uResult != D3DTESTINIT_RUN)
		return uResult;

	bool bResult;
	HRESULT hr;

	m_fDXVersion = floor(m_pSrcDevice->GetInterface());

	GenerateCaseList();

	m_bGuardband = (m_pSrcDevice->GetCaps()->dvGuardBandLeft != 0) && (m_pSrcDevice->GetCaps()->dvGuardBandTop != 0);

	//Set the Test Range here
	if (m_listCases.size() == 0)
	{
		WriteToLog("TestInitialize() - No test cases found.");
		WriteToLog("This could be because no format are exported for D3DFORMAT_OP_AUTOGENMIPMAP by the driver.");
		return D3DTESTINIT_ABORT;
	}

#if FAST_TEST
    SetTestRange(1, m_listCases.size() * 2); //1 for all mips at same time, then 1 for each mip
#else //FAST_TEST
	SetTestRange(1, m_listCases.size() * (8 + 1)); //1 for all mips at same time, then 1 for each mip
#endif //FAST_TEST

	// Setup our default Renderstates
    SetRenderState(D3DRS_CLIPPING, (DWORD)TRUE);
    SetRenderState(D3DRS_LIGHTING, (DWORD)FALSE);
    SetRenderState(D3DRS_ZENABLE, (DWORD)FALSE);

	// Setup our default Sampler State
    SetSamplerState(0, SAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    SetSamplerState(0, SAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    SetSamplerState(0, SAMP_ADDRESSW, D3DTADDRESS_CLAMP);

	// Setup our default Texture Stage State
    SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	m_bShowTopLevel = KeySet("SHOWTOPLEVEL");

	return D3DTESTINIT_RUN;
}

bool CMipGenTest::TestTerminate()
{
	//Delete the cases
	std::vector <TEXTURE_CASE *>::iterator caseIter;
	for (caseIter = m_listCases.begin(); caseIter != m_listCases.end(); caseIter++)
	{
		TEXTURE_CASE* pTexCase = (TEXTURE_CASE*)*caseIter;
		delete pTexCase;
	}
	m_listCases.clear();

	return true;
}

bool CMipGenTest::WritePatternOnSurface(LPSURFACE pSurface, DWORD dwPattern, UINT uFace)
{
	UINT uSquareSize = 1;
	switch (dwPattern)
	{
		case PATTERN_1X1 :
			uSquareSize = 1;
			break;

		case PATTERN_16X16 :
			uSquareSize = 16;
			break;

		case PATTERN_17X17 :
			uSquareSize = 17;
			break;
	}
    LOCKED_RECT Rect;
    if ( FAILED( pSurface->LockRect(&Rect, NULL, 0) ) )
	{
		WriteToLog("WritePatternOnSurface() - LockRect() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		return false;
	}

	FColor color;
    for (unsigned i = 0; i < 256; ++i)
    {
        for (unsigned j = 0; j < 256; ++j)
        {
			if (dwPattern == PATTERN_COLORFILL)
			{
				color = D3DCOLOR_ARGB(0xff, 0, 0, 0xff);
			}
			else
			{
				bool bFirstColor = ( (j & uSquareSize) == ( (i & uSquareSize) == 0 ? 0 : uSquareSize ) );
				BYTE gs = (((j & uSquareSize) == ((i & uSquareSize) == 0 ? 0 : uSquareSize)) ? 0 : 0xFF);
				color = D3DCOLOR_ARGB(0xff, gs, gs, gs);

				switch(uFace)
				{
					case 0: //white and black
						color = bFirstColor ? D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff) : D3DCOLOR_ARGB(0xff, 0x0, 0x0, 0x0);
						break;

					case 1: //red and blue
						color = bFirstColor ? D3DCOLOR_ARGB(0xff, 0xff, 0x0, 0x0) : D3DCOLOR_ARGB(0xff, 0x0, 0x0, 0xff);
						break;

					case 2: //green and aqua
						color = bFirstColor ? D3DCOLOR_ARGB(0xff, 0x0, 0xff, 0x0) : D3DCOLOR_ARGB(0xff, 0x0, 0xff, 0xff);
						break;

					case 3: //magenta and white
						color = bFirstColor ? D3DCOLOR_ARGB(0xff, 0xff, 0x0, 0xff) : D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff);
						break;

					case 4: //black and yellow
						color = bFirstColor ? D3DCOLOR_ARGB(0xff, 0x0, 0x0, 0x0) : D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0x0);
						break;

					case 5: //blue and green
						color = bFirstColor ? D3DCOLOR_ARGB(0xff, 0x0, 0x0, 0xff) : D3DCOLOR_ARGB(0xff, 0x0, 0xff, 0x0);
						break;
				}
			}
			char* pChar = ((char*)Rect.pBits + (j * m_pCurrentCase->uBPP));
			color.ConvertTo(m_pCurrentCase->format, 0, pChar, false, ELEMENTINDEX_0);
        }
        Rect.pBits = (BYTE*)Rect.pBits + Rect.iPitch;
    }

    if ( FAILED( pSurface->UnlockRect() ) )
	{
		WriteToLog("WritePatternOnSurface() - UnlockRect() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		return false;
	}

	return true;
}

//Test rendering, not working

//typedef struct _SPRITEVERTEX 
//{
//	FLOAT     x;             
//	FLOAT     y;
//	FLOAT     z;
//	//DWORD	  color;
//	FLOAT     tu;         
//	FLOAT     tv;
//} SPRITEVERTEX;
//
//bool CMipGenTest::DrawTexture(float x, float y, float size, bool bViewportEqualsSize, DWORD dwDevice)
//{
//    SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
//    SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
//    SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
//    SetRenderState(D3DRS_WRAP0, 0);
//    SetRenderState(D3DRS_CLIPPING, TRUE);
//    SetRenderState(D3DRS_VERTEXBLEND, FALSE);
//    SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);
//    SetNPatchMode(0.0f);
//    SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
//    SetRenderState(D3DRS_ENABLEADAPTIVETESSELLATION, FALSE);
//    SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
//    SetRenderState(D3DRS_COLORWRITEENABLE, 
//        D3DCOLORWRITEENABLE_RED  | D3DCOLORWRITEENABLE_GREEN | 
//        D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
//
//    SetRenderState(D3DRS_FOGENABLE, FALSE);
//    SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
//    SetRenderState(D3DRS_SPECULARENABLE, FALSE);
//    SetRenderState(D3DRS_STENCILENABLE, FALSE);
//    SetRenderState(D3DRS_LIGHTING, FALSE);
//    SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
//
//    // Modulate color and alpha
//    SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
//    SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//    SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
//    SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
//    SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
//    SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
//    SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
//    SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
//    SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
//    SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
//
//    //SetRenderState(D3DRS_ALPHATESTENABLE, true);
//    //SetRenderState(D3DRS_ALPHAREF, 0);
//    //SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
//
//    //SetRenderState(D3DRS_ALPHABLENDENABLE, true);
//    //SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);            
//    //SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//    //SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
//    //SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
//
//
//	SPRITEVERTEX v[4];
//
//	float left = x;
//	float right = x + size;
//	float top = y;
//	float bottom = y + size;
//
//	float minUV = 0;
//	float maxUV = 1;
//
//    v[0].x=left;   v[0].y =top;		v[0].z=0;  v[0].tu=minUV; v[0].tv=minUV;  //v[0].color = 0xffffffff;
//    v[1].x=left;   v[1].y =bottom;	v[1].z=0;  v[1].tu=minUV; v[1].tv=maxUV;  //v[1].color = 0xffffffff; 
//    v[2].x=right;  v[2].y =top;     v[2].z=0;  v[2].tu=maxUV; v[2].tv=minUV;  //v[2].color = 0xffffffff;  
//    v[3].x=right;  v[3].y =bottom;  v[3].z=0;  v[3].tu=maxUV; v[3].tv=maxUV;  //v[3].color = 0xffffffff;  
//
//	WORD indices[6];
//    indices[0] = (WORD) 0;
//    indices[1] = (WORD) 1;
//    indices[2] = (WORD) 2;
//    indices[3] = (WORD) 2;
//    indices[4] = (WORD) 1;
//    indices[5] = (WORD) 3;
//
//	D3DXMATRIXA16 pTransform;
//    D3DXMATRIX mat;
//	D3DXMatrixIdentity(&pTransform);
//    D3DXMatrixIdentity(&mat);
//
//    SetTransform(D3DTS_WORLD, &mat, dwDevice);
//    SetTransform(D3DTS_VIEW, &mat, dwDevice);
//
//	float fWidth = bViewportEqualsSize ? size : (float) m_DeviceList[0]->GetPresParams()->uWidth;
//	float fHeight = bViewportEqualsSize ? size : (float) m_DeviceList[0]->GetPresParams()->uHeight;
//    float fWidthInv  = 1.0f / fWidth;
//    float fHeightInv = 1.0f / fHeight;
//
//    memset(mat, 0x00, sizeof(mat));
//
//    mat._11 = 2.0f * fWidthInv;
//    mat._41 = -2.0f * fWidthInv * (FLOAT) 0 - fWidthInv - 1.0f;
//    mat._22 = -2.0f * fHeightInv;
//    mat._42 = 2.0f * fHeightInv * (FLOAT)fHeightInv + 1.0f;
//    mat._33 = 1.0f;
//    mat._44 = 1.0f;
//
//    SetTransform(D3DTS_PROJECTION, &mat, dwDevice);
//
//	SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1, dwDevice); 
//    SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE, dwDevice);
//	SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0, dwDevice);
//    SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE, dwDevice);
//
//    if (! BeginScene())
//	{
//		return false;
//	}
//
//	if (! SetVertexShader( D3DFVF_XYZ | /*D3DFVF_DIFFUSE |*/ D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0), dwDevice ) )
//	{
//		DPF( 1, _T("DrawResource() SetVertexShader() failed\n"));
//		//bRes = false;
//		//goto Exit;
//	}
//
//	if (! DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 4, 2, indices, FMT_INDEX16, v, sizeof(SPRITEVERTEX), dwDevice) )
//	{
//		DPF( 1, _T("DrawResource() DrawIndexedPrimitiveUP() failed\n"));
//		//bRes = false;
//		//goto Exit;
//	}
//
//	if (! EndScene() )
//	{
//		DPF( 1, _T("DrawResource() EndScene() failed\n"));
//		//bRes = false;
//		//goto Exit;
//	}
//
//	return true;
//}

bool CMipGenTest::DrawTexture(float x, float y, UINT size, DWORD dwDevice)
{
    MYDISPLAYVERTEX v[4];

	float left = (float)(x - 0.5);
	float right = (float)(x + size - 0.5);
	float top = (float)(y - 0.5);
	float bottom = (float)(y + size - 0.5);

	float minUV = 0;//0.5f / size;
	float maxUV = 1;//(size + 0.5f) / size;

    v[0].x=left;   v[0].y =bottom;  v[0].z=0;  v[0].rhw=1.0; v[0].tu=minUV; v[0].tv=maxUV; v[0].tw=0.0;  
    v[1].x=left;   v[1].y =top;     v[1].z=0;  v[1].rhw=1.0; v[1].tu=minUV; v[1].tv=minUV; v[1].tw=0.0;  
    v[2].x=right;  v[2].y =top;     v[2].z=0;  v[2].rhw=1.0; v[2].tu=maxUV; v[2].tv=minUV; v[2].tw=0.0;  
    v[3].x=right;  v[3].y =bottom;  v[3].z=0;  v[3].rhw=1.0; v[3].tu=maxUV; v[3].tv=maxUV; v[3].tw=0.0;  

	SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1, dwDevice);
    SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE, dwDevice);

    if (! BeginScene())
	{
		return false;
	}

	if (! SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0), dwDevice ) )
	{
		DPF( 1, _T("DrawResource() SetVertexShader() failed\n"));
		//bRes = false;
		//goto Exit;
	}

	if (! DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, v, sizeof(MYDISPLAYVERTEX), dwDevice) )
	{
		DPF( 1, _T("DrawResource() DrawPrimitiveUP() failed\n"));
		//bRes = false;
		//goto Exit;
	}

	if (! EndScene() )
	{
		DPF( 1, _T("DrawResource() EndScene() failed\n"));
		//bRes = false;
		//goto Exit;
	}

	return true;
}

bool CMipGenTest::DrawCubeTextureFace(float x, float y, UINT size, int face, DWORD dwDevice)
{
    MYDISPLAYVERTEX v[4];

	//we need to offset the pixel by -0.5 to align it.
	float offset = -0.5f;

	//If the hardware doesn't support guardband, then we can't have negative coord.
	//So we remove the offset.  We lose precision, but the result will be valid.
	//if (!m_bGuardband)
	//	offset = 0.0f;


	float left = x + offset;
	float right = x + size + offset;
	float top = y + offset;
	float bottom = y + size + offset;

	switch (face)
	{
		case 0 : //Positive X
			v[0].x=left;   v[0].y =bottom;  v[0].z=0;  v[0].rhw=1.0; v[0].tu=1.0; v[0].tv=-1.0; v[0].tw=1.0;  
			v[1].x=left;   v[1].y =top;     v[1].z=0;  v[1].rhw=1.0; v[1].tu=1.0; v[1].tv=1.0; v[1].tw=1.0;  
			v[2].x=right;  v[2].y =top;     v[2].z=0;  v[2].rhw=1.0; v[2].tu=1.0; v[2].tv=1.0; v[2].tw=-1.0;  
			v[3].x=right;  v[3].y =bottom;  v[3].z=0;  v[3].rhw=1.0; v[3].tu=1.0; v[3].tv=-1.0; v[3].tw=-1.0;  
			break;

		case 1 : //Negative X
			v[0].x=left;   v[0].y =bottom;  v[0].z=0;  v[0].rhw=1.0; v[0].tu=-1.0; v[0].tv=-1.0; v[0].tw=-1.0;  
			v[1].x=left;   v[1].y =top;     v[1].z=0;  v[1].rhw=1.0; v[1].tu=-1.0; v[1].tv=1.0; v[1].tw=-1.0;  
			v[2].x=right;  v[2].y =top;     v[2].z=0;  v[2].rhw=1.0; v[2].tu=-1.0; v[2].tv=1.0; v[2].tw=1.0;  
			v[3].x=right;  v[3].y =bottom;  v[3].z=0;  v[3].rhw=1.0; v[3].tu=-1.0; v[3].tv=-1.0; v[3].tw=1.0;  
			break;

		case 2 : //Positive Y
			v[0].x=left;   v[0].y =bottom;  v[0].z=0;  v[0].rhw=1.0; v[0].tu=-1.0; v[0].tv=1.0; v[0].tw=1.0;  
			v[1].x=left;   v[1].y =top;     v[1].z=0;  v[1].rhw=1.0; v[1].tu=-1.0; v[1].tv=1.0; v[1].tw=-1.0;  
			v[2].x=right;  v[2].y =top;     v[2].z=0;  v[2].rhw=1.0; v[2].tu=1.0; v[2].tv=1.0; v[2].tw=-1.0;  
			v[3].x=right;  v[3].y =bottom;  v[3].z=0;  v[3].rhw=1.0; v[3].tu=1.0; v[3].tv=1.0; v[3].tw=1.0;  
			break;

		case 3 : //Negative Y
			v[0].x=left;   v[0].y =bottom;  v[0].z=0;  v[0].rhw=1.0; v[0].tu=-1.0; v[0].tv=-1.0; v[0].tw=-1.0;  
			v[1].x=left;   v[1].y =top;     v[1].z=0;  v[1].rhw=1.0; v[1].tu=-1.0; v[1].tv=-1.0; v[1].tw=1.0;  
			v[2].x=right;  v[2].y =top;     v[2].z=0;  v[2].rhw=1.0; v[2].tu=1.0; v[2].tv=-1.0; v[2].tw=1.0;  
			v[3].x=right;  v[3].y =bottom;  v[3].z=0;  v[3].rhw=1.0; v[3].tu=1.0; v[3].tv=-1.0; v[3].tw=-1.0;  
			break;

		case 4 : //Positive Z
			v[0].x=left;   v[0].y =bottom;  v[0].z=0;  v[0].rhw=1.0; v[0].tu=-1.0; v[0].tv=-1.0; v[0].tw=1.0;  
			v[1].x=left;   v[1].y =top;     v[1].z=0;  v[1].rhw=1.0; v[1].tu=-1.0; v[1].tv=1.0; v[1].tw=1.0;  
			v[2].x=right;  v[2].y =top;     v[2].z=0;  v[2].rhw=1.0; v[2].tu=1.0; v[2].tv=1.0; v[2].tw=1.0;  
			v[3].x=right;  v[3].y =bottom;  v[3].z=0;  v[3].rhw=1.0; v[3].tu=1.0; v[3].tv=-1.0; v[3].tw=1.0;  
			break;

		case 5 : //Negative Z
			v[0].x=left;   v[0].y =bottom;  v[0].z=0;  v[0].rhw=1.0; v[0].tu=1.0; v[0].tv=-1.0; v[0].tw=-1.0;  
			v[1].x=left;   v[1].y =top;     v[1].z=0;  v[1].rhw=1.0; v[1].tu=1.0; v[1].tv=1.0; v[1].tw=-1.0;  
			v[2].x=right;  v[2].y =top;     v[2].z=0;  v[2].rhw=1.0; v[2].tu=-1.0; v[2].tv=1.0; v[2].tw=-1.0;  
			v[3].x=right;  v[3].y =bottom;  v[3].z=0;  v[3].rhw=1.0; v[3].tu=-1.0; v[3].tv=-1.0; v[3].tw=-1.0;  
			break;
	}

	SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1, dwDevice);
    SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE, dwDevice);

    if (! BeginScene())
	{
		return false;
	}

	if (! SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0), dwDevice ) )
	{
		DPF( 1, _T("DrawCubeTextureFace() SetVertexShader() failed\n"));
		return false;
	}

	if (! DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, v, sizeof(MYDISPLAYVERTEX), dwDevice) )
	{
		DPF( 1, _T("DrawCubeTextureFace() DrawPrimitiveUP() failed\n"));
		return false;
	}

	if (! EndScene() )
	{
		DPF( 1, _T("DrawCubeTextureFace() EndScene() failed\n"));
		return false;
	}

	return true;
}

bool CMipGenTest::RenderTextureScene()
{
	if (m_uCurrentFrame == 0)
	{
		SetSamplerState(0, SAMP_MAXMIPLEVEL, 0);
		float xOffset = 0;
		float yOffset = 0;
	//	for (int i=0; i<14; i++)
		for (int i=0; i<8; i++)
		{
			UINT uSize = 1 << i;
			//UINT uSize = 1 << (i+1) / 2;
			if (i == 0)
			{
				uSize = 1;
			}
			//else if (! (i%2) )
			//{
			//	uSize = uSize + (uSize / 2);
			//}

			if (xOffset + uSize > 256)
			{
				xOffset = 0;
				yOffset = (float)uSize;
			}

			DrawTexture(xOffset, yOffset, uSize);

			xOffset += (float)uSize;
		}
		if (m_bShowTopLevel)
		{
			DrawTexture(xOffset, yOffset, 256);
		}
	}
	else
	{
		m_uCurrentMipLevel = 8 - (m_uCurrentFrame - 1);

		SetSamplerState(0, SAMP_MAXMIPLEVEL, m_uCurrentMipLevel);
		DrawTexture(0, 0, 128);
	}

	return true;
}

bool CMipGenTest::RenderCubeTextureScene()
{
	// Initialize some variables
	UINT xOffset = 0;
	UINT yOffset = 0;

	if (m_uCurrentFrame == 0)
	{
		SetSamplerState(0, SAMP_MAXMIPLEVEL, 0);
		for (int iFace=0; iFace<6; iFace++)
		{
		//	for (int i=0; i<14; i++)
			for (int i=0; i<8; i++)
			{
				UINT uSize = 1 << i;
				//UINT uSize = 1 << (i+1) / 2;
				if (i == 0)
				{
					uSize = 1;
				}
				//else if (! (i%2) )
				//{
				//	uSize = uSize + (uSize / 2);
				//}

				if (xOffset + uSize > 256)
				{
					xOffset = 0;
					yOffset = uSize;
				}

				DrawCubeTextureFace((float)xOffset, (float)yOffset, uSize, iFace);

				xOffset += uSize;
			}
			if (m_bShowTopLevel)
			{
				DrawCubeTextureFace((float)xOffset, (float)yOffset, 256, iFace);
			}
			xOffset = 0;
			yOffset += 48;
		}
	}
	else
	{
		m_uCurrentMipLevel = 8 - (m_uCurrentFrame - 1);

		SetSamplerState(0, SAMP_MAXMIPLEVEL, m_uCurrentMipLevel);


		//Since REF uses multiple faces for linear, always use point
		SetSamplerState(0, SAMP_MINFILTER, TEXF_POINT);
		if (m_bMagTexFilSupported[0])
		{
			SetSamplerState(0, SAMP_MAGFILTER, TEXF_POINT);
		}

		for (int iFace=0; iFace<6; iFace++)
		{
			if (iFace == 3)
			{
				xOffset = 0;
				yOffset = 128;
			}

			DrawCubeTextureFace((float)xOffset, (float)yOffset, 128, iFace);

			xOffset += 100;
		}
	}

	return true;
}

//This is overloaded so that we print the test details on a failure.
bool CMipGenTest::ProcessFrame()
{
	bool bRes = CD3DTest::ProcessFrame();

	//If they pass the test, but not higher than 99.9%, 
	//render each mip to make sure the small levels are fine.
	m_bRenderEachMip = (m_fPassPercentage > 0.85f && m_fPassPercentage < 0.999f);

	if ( FAIL == m_pFramework->m_DXGLog.GetCurrentResult() )
	{
		DumpTestDetails();
	}

	return bRes;
}
