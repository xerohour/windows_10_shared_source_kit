#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "ResMan.h"

// Global Test and App instance
CD3DWindowFramework App;
CBasicTest          BasicTest;
CDiscardBytesTest   DiscardBytesTest;
CChangeColorsTest   ChangeColorsTest;
CLRUTest		LRUTexTest(IS_TEXTURE);
CLRUTest		LRUCubeTest(IS_CUBEMAP);
CLRUTest		LRUVolTest(IS_VOLUME);
CLRUTest		LRUMixedTest;
CMRUTest    	MRUTexTest(IS_TEXTURE);
CMRUTest		MRUCubeTest(IS_CUBEMAP);
CMRUTest		MRUVolTest(IS_VOLUME);
CMRUTest		MRUMixedTest;

//-----------------------------------------------------------------------------

ResMan::ResMan()
{
	m_input = NULL;
	m_output = NULL;

	if(KeySet(_T("stress")))
	{
		SetTestType(TESTTYPE_STRESS);
		m_szTestName   = "ResManStress";
        m_szLogName    = "ResManStress";
		m_szCommandKey = "ResManStress";
		m_Options.D3DOptions.fMinDXVersion = 7.f;
	}
	else if(KeySet(_T("D3DManage")))
	{
		SetTestType(TESTTYPE_FUNC);
		m_szTestName   = "ResManD3D";
        m_szLogName    = "ResManD3D";
		m_szCommandKey = "ResManD3D";
		m_Options.D3DOptions.fMinDXVersion = 7.f;
	}
	else
	{
		SetTestType(TESTTYPE_CONF);
		m_szTestName   = "ResMan";
		m_szCommandKey = "ResMan";
		m_szLogName    = "ResMan";
		m_Options.D3DOptions.fMinDXVersion = 7.f;

		// Force test to use relevant DirectX versions for WHQL
		if (KeySet("WHQL"))
			m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_7;
	}

	m_Options.ModeOptions.dwTexExclude  |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN;
	m_Options.ModeOptions.dwCubeExclude |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN;
	m_Options.ModeOptions.dwVolExclude  |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN;

	m_Options.D3DOptions.bReference = false;
}

//-----------------------------------------------------------------------------

bool ResMan::PreModeSetup(void)
{
	//we don't run the test with driver resource management any more since nobody supports it
	if (!KeySet(_T("DriverManage")))
	{
		DWORD dwBehavior;
		
		dwBehavior = m_pSrcDevice->GetBehaviors();

		//use new extended flag for dx9.c and higher DX versions
		if ( m_pD3D->GetRuntimeVersion() >= 9.029f )
		{
			if (KeySet("oldflag"))
				dwBehavior |= DEVICECREATE_DISABLE_DRIVER_MANAGEMENT;
			else
				dwBehavior |= DEVICECREATE_DISABLE_DRIVER_MANAGEMENT_EX;
		}
		else
			dwBehavior |= DEVICECREATE_DISABLE_DRIVER_MANAGEMENT;

		m_pSrcDevice->SetBehaviors(dwBehavior); 
	}

	//fix for bug 88180: skip all DXTN formats
	if ( m_dwVersion < 0x0800 && m_pFramework->m_DXGLog.GetSysInfo()->GetOSID() == 6 /*WinME*/ )
	{
		FORMAT *pTmp = NULL;
		UINT count = m_uCommonTextureFormats;

		for(UINT i = 0; i < m_uCommonTextureFormats; i++)
		{
			switch (m_pCommonTextureFormats[ i ].ddpfPixelFormat.dwFourCC)
			{
				case FMT_DXT1: case FMT_DXT2: case FMT_DXT3: case FMT_DXT4: case FMT_DXT5: 
					count--;
			}
		}
		if (count != m_uCommonTextureFormats)
		{
			UINT j = 0;
			pTmp = new FORMAT[ count ];

			for(UINT i = 0; i < count; i++)
			{
				bool isDXT = true;
				while ( isDXT && j < m_uCommonTextureFormats )
				{
					switch (m_pCommonTextureFormats[ j ].ddpfPixelFormat.dwFourCC)
					{
						case FMT_DXT1: case FMT_DXT2: case FMT_DXT3: case FMT_DXT4: case FMT_DXT5: 
							j++;
							isDXT = true;
							break;
						default:
							isDXT = false;
							break;
					}
				}

				pTmp[ i ].ddpfPixelFormat = m_pCommonTextureFormats[ j ].ddpfPixelFormat;
				pTmp[ i ].d3dfFormat      = m_pCommonTextureFormats[ j ].d3dfFormat;
				j++;
			}

			for(UINT i = 0; i < count; i++)
			{
				if ( i < count - 1 )
					pTmp[ i ].pNext = &pTmp[ i + 1 ];
				else
					pTmp[ i ].pNext = NULL;
			}

			SAFEDELETEA(m_pCommonTextureFormats);
		    m_pCommonTextureFormats = pTmp;
			m_uCommonTextureFormats = count;
		}
	}

	return CD3DTest::PreModeSetup();
}

