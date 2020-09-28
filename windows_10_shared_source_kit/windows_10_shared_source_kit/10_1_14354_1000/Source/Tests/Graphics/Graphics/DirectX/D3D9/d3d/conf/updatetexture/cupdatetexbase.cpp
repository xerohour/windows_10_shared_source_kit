//
//  CUpdateTex.cpp
//
//  Class implementation for CUpdateTex.
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 1998 Microsoft Corporation.
//
//  Original Author :  xfu
//

#include "CUpdateTex.h"

//
// Declare the framework
//
                   
CD3DWindowFramework	App;

//
// Define the groups here
//

GROUP_CREATE(UpdateTex_MipTextures);
GROUP_CREATE(UpdateTex_CubeTextures);

const	UINT	GENERICTEXSIZE=64;
#if FAST_TEST
const	UINT	GENERICTEXLEVEL=2;
#else //FAST_TEST
const	UINT	GENERICTEXLEVEL = 7;
#endif /FAST_TEST

CUpdateTexBase::CUpdateTexBase()
{
	SetTestType(TESTTYPE_CONF);

	m_Options.TestOptions.bCompareSrcRef = true;

	m_pSrcTex = NULL;
	m_pDestTex = NULL;
	m_pBogusTex = NULL;
}

CUpdateTexBase::~CUpdateTexBase()
{
}

UINT CUpdateTexBase::TestInitialize()
{
	HRESULT			hr;

    UINT uResult = CD3DTest::TestInitialize();
	if(uResult == D3DTESTINIT_RUN)
	{
		m_pDevice = m_pSrcDevice;

		m_pDevice -> GetDirect3D(&m_pD3Dxfu);

		m_DeviceType = (D3DDEVTYPE)(m_pSrcDevice -> GetDevType());

		m_caps = *(m_pSrcDevice -> GetCaps());

		m_hWnd = (App.m_pDisplayWindow)->m_hWindow;
	
		m_bDebugMode=IsDebugMode();

		if (m_caps.dpcTriCaps.dwTextureCaps&D3DPTEXTURECAPS_MIPMAP)
		{
			m_bCanDoMipmap=TRUE;
			m_uGenericMipLevel=0;
		}
		else 
		{
			m_bCanDoMipmap=FALSE;
			m_uGenericMipLevel=1;
		}

		if (m_caps.dpcTriCaps.dwTextureCaps&D3DPTEXTURECAPS_CUBEMAP)
		{
			m_bCanDoCubeMap=TRUE;
		}
		else 
		{
			m_bCanDoCubeMap=FALSE;
		}

		if (m_caps.dpcTriCaps.dwTextureCaps&D3DPTEXTURECAPS_VOLUMEMAP)
		{
			m_bCanDoVolMap=TRUE;
		}
		else 
		{
			m_bCanDoVolMap=FALSE;
		}

		if (m_caps.dpcTriCaps.dwTextureCaps&D3DPTEXTURECAPS_MIPCUBEMAP)
		{
			m_bCanDoCubeMipmap=TRUE;
		}
		else 
		{
			m_bCanDoCubeMipmap=FALSE;
		}

		if (m_caps.dpcTriCaps.dwTextureCaps&D3DPTEXTURECAPS_MIPVOLUMEMAP)
		{
			m_bCanDoVolMipmap=TRUE;
		}
		else 
		{
			m_bCanDoVolMipmap=FALSE;
		}

		if (m_caps.dpcTriCaps.dwTextureCaps&D3DPTEXTURECAPS_SQUAREONLY)
			m_bCanDoNonSquare=FALSE;
		else 
			m_bCanDoNonSquare=TRUE;

		DISPLAYMODE Mode;

		hr=m_pD3Dxfu->GetAdapterDisplayMode(m_pSrcDevice -> GetAdapterID(), &Mode);

		if (FAILED(hr))
				WriteToLog (TEXT("\nFailed: GetAdapterDisplayMode failed in CUpdateTexBase::TestInitialize.\n"));
		else
		{
			CSurface *lpsurfBB = NULL;

			m_AdapterFormat=Mode.Format;

			if (FAILED((hr = m_pSrcDevice -> GetBackBuffer(0, BACKBUFFER_TYPE_MONO, &lpsurfBB))))
				WriteToLog(TEXT("GetBackBuffer failed in CUpdateTexBase::TestInitialize.\n"));
			else
			{
				SURFACEDESC sdBB;

				hr = lpsurfBB -> GetDesc(&sdBB);
				lpsurfBB -> Release();
				if (FAILED(hr))
					WriteToLog(TEXT("GetDesc (back buffer) failed in CUpdateTexBase::TestInitialize.\n"));
				else
				{
					rectBB.left = (rectBB.top = 0);
					rectBB.right = sdBB.Width;
					rectBB.bottom = sdBB.Height;
					return D3DTESTINIT_RUN;
				}
			}
		}
	}

	SetTestRange(1, 1);
	return D3DTESTINIT_SKIPALL;
}

BOOL CUpdateTexBase::IsDebugMode()
{
	HKEY hKey1;
	DWORD LoadDebugRunTime;
	DWORD size=sizeof (LoadDebugRunTime);

	::RegOpenKeyEx (HKEY_LOCAL_MACHINE,TEXT("Software\\microsoft\\Direct3D") ,0,KEY_READ,&hKey1);
	::RegQueryValueEx(hKey1,TEXT("LoadDebugRuntime"),NULL,NULL,(LPBYTE)&LoadDebugRunTime,&size);
	::RegCloseKey (hKey1);
	if (LoadDebugRunTime==1) 
		return TRUE;
	else 
		return FALSE;
}

BOOL  CUpdateTexBase::CreateTex(D3DRESOURCETYPE ResType,
									 D3DFORMAT Format, 
									 D3DPOOL Pool,
									 UINT uSize,
									 UINT cLevels,
									 DWORD dwUsage,
									 SURFACEVALUE Value,
									 CBaseTex ** ppTexture,
									 BOOL bInit
									 )
{

			BOOL	bRet;

			//
			//	Create texture
			//

			if (ResType==D3DRTYPE_TEXTURE)
			{
				*ppTexture  = new CMipTex (this,m_pDevice,Format,Pool,uSize,cLevels,dwUsage);
			}

			else if (ResType==D3DRTYPE_CUBETEXTURE)
			{
				*ppTexture  = new CCubeTex (this,m_pDevice,Format,Pool,uSize,cLevels,dwUsage);
			}

			else
			{
				*ppTexture  = new CVolTex (this,m_pDevice,Format,Pool,uSize,cLevels);			
			}

			bRet= ((*ppTexture)->ValidateTexture());
			if (bRet&&bInit)
				(*ppTexture)->SetTexValue(Value);
			return bRet;
}