//-----------------------------------------------------------------------------

bool ResMan::Setup()
{
	if (!SetDefaultMatrices() || !SetDefaultMaterials() || !SetDefaultRenderStates() || !SetDefaultLights())
		return false;

	//for DX7 remove formats from m_pCommonCubeFormats which are not supported as cube map formats
	//also check if mipmapped cube maps are supported
	if ( m_dwVersion < 0x0800 )
	{
		CCubeTexture *tempC		 = NULL;
		FORMAT		 *pTmpCubes	 = NULL;
		bool		 *pSupported = NULL;
		UINT		 uSupported	 = m_uCommonCubeFormats;
		UINT		 uNotMipped	 = 0;
		pSupported = new bool[ m_uCommonCubeFormats ];

		for(UINT i = 0; i < m_uCommonCubeFormats; i++)
		{
			//check if this format is supported with mip-mapped cube maps in dx7
			if (E_NOTIMPL == m_pSrcDevice->CreateCubeTexture(4, 0, 0, m_pCommonCubeFormats[i], POOL_DEFAULT, &tempC))
				uNotMipped++;
			RELEASE(tempC);
			
			//check if this format is supported with cube maps in dx7
			if (E_NOTIMPL == m_pSrcDevice->CreateCubeTexture(4, 1, 0, m_pCommonCubeFormats[i], POOL_DEFAULT, &tempC))
			{
				uSupported--;
				pSupported[ i ] = false;
			}
			else
				pSupported[ i ] = true;
			RELEASE(tempC);
		}

		//since mip mapped cube maps failed to be created for all formats -> mip mapping is not supported on cube maps
        if ( uNotMipped == m_uCommonCubeFormats )
			m_UseMipCubeMaps = false;

		if ( uSupported != m_uCommonCubeFormats )
		{
			UINT j = 0;
			if ( uSupported > 0 )
				pTmpCubes = new FORMAT[ uSupported ];

			for(UINT i = 0; i < uSupported; i++)
			{
				while ( !pSupported[ j ] )
				{
					j++;
				}

				pTmpCubes[ i ].ddpfPixelFormat = m_pCommonTextureFormats[ j ].ddpfPixelFormat;
				pTmpCubes[ i ].d3dfFormat      = m_pCommonTextureFormats[ j ].d3dfFormat;
				j++;
			}

			for(UINT i = 0; i < uSupported; i++)
			{
				if ( i < uSupported - 1 )
					pTmpCubes[ i ].pNext = &pTmpCubes[ i + 1 ];
				else
					pTmpCubes[ i ].pNext = NULL;
			}

			SAFEDELETEA(m_pCommonCubeFormats);
		    m_pCommonCubeFormats = pTmpCubes;
			m_uCommonCubeFormats = uSupported;
		}

		delete [] pSupported;
	}

	//need to check if USAGE_QUERY_WRAPANDMIP query flag is set for each format
	
	if (m_uCommonTextureFormats > 0)
		m_bWrapAndMipTex  = new bool [m_uCommonTextureFormats];
	else
		m_bWrapAndMipTex  = NULL;

	if (m_uCommonCubeFormats > 0)
		m_bWrapAndMipCube = new bool [m_uCommonCubeFormats   ];
	else
		m_bWrapAndMipCube  = NULL;

	if (m_uCommonVolumeFormats > 0)
		m_bWrapAndMipVol  = new bool [m_uCommonVolumeFormats ];
	else
		m_bWrapAndMipVol  = NULL;

	for (UINT i = 0; i < m_uCommonTextureFormats; i++)
	{
		if (m_pD3D->GetRuntimeVersion() >= 9.029f && 
			FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
                                             USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, m_pCommonTextureFormats[i])))
			m_bWrapAndMipTex[i] = false;
		else
			m_bWrapAndMipTex[i] = true;
	}

	for (UINT i = 0; i < m_uCommonCubeFormats; i++)
	{
		if (m_pD3D->GetRuntimeVersion() >= 9.029f && 
			FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
                                             USAGE_QUERY_WRAPANDMIP, RTYPE_CUBETEXTURE, m_pCommonCubeFormats[i])))
			m_bWrapAndMipCube[i] = false;
		else
			m_bWrapAndMipCube[i] = true;
	}

	for (UINT i = 0; i < m_uCommonVolumeFormats; i++)
	{
		if (m_pD3D->GetRuntimeVersion() >= 9.029f && 
			FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
                                             USAGE_QUERY_WRAPANDMIP, RTYPE_VOLUMETEXTURE, m_pCommonVolumeFormats[i])))
			m_bWrapAndMipVol[i] = false;
		else
			m_bWrapAndMipVol[i] = true;
	}

	return true;
}

//-----------------------------------------------------------------------------

UINT ResMan::TestInitialize(void)
{
	m_TotalNumberOfSlices = 0;
	m_uPrevMaxMipLevel	  = 0;

	D3DXMATRIX Matrix;
	D3DXMatrixIdentity(&Matrix);
	m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix);

	if (m_DeviceList[0]->GetDevType() == DEVICETYPE_REF && m_DeviceList[0]->GetDDI() < 0x08)
	{
		WriteToLog("ResMan: Driver resource management is not supported on dx6 and dx7 reference device.\n");
		return D3DTESTINIT_SKIPALL;
	} 

	if ( !FindScreenFormat() )  //determines screen format
	{
		WriteToLog("ResMan: Screen format isn't set.\n");
		return D3DTESTINIT_ABORT;
	}

	if (!KeySet("notmaxsize") && !FindMaxTexSizes())
		return D3DTESTINIT_ABORT;

	return D3DTESTINIT_RUN;
}

//-----------------------------------------------------------------------------

bool ResMan::ClearFrame(void)
{
	DPF(4, _T("ResMan::ClearFrame\n"));
	return Clear(0, NULL, FMWK_DEFAULT, 0x00000000, 1.f, 0);
}

//-----------------------------------------------------------------------------

void ResMan::Cleanup()
{
	if (NULL != m_bWrapAndMipTex)
		delete [] m_bWrapAndMipTex;

	if (NULL != m_bWrapAndMipCube)
		delete [] m_bWrapAndMipCube;

	if (NULL != m_bWrapAndMipVol)
		delete [] m_bWrapAndMipVol;

}

//-----------------------------------------------------------------------------

void ResMan::CommandLineHelp(void)
{
    WriteCommandLineHelp(_T("$w\td3dmanage\t$gUse runtime resource management - full test run."));
    WriteCommandLineHelp(_T("$w\tdrivermanage\t$gUse driver resource management."));
    WriteCommandLineHelp(_T("$w\tmaxmiplevel\t$gUse SetMaxMipLevel for mipmaping."));
	//WriteCommandLineHelp(_T("$y\tNOTMANDEV\t$gNot to use managed devices."));
}

//-----------------------------------------------------------------------------

bool ResMan::FindScreenFormat()
{
	CSurface *pSrcSurf = NULL;
	HRESULT   hr;

	ADAPTER *pAdapter = NULL;
	pAdapter = m_pSrcDevice->GetAdapter();
	if( pAdapter == NULL )
	{
		WriteToLog("FindScreenFormat: GetAdapter() failed\n" );
		return false;
	}
	m_ScreenFormat = pAdapter->DesktopMode.Format;

	hr = m_pSrcDevice->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &pSrcSurf);
	if(FAILED(hr))
	{
		DPF(1, _T("FindScreenFormat(): GetBackBufferCached failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
		return false;
	}
	//m_screenRedMask   = pSrcSurf->m_Mask.dwRedMask;
	//m_screenGreenMask = pSrcSurf->m_Mask.dwGreenMask;
	//m_screenBlueMask  = pSrcSurf->m_Mask.dwBlueMask;
	RELEASE(pSrcSurf);

	return true;
}

//-----------------------------------------------------------------------------

bool ResMan::FindMaxTexSizes()
{
	FORMAT tempF;
	DWORD tempW = m_MaxTextureWidth, tempH = m_MaxTextureHeight, tempE = m_MaxVolumeExtent;
	CTexture *tempT = NULL;
	CCubeTexture *tempC = NULL;
	CVolTexture *tempV = NULL;
	bool go = true;
	HRESULT hr;

	if (m_UseTextures)
	{
		tempF = m_pCommonTextureFormats[0];
		for (int i = 1; i < m_uCommonTextureFormats; i++)
			if ( m_pCommonTextureFormats[i].ddpfPixelFormat.dwRGBBitCount > tempF.ddpfPixelFormat.dwRGBBitCount)
				tempF = m_pCommonTextureFormats[i];

		//need to do this check here in case if m_MaxTextureWidth or m_MaxTextureHeight is nonpowtwo
		if ((m_TextureIsPow2 && !m_NonPow2Conditional)       || 
			(m_NonPow2Conditional && m_UseMipMaps/*(isDXTn || m_UseMipMaps)*/)   ||
		    (m_UseMipMaps && m_dwVersion < 0x0800)                )//nonpow2 mipmaped textures are broken on dx7
		{
			int pow1 = (int) (log((float)tempW)/log(2.f));
			int pow2 = (int) (log((float)tempH)/log(2.f));
			tempW    = 1 << pow1; //pow(2, pow1);
			tempH    = 1 << pow2; //pow(2, pow2);
		}

		while (go)
		{
			hr = m_pSrcDevice->CreateTexture(tempW, tempH, m_UseMipMaps?0:1, 0, tempF, POOL_DEFAULT, &tempT);
			if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
			{
				if (tempW<=1)
					go = false;
				tempW >>= 1;
				tempH >>= 1;
			}
			else
			{
				go = false;
				RELEASE(tempT);
			}
			m_pSrcDevice->GetLastError();
		}

		m_MaxTextureWidth  = (WORD) tempW >> 1;
		m_MaxTextureHeight = (WORD) tempH >> 1;
				
		if (m_MaxTextureWidth == 0 || m_MaxTextureHeight == 0)
		{
			WriteToLog("Memory is full. Can't fill textures in.\n");
			return false;
		}
	}
	if (m_UseCubeMaps)
	{
		go = true;
		tempW = m_MaxCubeWidth;
		tempF = FMT_UNKNOWN;

		//need to do this check here in case if m_MaxCubeWidth is nonpowtwo
		if ((m_CubeMapIsPow2 /*&& !m_NonPow2Conditional*/) || 
			/*(m_NonPow2Conditional && (isDXTn || !uLevels)) ||*/
		    m_dwVersion < 0x0800) //nonpow2 cube maps are broken on dx7 interfaces
		{
			int pow = (int)(log((float)tempW)/log(2.f));
			tempW   = 1 << pow;
		}
		
		for (int i = 0; i < m_uCommonCubeFormats; i++)
		{
			if (tempF.d3dfFormat != FMT_UNKNOWN &&
				m_pCommonCubeFormats[i].ddpfPixelFormat.dwRGBBitCount <= tempF.ddpfPixelFormat.dwRGBBitCount)
				continue;

			if ( m_dwVersion < 0x0800 )
			{
				//check if this format is supported with cube maps in dx7
				if (E_NOTIMPL == m_pSrcDevice->CreateCubeTexture(4, 1, 0, m_pCommonCubeFormats[i], POOL_DEFAULT, &tempC))
					continue;
				RELEASE(tempC);

				//check if this format is supported with mip-mapped cube maps in dx7
				if (E_NOTIMPL == m_pSrcDevice->CreateCubeTexture(4, 0, 0, m_pCommonCubeFormats[i], POOL_DEFAULT, &tempC))
				{
					WriteToLog("CreateCubeTexture() returns E_NOTIMPL for mipmapped cubes => we skip cases with mipmapped cube textures.");
					m_UseMipCubeMaps = false;
				}
				RELEASE(tempC);
			}

			tempF = m_pCommonCubeFormats[i];
		}

		if ( tempF.d3dfFormat == FMT_UNKNOWN )
			go = false;

		while (go)
		{
			hr = m_pSrcDevice->CreateCubeTexture(tempW, m_UseMipCubeMaps?0:1, 0, tempF, POOL_DEFAULT, &tempC);
			if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
			{
				if (tempW<=1)
					go = false;
				tempW >>= 1;
				//hr = m_pSrcDevice->GetLastError();
			}
			else
			{
				go = false;
				RELEASE(tempC);
			}
			m_pSrcDevice->GetLastError();
		}

		m_MaxCubeWidth = (WORD)tempW >> 1;
		
		if (m_MaxCubeWidth == 0)
		{
			WriteToLog("Memory is full. Can't fill cubees in.\n");
			return false;
		}
	}
	if (m_UseVolMaps)
	{
		go = true;
		tempF = m_pCommonVolumeFormats[0];
		for (int i = 1; i < m_uCommonVolumeFormats; i++)
			if ( m_pCommonVolumeFormats[i].ddpfPixelFormat.dwRGBBitCount > tempF.ddpfPixelFormat.dwRGBBitCount)
				tempF = m_pCommonVolumeFormats[i];

		//need to do this check here in case m_MaxVolumeExtent is nonpowtwo
        if ((m_VolumeMapIsPow2 /*&& !m_NonPow2Conditional*/) /*|| 
			(m_NonPow2Conditional && (isDXTn || !uLevels))*/)
		{
			int pow = (int)(log((float)tempE )/log(2.f));
			tempE   = 1 << pow;
		}

		while (go)
		{
			hr = m_pSrcDevice->CreateVolumeTexture(tempE, tempE, min(32,tempE), m_UseMipVolMaps?0:1, 0, tempF, POOL_DEFAULT, &tempV);
			if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
			{
				if (tempW<=1)
					go = false;
				tempE >>= 1;
				//hr = m_pSrcDevice->GetLastError();
			}
			else
			{
				go = false;
				RELEASE(tempV);
			}
			m_pSrcDevice->GetLastError();
		}

		m_MaxVolumeExtent = (WORD)tempE >> 1;
		
		if (m_MaxVolumeExtent == 0)
		{
			WriteToLog("Memory is full. Can't fill volumes in.\n");
			return false;
		}
	}

	//WriteToLog("maxTexW=%d maxTexH=%d maxCubeW=%d maxVolD=%d.\n", m_MaxTextureWidth, m_MaxTextureHeight, m_MaxCubeWidth, m_MaxVolumeExtent);
	return true;
}