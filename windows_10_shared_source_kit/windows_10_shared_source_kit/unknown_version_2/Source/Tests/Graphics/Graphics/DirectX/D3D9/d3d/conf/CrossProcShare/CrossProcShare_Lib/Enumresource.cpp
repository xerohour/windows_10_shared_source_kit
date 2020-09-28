//
//  EnumResource.cpp
//
//  This class allows to get all valid surface from these interfaces :
//
//  IDirect3DDevice::CreateOffscreenPlainSurface
//					 CreateRenderTarget
//					 CreateDepthStencilSurface
//					 CreateTexture
//					 CreateCubeTexture
//					 CreateVolumeTexture
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 2002 Microsoft Corporation.
//
//  Original Author :	ChrisLav
//

#include "EnumResource.h"

#include "CTextures.h"
#include "d3d9.h"

const TCHAR *TypeName[] =
{
	_T("OffscreenPlain"),
	_T("RenderTarget"),
	_T("DepthStencil"),
	_T("Texture"),
	_T("CubeTexture"),
	_T("VolumeTexture"),
	_T("VertexBuffer"),
	_T("IndexBuffer")
};

RESOURCE_POOL g_pools[] =
{
	{ POOL_DEFAULT,		_T("Default"),		8.0f, 9.2f },
	{ POOL_MANAGED,		_T("Managed"),		8.0f, 9.2f },
	{ POOL_SYSTEMMEM,	_T("SystemMem"),	8.0f, 9.2f },
	{ POOL_SCRATCH,		_T("Scratch"),		8.1f, 9.2f }
};
const UINT cPool = sizeof(g_pools) / sizeof(g_pools[0]);

RESOURCE_FORMAT g_formats[] =
{
	{ FMT_R8G8B8               	,	_T("FMT_R8G8B8"),		8.0f, 9.2f },
	{ FMT_A8R8G8B8             	,	_T("FMT_A8R8G8B8"),		8.0f, 9.2f },
	{ FMT_X8R8G8B8             	,	_T("FMT_X8R8G8B8"),		8.0f, 9.2f },
	{ FMT_R5G6B5               	,	_T("FMT_R5G6B5"),		8.0f, 9.2f },
	{ FMT_X1R5G5B5             	,	_T("FMT_X1R5G5B5"),		8.0f, 9.2f },
	{ FMT_A1R5G5B5             	,	_T("FMT_A1R5G5B5"),		8.0f, 9.2f },
	{ FMT_A4R4G4B4             	,	_T("FMT_A4R4G4B4"),		8.0f, 9.2f },
	{ FMT_R3G3B2               	,	_T("FMT_R3G3B2"),		8.0f, 9.2f },
	{ FMT_A8                   	,	_T("FMT_A8"),			8.0f, 9.2f },
	{ FMT_A8R3G3B2             	,	_T("FMT_A8R3G3B2"),		8.0f, 9.2f },
	{ FMT_X4R4G4B4             	,	_T("FMT_X4R4G4B4"),		8.0f, 9.2f },
	{ FMT_A2B10G10R10          	,	_T("FMT_A2B10G10R10"),	8.1f, 9.2f },
	{ FMT_A8B8G8R8             	,	_T("FMT_A8B8G8R8"),		9.0f, 9.2f },
	{ FMT_X8B8G8R8             	,	_T("FMT_X8B8G8R8"),		9.0f, 9.2f },
	{ FMT_G16R16               	,	_T("FMT_G16R16"),		8.1f, 9.2f },
	{ FMT_A2R10G10B10          	,	_T("FMT_A2R10G10B10"),	9.0f, 9.2f },
	{ FMT_A16B16G16R16			,	_T("FMT_A16B16G16R16"),	9.0f, 9.2f },

	{ FMT_L8                   	,	_T("FMT_L8"),			8.0f, 9.2f },
	{ FMT_A8L8                 	,	_T("FMT_A8L8"),			8.0f, 9.2f },
	{ FMT_A4L4                 	,	_T("FMT_A4L4"),			8.0f, 9.2f },
	{ FMT_L16                  	,	_T("FMT_L16"),			9.0f, 9.2f },

	{ FMT_V8U8                 	,	_T("FMT_V8U8"),			8.0f, 9.2f },
	{ FMT_L6V5U5               	,	_T("FMT_L6V5U5"),		8.0f, 9.2f },
	{ FMT_X8L8V8U8             	,	_T("FMT_X8L8V8U8"),		8.0f, 9.2f },
	{ FMT_Q8W8V8U8             	,	_T("FMT_Q8W8V8U8"),		8.0f, 9.2f },
	{ FMT_V16U16               	,	_T("FMT_V16U16"),		8.0f, 9.2f },
	{ FMT_W11V11U10            	,	_T("FMT_W11V11U10"),	8.0f, 8.1f },

    { FMT_A2W10V10U10          	,	_T("FMT_A2W10V10U10"),	8.1f, 9.2f },

	{ FMT_Q16W16V16U16         	,	_T("FMT_Q16W16V16U16"),	9.0f, 9.2f },

	{ FMT_UYVY                 	,	_T("FMT_UYVY"),			8.0f, 9.2f },
	{ FMT_YUY2                 	,	_T("FMT_YUY2"),			8.0f, 9.2f },

	{ FMT_D16_LOCKABLE         	,	_T("FMT_D16_LOCKABLE"),	8.0f, 9.2f },
	{ FMT_D32                  	,	_T("FMT_D32"),			8.0f, 9.2f },
	{ FMT_D15S1                	,	_T("FMT_D15S1"),		8.0f, 9.2f },
	{ FMT_D24S8                	,	_T("FMT_D24S8"),		8.0f, 9.2f },
	{ FMT_D24X8                	,	_T("FMT_D24X8"),		8.0f, 9.2f },
	{ FMT_D24X4S4              	,	_T("FMT_D24X4S4"),		8.0f, 9.2f },
	{ FMT_D16                  	,	_T("FMT_D16"),			8.0f, 9.2f },
	{ FMT_D32F_LOCKABLE        	,	_T("FMT_D32F_LOCKABLE"),9.0f, 9.2f },
	{ FMT_D24FS8               	,	_T("FMT_D24FS8"),		8.0f, 9.2f },

	{ FMT_VERTEXDATA           	,	_T("FMT_VERTEXDATA"),	8.0f, 9.2f },
	{ FMT_INDEX16              	,	_T("FMT_INDEX16"),		8.0f, 9.2f },
	{ FMT_INDEX32              	,	_T("FMT_INDEX32"),		8.0f, 9.2f },

	{ FMT_MULTI2_ARGB8         	,	_T("FMT_MULTI2_ARGB8"),	9.0f, 9.2f },

	{ FMT_R16F                 	,	_T("FMT_R16F"),			9.0f, 9.2f },
	{ FMT_G16R16F              	,	_T("FMT_G16R16F"),		9.0f, 9.2f },
	{ FMT_A16B16G16R16F        	,	_T("FMT_A16B16G16R16F"),9.0f, 9.2f },

	{ FMT_R32F                 	,	_T("FMT_R32F"),			9.0f, 9.2f },
	{ FMT_G32R32F              	,	_T("FMT_G32R32F"),		9.0f, 9.2f },
	{ FMT_A32B32G32R32F        	,	_T("FMT_A32B32G32R32F"),9.0f, 9.2f },

	{ FMT_CxV8U8               	,	_T("FMT_CxV8U8"),		9.0f, 9.2f },
};
const UINT cFormat = sizeof(g_formats) / sizeof(g_formats[0]);

RESOURCE_FORMAT g_formatsDS[] =
{
	{ FMT_D16_LOCKABLE         	,	_T("FMT_D16_LOCKABLE"),	8.0f, 9.2f },
	{ FMT_D32                  	,	_T("FMT_D32"),			8.0f, 9.2f },
	{ FMT_D15S1                	,	_T("FMT_D15S1"),		8.0f, 9.2f },
	{ FMT_D24S8                	,	_T("FMT_D24S8"),		8.0f, 9.2f },
	{ FMT_D24X8                	,	_T("FMT_D24X8"),		8.0f, 9.2f },
	{ FMT_D24X4S4              	,	_T("FMT_D24X4S4"),		8.0f, 9.2f },
	{ FMT_D16                  	,	_T("FMT_D16"),			8.0f, 9.2f },
	{ FMT_D32F_LOCKABLE        	,	_T("FMT_D32F_LOCKABLE"),9.0f, 9.2f },
	{ FMT_D24FS8               	,	_T("FMT_D24FS8"),		8.0f, 9.2f }
};
const UINT cFormatDS = sizeof(g_formatsDS) / sizeof(g_formatsDS[0]);

RESOURCE_USAGE g_usages[] =
{
	{ 8.0f, 9.2f, 0,                        _T("Zero") },
	{ 8.0f, 9.2f, USAGE_RENDERTARGET,		_T("RenderTarget") },
	{ 8.0f, 9.2f, USAGE_DEPTHSTENCIL,		_T("DepthStencil") },
	{ 8.1f, 9.2f, USAGE_DYNAMIC,			_T("Dynamic") },
	{ 9.0f, 9.2f, USAGE_AUTOGENMIPMAP,		_T("AutoGenMipMap") },

	{ 9.0f, 9.2f, USAGE_AUTOGENMIPMAP | USAGE_RENDERTARGET,					_T("AutoGenMipMap | RenderTarget") },
	{ 9.0f, 9.2f, USAGE_AUTOGENMIPMAP | USAGE_DEPTHSTENCIL,					_T("AutoGenMipMap | DepthStencil") },
	{ 9.0f, 9.2f, USAGE_AUTOGENMIPMAP | USAGE_DYNAMIC,						_T("AutoGenMipMap | Dynamic") },
};	  
const UINT cUsage = sizeof(g_usages) / sizeof(g_usages[0]);

RESOURCE_USAGE g_usagesVB[] =
{
	{ 8.0f, 9.2f, 0,                                _T("Zero") },
	{ 8.0f, 9.2f, USAGE_DYNAMIC,                    _T("Dynamic") },
	{ 8.1f, 9.2f, USAGE_WRITEONLY,	                _T("WriteOnly") },

	{ 9.0f, 9.2f, USAGE_DYNAMIC | USAGE_WRITEONLY,	_T("Dynamic | WriteOnly") },
};	  
const UINT cUsageVB = sizeof(g_usagesVB) / sizeof(g_usagesVB[0]);
/////////////////////////////////////////////////////////////////////////////////////////////
//
//  Class : CEnumResource
//
/////////////////////////////////////////////////////////////////////////////////////////////

CEnumResource::CEnumResource()
{
}

CEnumResource::~CEnumResource(void)
{
}

bool CEnumResource::Init(CD3DTest *pD3DTest)
{
	m_fDXVersion = pD3DTest->m_DeviceList[0]->GetInterface();
	m_pD3D = pD3DTest->m_pD3D;
	m_pDevice = pD3DTest->m_DeviceList[0];
	m_dwDeviceType = pD3DTest->m_DeviceList[0]->GetDevType();
	m_formatAdapter = (FMT)pD3DTest->m_DeviceList[0]->GetAdapter()->DesktopMode.Format.d3dfFormat;
	m_pD3DTest = pD3DTest;

	for (int i=0; i<TYPE_MAXTYPE; i++)
		m_iCurrentCase[i] = -1;

	m_nMultiSample = 17; // includes invalid "1"

    FindComapreRenderTargetFMT(&m_formatCompareRT);

	GetCommandLineOptions();
	GenerateAllCases();

	return true;
}

void CEnumResource::Clean()
{
	std::vector <RESOURCE_CASE *>::iterator caseIter;

	for( int iType=0; iType<TYPE_MAXTYPE; iType++ )
	{
		// Delete the cases
		for( caseIter = m_listCases[iType].begin(); m_listCases[iType].end() != caseIter; caseIter++ )
		{
			RESOURCE_CASE* pResCase = (RESOURCE_CASE*)*caseIter;
			delete pResCase;
		}
		m_listCases[iType].clear();
	}
}

bool CEnumResource::FormatNotInCurrentDX(UINT uFormat)
{
	return ( (m_fDXVersion > g_formats[uFormat].fMaxDX) || (m_fDXVersion < g_formats[uFormat].fMinDX) );
}

bool CEnumResource::PoolNotInCurrentDX(UINT uPool)
{
	return ( (m_fDXVersion > g_pools[uPool].fMaxDX) || (m_fDXVersion < g_pools[uPool].fMinDX) );
}

bool CEnumResource::UsageNotInCurrentDX(UINT uUsage)
{
	return ( (m_fDXVersion > g_usages[uUsage].fMaxDX) || (m_fDXVersion < g_usages[uUsage].fMinDX) );
}

UINT CEnumResource::GetNbCasesForPool(DWORD dwRTYPE, DWORD dwPool)
{
	UINT uCount = 0;

	for (int i=0; i<m_listCases[dwRTYPE].size(); i++)
	{
		if ( m_listCases[dwRTYPE][i]->dwPool == dwPool )
			uCount++;
	}

	return uCount;
}

int CEnumResource::GetLastCaseForPool(DWORD dwRTYPE, DWORD dwPool)
{
	for (int i=m_listCases[dwRTYPE].size(); i>0; i--)
	{
		if ( m_listCases[dwRTYPE][i-1]->dwPool == dwPool )
			return i;
	}

	return -1;
}

void CEnumResource::GetCommandLineOptions()
{
	TCHAR	szOption[512] = _T("");

	// Check if we override the pool to test
	m_dwPoolToTest = 0xffffffff;
	if(m_pD3DTest->ReadString(_T("POOL"), szOption, ARRAYSIZE(szOption)))
	{
		// Convert to uppercase
		_tcsupr(szOption);

		if(_tcsstr(szOption, _T("DEFAULT")))
		{
			m_dwPoolToTest = POOL_DEFAULT;
		}
		else if(_tcsstr(szOption, _T("MANAGED")))
		{
			m_dwPoolToTest = POOL_MANAGED;
		}
		else if(_tcsstr(szOption, _T("SYSTEMMEM")))
		{
			m_dwPoolToTest = POOL_SYSTEMMEM;
		}
		else if(_tcsstr(szOption, _T("SCRATCH")))
		{
			m_dwPoolToTest = POOL_SCRATCH;
		}
	}

	// Check if we override the usage to test
	m_dwUsageToTest = 0xffffffff;
	if(m_pD3DTest->ReadString(_T("USAGE"), szOption, ARRAYSIZE(szOption)))
	{
		// Convert to uppercase
		_tcsupr(szOption);

		if(_tcsstr(szOption, _T("0")))
		{
			m_dwUsageToTest = 0;
		}
		else if(_tcsstr(szOption, _T("RENDERTARGET")))
		{
			m_dwUsageToTest = USAGE_RENDERTARGET;
		}
		else if(_tcsstr(szOption, _T("DYNAMIC")))
		{
			m_dwUsageToTest = USAGE_DYNAMIC;
		}
		else if(_tcsstr(szOption, _T("DEPTHSTENCIL")))
		{
			m_dwUsageToTest = USAGE_DEPTHSTENCIL;
		}
		else if(_tcsstr(szOption, _T("AUTOGENMIPMAP")))
		{
			m_dwUsageToTest = USAGE_AUTOGENMIPMAP;
		}
		else if(_tcsstr(szOption, _T("SOFTWAREPROCESSING")))
		{
			m_dwUsageToTest = USAGE_SOFTWAREPROCESSING;
		}
		else if(_tcsstr(szOption, _T("WRITEONLY")))
		{
			m_dwUsageToTest = USAGE_WRITEONLY;
		}
	}

    // Check if we override the type of resources to test
	m_dwRTYPEToTest = 0xffffffff;
	if(m_pD3DTest->ReadString(_T("RESOURCETYPE"), szOption, ARRAYSIZE(szOption)))
	{
		// Convert to uppercase
		_tcsupr(szOption);

		if(_tcsstr(szOption, _T("OFFSCREENPLAIN")))
		{
			m_dwRTYPEToTest = TYPE_OFFSCREENPLAIN;
		}
		else if(_tcsstr(szOption, _T("DEPTHSTENCIL")))
		{
			m_dwRTYPEToTest = TYPE_DEPTHSTENCIL;
		}
		else if(_tcsstr(szOption, _T("RENDERTARGET")))
		{
			m_dwRTYPEToTest = TYPE_RENDERTARGET;
		}
		else if(_tcsstr(szOption, _T("TEXTURE")))
		{
			m_dwRTYPEToTest = TYPE_TEXTURE;
		}
		else if(_tcsstr(szOption, _T("CUBETEXTURE")))
		{
			m_dwRTYPEToTest = TYPE_CUBETEXTURE;
		}
		else if(_tcsstr(szOption, _T("VOLUMETEXTURE")))
		{
			m_dwRTYPEToTest = TYPE_VOLUMETEXTURE;
		}
		else if(_tcsstr(szOption, _T("VERTEXBUFFER")))
		{
			m_dwRTYPEToTest = TYPE_VERTEXBUFFER;
		}
		else if(_tcsstr(szOption, _T("INDEXBUFFER")))
		{
			m_dwRTYPEToTest = TYPE_INDEXBUFFER;
		}
	}
}

void CEnumResource::GenerateRenderTargetCases()
{
	HRESULT hr;
	RESOURCE_CASE *pResCase = NULL;

	// Make sure we want to test the render targets
	if ( (m_dwRTYPEToTest == 0xffffffff) || (m_dwRTYPEToTest == TYPE_RENDERTARGET) )
	{
		// Make sure we want to test RenderTargets and Default pool
		if ( (USAGE_RENDERTARGET & m_dwUsageToTest) && (m_dwPoolToTest == 0xffffffff || m_dwPoolToTest == POOL_DEFAULT) )
		{
			// Create the RenderTarget cases
			for (int iFormat = 0; iFormat<cFormat; iFormat++)
			{
				if (FormatNotInCurrentDX(iFormat))
					continue;

				// Check if we only want to test 1 format (from command line)
				if ( (m_pD3DTest->m_Options.ModeOptions.TextureFmt != FMT_ANY) && (m_pD3DTest->m_Options.ModeOptions.TextureFmt != g_formats[iFormat].format) )
					continue;

				for (int iLockable = 0; iLockable<2; iLockable++)
				{
					hr = m_pD3D->CheckDeviceFormat(m_pDevice->GetAdapterID(), m_dwDeviceType, m_formatAdapter, USAGE_RENDERTARGET, RTYPE_SURFACE, g_formats[iFormat].format);
					if( FAILED(hr) || D3DOK_NOAUTOGEN == hr)
						continue;

					hr = m_pD3D->CheckDeviceMultiSampleType(m_pDevice->GetAdapterID(), m_dwDeviceType, g_formats[iFormat].format, true, (MULTISAMPLE_TYPE)0);
					if (FAILED(hr))
						continue;

					pResCase = new RESOURCE_CASE;
					ZeroMemory(pResCase, sizeof(RESOURCE_CASE));

					pResCase->dwRType = TYPE_RENDERTARGET;
					pResCase->format = g_formats[iFormat].format;
					_tcscpy(pResCase->szFormat, g_formats[iFormat].szFormat);
					pResCase->dwPool = POOL_DEFAULT;
					_tcscpy(pResCase->szPool, _T("Default"));
					pResCase->multiSample = (MULTISAMPLE_TYPE)0;
					pResCase->bLockable = iLockable == 1;
					pResCase->dwUsage = USAGE_RENDERTARGET;
					_tcscpy(pResCase->szUsage, _T("RenderTarget"));
					pResCase->uWidth = 64;
					pResCase->uHeight = 64;

					pResCase->bCanUpdateSurface = true;
					pResCase->bCanStretchRect = true;
					pResCase->bCanColorFill = true;
					pResCase->bCanRender = true;
                    pResCase->bRequiredShareable = 1 != iLockable;

					// Add the case to the list
					m_listCases[TYPE_RENDERTARGET].push_back(pResCase);
				}
			}
		}
	}
}

bool CEnumResource::FindComapreRenderTargetFMT(FMT* fmt)
{
	HRESULT hr;

    // Create the RenderTarget cases
    for( int iFormat = 0; iFormat < cFormat; iFormat++ )
    {
	    if(true == FormatNotInCurrentDX(iFormat) )
		    continue;

        hr = m_pD3D->CheckDeviceFormat(m_pDevice->GetAdapterID(), m_dwDeviceType, m_formatAdapter, USAGE_RENDERTARGET, RTYPE_SURFACE, g_formats[iFormat].format);
        if( FAILED(hr) || D3DOK_NOAUTOGEN == hr)
	        continue;

        hr = m_pD3D->CheckDeviceMultiSampleType(m_pDevice->GetAdapterID(), m_dwDeviceType, g_formats[iFormat].format, true, MULTISAMPLE_NONE);
        if (FAILED(hr))
	        continue;

        // Found
        *fmt = g_formats[iFormat].format;
        return true;
    }
    
    return false;
}

void CEnumResource::GenerateDepthStencilCases()
{
	HRESULT hr;
	RESOURCE_CASE *pResCase = NULL;

	// Make sure we want to test the depth stencil
	if ( (m_dwRTYPEToTest == 0xffffffff) || (m_dwRTYPEToTest == TYPE_RENDERTARGET) )
	{
		// Make sure we want to test DepthStencil and Default pool
		if ( (USAGE_DEPTHSTENCIL & m_dwUsageToTest) && (m_dwPoolToTest == 0xffffffff || m_dwPoolToTest == POOL_DEFAULT) )
		{
			// Create the DepthStencil cases
			for (int iFormat = 0; iFormat<cFormatDS; iFormat++)
			{
				if (FormatNotInCurrentDX(iFormat))
					continue;

				// Check if we only want to test 1 format (from command line)
				if ( (m_pD3DTest->m_Options.ModeOptions.TextureFmt != FMT_ANY) && (m_pD3DTest->m_Options.ModeOptions.TextureFmt != g_formats[iFormat].format) )
					continue;

				for (int iDiscard = 0; iDiscard < 2; iDiscard ++)
				{
					if( g_formatsDS[iFormat].format != FMT_D16_LOCKABLE &&
                        g_formatsDS[iFormat].format != FMT_D32F_LOCKABLE )
                        continue;

					hr = m_pD3D->CheckDeviceFormat(m_pDevice->GetAdapterID(), m_dwDeviceType, m_formatAdapter, USAGE_DEPTHSTENCIL, RTYPE_SURFACE, g_formatsDS[iFormat].format);
					if( FAILED(hr) || D3DOK_NOAUTOGEN == hr)
						continue;

					hr = m_pD3D->CheckDeviceMultiSampleType(m_pDevice->GetAdapterID(), m_dwDeviceType, g_formatsDS[iFormat].format, true, (MULTISAMPLE_TYPE)0);
					if (FAILED(hr))
						continue;

					pResCase = new RESOURCE_CASE;
					ZeroMemory(pResCase, sizeof(RESOURCE_CASE));

					pResCase->dwRType = TYPE_DEPTHSTENCIL;
					pResCase->format = g_formatsDS[iFormat].format;
					_tcscpy(pResCase->szFormat, g_formatsDS[iFormat].szFormat);
					pResCase->dwPool = POOL_DEFAULT;
					_tcscpy(pResCase->szPool, _T("Default"));
					pResCase->multiSample = (MULTISAMPLE_TYPE)0;
					pResCase->bDiscardable = iDiscard == 1;
					pResCase->bLockable = true;
					pResCase->dwUsage = USAGE_DEPTHSTENCIL;
					_tcscpy(pResCase->szUsage, _T("DepthStencil"));
					pResCase->uWidth = 64;
					pResCase->uHeight = 64;

					pResCase->bCanUpdateSurface = false;
					pResCase->bCanStretchRect = (iDiscard == 0) && (m_pDevice->GetCaps()->dwDevCaps2 & D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES);
					pResCase->bCanColorFill = false;
					pResCase->bCanRender = false;
                    pResCase->bRequiredShareable = false;

					//Add the case to the list
					m_listCases[TYPE_DEPTHSTENCIL].push_back(pResCase);
				}
			}
		}
	}
}

void CEnumResource::GenerateOffscreenPlainCases()
{
	HRESULT hr;
	RESOURCE_CASE *pResCase = NULL;

	// Make sure we want to test the offscreenplain surfaces
	if ( (m_dwRTYPEToTest == 0xffffffff) || (m_dwRTYPEToTest == TYPE_OFFSCREENPLAIN) )
	{
		// Make sure we want to test no usages
		if ( m_dwUsageToTest == 0 || m_dwUsageToTest == 0xffffffff )
		{
			// Create the OffscreenPlain cases
			for (int iPool = 0; iPool<cPool; iPool++)
			{
				if (PoolNotInCurrentDX(iPool))
					continue;

				// Make sure we want to test this pool
				if ( (m_dwPoolToTest != 0xffffffff) && (g_pools[iPool].dwPool != m_dwPoolToTest) )
					continue;

				for (int iFormat = 0; iFormat<cFormat; iFormat++)
				{
					if (FormatNotInCurrentDX(iFormat))
						continue;

					// Check if we only want to test 1 format (from command line)
					if ( (m_pD3DTest->m_Options.ModeOptions.TextureFmt != FMT_ANY) && (m_pD3DTest->m_Options.ModeOptions.TextureFmt != g_formats[iFormat].format) )
						continue;

					// POOL_MANAGED not allowed for OffscreenPlain
					if (g_pools[iPool].dwPool == POOL_MANAGED)
						continue;

					//Non-lockable Z formats not supported
					//IndexBuffer Formats not supported
					//Unknown and Force_DWORD should fail
					if (	(g_formats[iFormat].format == FMT_UNKNOWN) ||
							(g_formats[iFormat].format == FMT_FORCE_DWORD)    ||
							(g_formats[iFormat].format == FMT_D16)    ||
							(g_formats[iFormat].format == FMT_D15S1)    ||
							(g_formats[iFormat].format == FMT_D24X8)    ||
							(g_formats[iFormat].format == FMT_D24S8)    ||
							(g_formats[iFormat].format == FMT_D24FS8)    ||
							(g_formats[iFormat].format == FMT_D24X4S4)  ||
							(g_formats[iFormat].format == FMT_D32)  ||
							(g_formats[iFormat].format == FMT_VERTEXDATA)  ||
							(g_formats[iFormat].format == FMT_INDEX16)  ||
							(g_formats[iFormat].format == FMT_INDEX32) )
					{
						continue;
					}

                    // FMT_D16_LOCKABLE not supported for read-only cases.
                    if( POOL_SYSTEMMEM == g_pools[iPool].dwPool && FMT_D16_LOCKABLE == g_formats[iFormat].format )
                    {
                        continue;
                    }

					hr = m_pD3D->CheckDeviceFormat(m_pDevice->GetAdapterID(), m_dwDeviceType, m_formatAdapter, 0, RTYPE_SURFACE, g_formats[iFormat].format);
					if( FAILED(hr) || D3DOK_NOAUTOGEN == hr)
						continue;

					pResCase = new RESOURCE_CASE;
					ZeroMemory(pResCase, sizeof(RESOURCE_CASE));

					pResCase->dwRType = TYPE_OFFSCREENPLAIN;
					pResCase->format = g_formats[iFormat].format;
					_tcscpy(pResCase->szFormat, g_formats[iFormat].szFormat);
					pResCase->dwPool = g_pools[iPool].dwPool;
					_tcscpy(pResCase->szPool, g_pools[iPool].szPool);
					pResCase->bLockable = true;
					pResCase->dwUsage = 0;
					_tcscpy(pResCase->szUsage, _T("None") );
					pResCase->uWidth = 64;
					pResCase->uHeight = 64;

					pResCase->bCanUpdateSurface = true;
					pResCase->bCanStretchRect = true;
					pResCase->bCanColorFill = true;
					pResCase->bCanRender = false;
                    pResCase->bRequiredShareable = false;

					// Add the case to the list
					m_listCases[TYPE_OFFSCREENPLAIN].push_back(pResCase);
				}
			}
		}
	}
}

void CEnumResource::GenerateTextureCases()
{
	HRESULT hr;
	RESOURCE_CASE *pResCase = NULL;

	// Make sure we want to test the textures
	if ( (m_dwRTYPEToTest == 0xffffffff) || (m_dwRTYPEToTest == TYPE_TEXTURE) )
	{
		// Create the Texture cases
		for (int iPool = 0; iPool<cPool; iPool++)
		{
			if (PoolNotInCurrentDX(iPool))
				continue;

			// Make sure we want to test this pool
			if ( (m_dwPoolToTest != 0xffffffff) && (g_pools[iPool].dwPool != m_dwPoolToTest) )
				continue;

			for (int iUsage = 0; iUsage<cUsage; iUsage++)
			{
				if (UsageNotInCurrentDX(iUsage))
					continue;

				// Make sure we want to test this usage
				if ( (m_dwUsageToTest != 0xffffffff) && !(g_usages[iUsage].dwUsage & m_dwUsageToTest) )
					continue;

				// RenderTarget and DepthStencil only allowed on pool Default
				if (	(g_usages[iUsage].dwUsage & (USAGE_RENDERTARGET | USAGE_DEPTHSTENCIL) ) && 
						(g_pools[iPool].dwPool != POOL_DEFAULT) )
					continue;

				// Managed textures cannot be dynamic
				if (	(g_usages[iUsage].dwUsage & USAGE_DYNAMIC) && 
						(g_pools[iPool].dwPool == POOL_MANAGED) )
					continue;

				// AutogenMipmap not allowed on SystemMem textures
				if (	(g_usages[iUsage].dwUsage & USAGE_AUTOGENMIPMAP) && 
						(g_pools[iPool].dwPool == POOL_SYSTEMMEM) )
					continue;

				// Scratch textures can't have any usage flags
				if (	(g_usages[iUsage].dwUsage != 0) && 
						(g_pools[iPool].dwPool == POOL_SCRATCH) )
					continue;

				for (int iFormat = 0; iFormat<cFormat; iFormat++)
				{
					if (FormatNotInCurrentDX(iFormat))
						continue;

					// Check if we only want to test 1 format (from command line)
					if ( (m_pD3DTest->m_Options.ModeOptions.TextureFmt != FMT_ANY) && (m_pD3DTest->m_Options.ModeOptions.TextureFmt != g_formats[iFormat].format) )
						continue;

					// Multiple element textures cannot be mip-maps
					if ( (g_formats[iFormat].format == FMT_MULTI2_ARGB8) && (g_usages[iUsage].dwUsage & USAGE_AUTOGENMIPMAP) )
						continue;

					//Scratch doesn't support these :
					//Non-lockable Z formats not supported
					//IndexBuffer Formats not supported
					//Unknown and Force_DWORD should fail
					if (g_pools[iPool].dwPool != POOL_DEFAULT )
					{
						if( (g_formats[iFormat].format == FMT_UNKNOWN) ||
                            (g_formats[iFormat].format == FMT_FORCE_DWORD) ||
                            (g_formats[iFormat].format == FMT_D16) ||
                            (g_formats[iFormat].format == FMT_D15S1) ||
                            (g_formats[iFormat].format == FMT_D24X8) ||
                            (g_formats[iFormat].format == FMT_D24S8) ||
                            (g_formats[iFormat].format == FMT_D24FS8) ||
                            (g_formats[iFormat].format == FMT_D24X4S4) ||
                            (g_formats[iFormat].format == FMT_D32) ||
                            (g_formats[iFormat].format == FMT_VERTEXDATA) ||
                            (g_formats[iFormat].format == FMT_INDEX16) ||
                            (g_formats[iFormat].format == FMT_INDEX32) )
						{
							continue;
						}
					}

					hr = m_pD3D->CheckDeviceFormat(m_pDevice->GetAdapterID(), m_dwDeviceType, m_formatAdapter, g_usages[iUsage].dwUsage, RTYPE_TEXTURE, g_formats[iFormat].format);
					if( FAILED(hr) || D3DOK_NOAUTOGEN == hr)
						continue;

					pResCase = new RESOURCE_CASE;
					ZeroMemory(pResCase, sizeof(RESOURCE_CASE));

					pResCase->dwRType = TYPE_TEXTURE;
					pResCase->format = g_formats[iFormat].format;
					_tcscpy(pResCase->szFormat, g_formats[iFormat].szFormat);
					pResCase->dwPool = g_pools[iPool].dwPool;
					_tcscpy(pResCase->szPool, g_pools[iPool].szPool);
					pResCase->dwUsage = g_usages[iUsage].dwUsage;
					_tcscpy(pResCase->szUsage, g_usages[iUsage].szUsage);
					pResCase->uLevels = 1;
					pResCase->uWidth = 64;
					pResCase->uHeight = 64;
					pResCase->uDepth = 64; // Used in IV cases for volume texture

					pResCase->bLockable = (pResCase->dwPool != POOL_DEFAULT) || (pResCase->dwUsage & USAGE_DYNAMIC);
					pResCase->bCanUpdateSurface = !(pResCase->dwUsage & USAGE_DEPTHSTENCIL);
					pResCase->bCanStretchRect = pResCase->dwUsage & USAGE_RENDERTARGET;
					pResCase->bCanColorFill = pResCase->dwUsage & USAGE_RENDERTARGET;
					pResCase->bCanRender = pResCase->dwUsage & USAGE_RENDERTARGET;
                    pResCase->bRequiredShareable = true;

					// Add the case to the list
					m_listCases[TYPE_TEXTURE].push_back(pResCase);
				}
			}
		}
	}
}

void CEnumResource::GenerateVolumeTextureCases()
{
	HRESULT hr;
	RESOURCE_CASE *pResCase = NULL;

	// Make sure we want to test the volume textures
	if( (m_dwRTYPEToTest == 0xffffffff) || (m_dwRTYPEToTest == TYPE_VOLUMETEXTURE) )
	{
		// Create the Texture cases
		for(int iPool = 0; iPool<cPool; iPool++)
		{
			if( PoolNotInCurrentDX(iPool) )
				continue;

			// Make sure we want to test this pool
			if( (m_dwPoolToTest != 0xffffffff) && (g_pools[iPool].dwPool != m_dwPoolToTest) )
				continue;

			for(int iUsage = 0; iUsage < cUsage; iUsage++)
			{
				if( UsageNotInCurrentDX(iUsage) )
					continue;

				// Make sure we want to test this usage
				if( (m_dwUsageToTest != 0xffffffff) && !(g_usages[iUsage].dwUsage & m_dwUsageToTest) )
					continue;

				// Only a handfull of usages allowed for volume textures
				if(	(g_usages[iUsage].dwUsage & (	USAGE_RENDERTARGET | 
														USAGE_DEPTHSTENCIL | 
														USAGE_AUTOGENMIPMAP |
														USAGE_DMAP) ) )
					continue;

				// Managed textures cannot be dynamic
				if(	(g_usages[iUsage].dwUsage & USAGE_DYNAMIC) && 
						(g_pools[iPool].dwPool == POOL_MANAGED) )
					continue;

				// Scratch textures can't have any usage flags
				if(	(g_usages[iUsage].dwUsage != 0) && 
						(g_pools[iPool].dwPool == POOL_SCRATCH) )
					continue;

				for(int iFormat = 0; iFormat<cFormat; iFormat++)
				{
					if( FormatNotInCurrentDX(iFormat) )
						continue;

					// Check if we only want to test 1 format (from command line)
					if( (m_pD3DTest->m_Options.ModeOptions.TextureFmt != FMT_ANY) && (m_pD3DTest->m_Options.ModeOptions.TextureFmt != g_formats[iFormat].format) )
						continue;

					// Multiple element textures cannot be mip-maps
					if( (g_formats[iFormat].format == FMT_MULTI2_ARGB8) && (g_usages[iUsage].dwUsage & USAGE_AUTOGENMIPMAP) )
						continue;

					//Scratch doesn't support these :
					//Non-lockable Z formats not supported
					//IndexBuffer Formats not supported
					//Unknown and Force_DWORD should fail
					if( POOL_SCRATCH == g_pools[iPool].dwPool )
					{
						if (	(g_formats[iFormat].format == FMT_UNKNOWN) ||
								(g_formats[iFormat].format == FMT_FORCE_DWORD)    ||
								(g_formats[iFormat].format == FMT_D16)    ||
								(g_formats[iFormat].format == FMT_D15S1)    ||
								(g_formats[iFormat].format == FMT_D24X8)    ||
								(g_formats[iFormat].format == FMT_D24S8)    ||
								(g_formats[iFormat].format == FMT_D24FS8)    ||
								(g_formats[iFormat].format == FMT_D24X4S4)  ||
								(g_formats[iFormat].format == FMT_D32)  ||
								(g_formats[iFormat].format == FMT_VERTEXDATA)  ||
								(g_formats[iFormat].format == FMT_INDEX16)  ||
								(g_formats[iFormat].format == FMT_INDEX32) )
						{
							continue;
						}
					}

					hr = m_pD3D->CheckDeviceFormat(m_pDevice->GetAdapterID(), m_dwDeviceType, m_formatAdapter, g_usages[iUsage].dwUsage, RTYPE_TEXTURE, g_formats[iFormat].format);
					if( FAILED(hr) || D3DOK_NOAUTOGEN == hr)
						continue;

					pResCase = new RESOURCE_CASE;
					ZeroMemory(pResCase, sizeof(RESOURCE_CASE));

					pResCase->dwRType = TYPE_VOLUMETEXTURE;
					pResCase->format = g_formats[iFormat].format;
					_tcscpy(pResCase->szFormat, g_formats[iFormat].szFormat);
					pResCase->dwPool = g_pools[iPool].dwPool;
					_tcscpy(pResCase->szPool, g_pools[iPool].szPool);
					pResCase->dwUsage = g_usages[iUsage].dwUsage;
					_tcscpy(pResCase->szUsage, g_usages[iUsage].szUsage);
					pResCase->uLevels = 1;
					pResCase->uWidth = 64;
					pResCase->uHeight = 64;
					pResCase->uDepth = 64;

					pResCase->bLockable = (pResCase->dwPool != POOL_DEFAULT) || (pResCase->dwUsage & USAGE_DYNAMIC);
					pResCase->bCanUpdateSurface = false;
					pResCase->bCanStretchRect = false;
					pResCase->bCanColorFill = false;
					pResCase->bCanRender = false;
                    pResCase->bRequiredShareable = false;

					// Add the case to the list
					m_listCases[TYPE_VOLUMETEXTURE].push_back(pResCase);
				}
			}
		}
	}
}

void CEnumResource::GenerateCubeTextureCases()
{
    HRESULT hr;
	RESOURCE_CASE *pResCase = NULL;

    // Make sure we want to test the cube textures
    if( (0xffffffff != m_dwRTYPEToTest) && (TYPE_CUBETEXTURE != m_dwRTYPEToTest) )
        return;

    // Create the CubeTexture cases
    for( int iPool = 0; iPool < cPool; iPool++ )
    {
	    if( PoolNotInCurrentDX(iPool) )
		    continue;

	    // Make sure we want to test this pool
	    if( (0xffffffff != m_dwPoolToTest) && (g_pools[iPool].dwPool != m_dwPoolToTest) )
		    continue;

	    for( int iUsage = 0; iUsage < cUsage; iUsage++ )
        {
            if( UsageNotInCurrentDX(iUsage) )
	            continue;

            // Make sure we want to test this usage
            if( (0xffffffff != m_dwUsageToTest) && !(g_usages[iUsage].dwUsage & m_dwUsageToTest) )
	            continue;

            if( g_usages[iUsage].dwUsage != USAGE_AUTOGENMIPMAP &&
                g_usages[iUsage].dwUsage != USAGE_DEPTHSTENCIL &&
                g_usages[iUsage].dwUsage != USAGE_DYNAMIC &&
                g_usages[iUsage].dwUsage != USAGE_RENDERTARGET )
                continue;

            // RenderTarget and DepthStencil only allowed on pool Default
            if( (g_usages[iUsage].dwUsage & USAGE_RENDERTARGET) || (g_usages[iUsage].dwUsage & USAGE_DEPTHSTENCIL) )
                continue;

            // AutogenMipmap not allowed on SystemMem textures
            if( (g_usages[iUsage].dwUsage & USAGE_AUTOGENMIPMAP) && (POOL_SYSTEMMEM == g_pools[iPool].dwPool) )
                continue;
            			
            // Managed Cube textures cannot be dynamic
            if( (g_usages[iUsage].dwUsage & USAGE_DYNAMIC) && (POOL_MANAGED == g_pools[iPool].dwPool) )
                continue;
        
            // AutogenMipmap not allowed on SystemMem textures
            if ( (g_usages[iUsage].dwUsage & USAGE_AUTOGENMIPMAP) && (POOL_SYSTEMMEM == g_pools[iPool].dwPool) )
                continue;

            // Scratch textures can't have any usage flags
            if( ( 0 != g_usages[iUsage].dwUsage) && (g_pools[iPool].dwPool == POOL_SCRATCH) )
                continue;                

            for( int iFormat = 0; iFormat < cFormat; iFormat++ )
            {
	            if( FormatNotInCurrentDX(iFormat) )
		            continue;

	            // Check if we only want to test 1 format (from command line)
	            if( (FMT_ANY != m_pD3DTest->m_Options.ModeOptions.TextureFmt) && (m_pD3DTest->m_Options.ModeOptions.TextureFmt != g_formats[iFormat].format) )
		            continue;

                if( (g_formats[iFormat].format == FMT_UNKNOWN) ||
                    (g_formats[iFormat].format == FMT_FORCE_DWORD) ||
                    (g_formats[iFormat].format == FMT_D16) ||
                    (g_formats[iFormat].format == FMT_D15S1) ||
                    (g_formats[iFormat].format == FMT_D24X8) ||
                    (g_formats[iFormat].format == FMT_D24S8) ||
                    (g_formats[iFormat].format == FMT_D24X4S4) ||
                    (g_formats[iFormat].format == FMT_D32) ||
                    (g_formats[iFormat].format == FMT_VERTEXDATA) ||
                    (g_formats[iFormat].format == FMT_INDEX16) ||
                    (g_formats[iFormat].format == FMT_INDEX32) )
                continue;

                // Multiple element textures cannot be mip-maps
			    if ( (g_formats[iFormat].format == FMT_MULTI2_ARGB8) && (g_usages[iUsage].dwUsage & USAGE_AUTOGENMIPMAP) )
				    continue;

                hr = m_pD3D->CheckDeviceFormat( m_pDevice->GetAdapterID(), m_dwDeviceType, m_formatAdapter, g_usages[iUsage].dwUsage, RTYPE_CUBETEXTURE, g_formats[iFormat].format );
                if( FAILED(hr) || D3DOK_NOAUTOGEN == hr)
                    continue;

                pResCase = new RESOURCE_CASE;
		        ZeroMemory(pResCase, sizeof(RESOURCE_CASE));

                pResCase->dwRType = TYPE_CUBETEXTURE;
                pResCase->format = g_formats[iFormat].format;
                _tcscpy(pResCase->szFormat, g_formats[iFormat].szFormat);
                pResCase->dwPool = g_pools[iPool].dwPool;
                _tcscpy(pResCase->szPool, g_pools[iPool].szPool);
                pResCase->dwUsage = g_usages[iUsage].dwUsage;
                _tcscpy(pResCase->szUsage, g_usages[iUsage].szUsage);
                pResCase->uLevels = 1;
                pResCase->uWidth = 128;
                pResCase->uHeight = 128;
                pResCase->bLockable = false;
                pResCase->bCanUpdateSurface = false;
                pResCase->bCanStretchRect = false;
                pResCase->bCanColorFill = false;
                pResCase->bCanRender = false;
                pResCase->bRequiredShareable = false;

                m_listCases[TYPE_CUBETEXTURE].push_back(pResCase);
            }
        }
    }
}

void CEnumResource::GenerateVertexBufferCases()
{
	HRESULT hr;
	RESOURCE_CASE *pResCase = NULL;

	// Make sure we want to test the vertex buffers
	if( (m_dwRTYPEToTest == 0xffffffff) || (m_dwRTYPEToTest == TYPE_VERTEXBUFFER) )
	{
		// Create the Vertex Buffer cases
		for( int iPool = 0; iPool < cPool; iPool++ )
		{
			if( PoolNotInCurrentDX(iPool) )
				continue;

			// Make sure we want to test this pool
			if( (m_dwPoolToTest != 0xffffffff) && (g_pools[iPool].dwPool != m_dwPoolToTest) )
				continue;

			// Scratch not allowed for VB
			if( g_pools[iPool].dwPool == POOL_SCRATCH )
				continue;

			for (int iUsage = 0; iUsage<cUsageVB; iUsage++)
			{
				if( UsageNotInCurrentDX(iUsage) )
					continue;

				// Make sure we want to test this usage
				if( (m_dwUsageToTest != 0xffffffff) && !(g_usagesVB[iUsage].dwUsage & m_dwUsageToTest))
					continue;

				// Only test Dynamic, SoftwareProcessing, WriteOnly and no usage
				if( (g_usagesVB[iUsage].dwUsage & ~(USAGE_DYNAMIC | USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY) ) && 
                    (g_usagesVB[iUsage].dwUsage != 0) )
					continue;

				// Managed VB cannot be dynamic
				if( (g_usagesVB[iUsage].dwUsage & USAGE_DYNAMIC) && (g_pools[iPool].dwPool == POOL_MANAGED) )
					continue;

				pResCase = new RESOURCE_CASE;
				ZeroMemory(pResCase, sizeof(RESOURCE_CASE));

				pResCase->dwRType = TYPE_VERTEXBUFFER;
				pResCase->dwPool = g_pools[iPool].dwPool;
				_tcscpy(pResCase->szPool, g_pools[iPool].szPool);
				pResCase->dwUsage = g_usagesVB[iUsage].dwUsage;
				_tcscpy(pResCase->szUsage, g_usagesVB[iUsage].szUsage);
				pResCase->dwFVF = D3DFVF_XYZRHW|D3DFVF_DIFFUSE;
				_tcscpy(pResCase->szFVF, _T("D3DFVF_XYZRHW | D3DFVF_DIFFUSE"));
				pResCase->uWidth = 4 * sizeof(XYZRHWDIFFUSEVERTEX);
                pResCase->bRequiredShareable = false;
                pResCase->bLockable = true;

				m_listCases[TYPE_VERTEXBUFFER].push_back(pResCase);
			}
		}
	}
}

void CEnumResource::GenerateIndexBufferCases()
{
	HRESULT hr;
	RESOURCE_CASE *pResCase = NULL;

	// Make sure we want to test the vertex buffers
	if( (m_dwRTYPEToTest == 0xffffffff) || (m_dwRTYPEToTest == TYPE_INDEXBUFFER) )
	{
		// Create the Index Buffer cases
		for( int iPool = 0; iPool < cPool; iPool++ )
		{
			if( PoolNotInCurrentDX(iPool) )
				continue;

			// Make sure we want to test this pool
			if( (m_dwPoolToTest != 0xffffffff) && (g_pools[iPool].dwPool != m_dwPoolToTest) )
				continue;

			// Scratch not allowed for IB
			if( g_pools[iPool].dwPool == POOL_SCRATCH )
				continue;

			for( int iUsage = 0; iUsage<cUsageVB; iUsage++ )
			{
				if( UsageNotInCurrentDX(iUsage) )
					continue;

				// Make sure we want to test this usage
				if( (m_dwUsageToTest != 0xffffffff) && !(g_usagesVB[iUsage].dwUsage & m_dwUsageToTest) )
					continue;

				// Only test Dynamic, SoftwareProcessing, WriteOnly and no usage
				if( (g_usagesVB[iUsage].dwUsage & ~(USAGE_DYNAMIC | USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY) ) &&
                    (g_usagesVB[iUsage].dwUsage != 0) )
					continue;

				// Managed IB cannot be dynamic
				if( (g_usagesVB[iUsage].dwUsage & USAGE_DYNAMIC) && 
						(g_pools[iPool].dwPool == POOL_MANAGED) )
					continue;

				pResCase = new RESOURCE_CASE;
				ZeroMemory(pResCase, sizeof(RESOURCE_CASE));
                
                pResCase->bLockable = true;
				pResCase->dwRType = TYPE_INDEXBUFFER;
				pResCase->dwPool = g_pools[iPool].dwPool;
				_tcscpy(pResCase->szPool, g_pools[iPool].szPool);
				pResCase->dwUsage = g_usagesVB[iUsage].dwUsage;
				_tcscpy(pResCase->szUsage, g_usagesVB[iUsage].szUsage);
				pResCase->format = FMT_INDEX16;
				_tcscpy(pResCase->szFormat, _T("FMT_INDEX16"));
				pResCase->uWidth = 6;
                pResCase->bRequiredShareable = false;
				m_listCases[TYPE_INDEXBUFFER].push_back(pResCase);
			}
		}
	}
}

void CEnumResource::GenerateAllCases()
{
    GenerateTextureCases();
    GenerateVertexBufferCases();
	GenerateOffscreenPlainCases();
	GenerateRenderTargetCases();
	GenerateDepthStencilCases();
	GenerateCubeTextureCases();
	GenerateVolumeTextureCases();
	GenerateIndexBufferCases();
}

void CEnumResource::GetTypeName(DWORD dwRType, TCHAR* szRType)
{
    _tcscpy(szRType, TypeName[dwRType]);
}

void CEnumResource::DumpCaseDetails(DWORD dwRTYPE, RESOURCE_CASE *pResCase, TCHAR* szDesc)
{
	_stprintf(szDesc, _T("%s  Type      : %s\n"), szDesc, TypeName[dwRTYPE]);

	switch (dwRTYPE)
	{
		case TYPE_OFFSCREENPLAIN :
			_stprintf(szDesc, _T("%s  Format    : %s\n"), szDesc, pResCase->szFormat);
			_stprintf(szDesc, _T("%s  Pool      : %s\n"), szDesc, pResCase->szPool);
			_stprintf(szDesc, _T("%s  Width     : %d\n"), szDesc, pResCase->uWidth);
			_stprintf(szDesc, _T("%s  Height    : %d\n"), szDesc, pResCase->uHeight);
			break;

		case TYPE_RENDERTARGET :
			_stprintf(szDesc, _T("%s  Format    : %s\n"), szDesc, pResCase->szFormat);
			_stprintf(szDesc, _T("%s  Pool      : %s\n"), szDesc, pResCase->szPool);
			_stprintf(szDesc, _T("%s  Width     : %d\n"), szDesc, pResCase->uWidth);
			_stprintf(szDesc, _T("%s  Height    : %d\n"), szDesc, pResCase->uHeight);
			_stprintf(szDesc, _T("%s  MultiSamp : %d\n"), szDesc, pResCase->multiSample);
			_stprintf(szDesc, _T("%s  Lockable  : %s\n"), szDesc, pResCase->bLockable ? _T("True") : _T("False"));
			break;

		case TYPE_DEPTHSTENCIL :
			_stprintf(szDesc, _T("%s  Format    : %s\n"), szDesc, pResCase->szFormat);
			_stprintf(szDesc, _T("%s  Pool      : %s\n"), szDesc, pResCase->szPool);
			_stprintf(szDesc, _T("%s  Width     : %d\n"), szDesc, pResCase->uWidth);
			_stprintf(szDesc, _T("%s  Height    : %d\n"), szDesc, pResCase->uHeight);
			_stprintf(szDesc, _T("%s  MultiSamp : %d\n"), szDesc, pResCase->multiSample);
			_stprintf(szDesc, _T("%s  Discard   : %s\n"), szDesc, pResCase->bDiscardable ? _T("True") : _T("False"));
			break;

		case TYPE_TEXTURE :
			_stprintf(szDesc, _T("%s  Format    : %s\n"), szDesc, pResCase->szFormat);
			_stprintf(szDesc, _T("%s  Pool      : %s\n"), szDesc, pResCase->szPool);
			_stprintf(szDesc, _T("%s  Width     : %d\n"), szDesc, pResCase->uWidth);
			_stprintf(szDesc, _T("%s  Height    : %d\n"), szDesc, pResCase->uHeight);
			_stprintf(szDesc, _T("%s  Levels    : %d\n"), szDesc, pResCase->uLevels);
			_stprintf(szDesc, _T("%s  Usage     : %s\n"), szDesc, pResCase->szUsage);
			break;

		case TYPE_CUBETEXTURE :
			_stprintf(szDesc, _T("%s  Format    : %s\n"), szDesc, pResCase->szFormat);
			_stprintf(szDesc, _T("%s  Pool      : %s\n"), szDesc, pResCase->szPool);
			_stprintf(szDesc, _T("%s  Edge      : %d\n"), szDesc, pResCase->uWidth);
			_stprintf(szDesc, _T("%s  Levels    : %d\n"), szDesc, pResCase->uLevels);
			_stprintf(szDesc, _T("%s  Usage     : %s\n"), szDesc, pResCase->szUsage);
			break;


		case TYPE_VOLUMETEXTURE :
			_stprintf(szDesc, _T("%s  Format    : %s\n"), szDesc, pResCase->szFormat);
			_stprintf(szDesc, _T("%s  Pool      : %s\n"), szDesc, pResCase->szPool);
			_stprintf(szDesc, _T("%s  Width     : %d\n"), szDesc, pResCase->uWidth);
			_stprintf(szDesc, _T("%s  Height    : %d\n"), szDesc, pResCase->uHeight);
			_stprintf(szDesc, _T("%s  Depth     : %d\n"), szDesc, pResCase->uDepth);
			_stprintf(szDesc, _T("%s  Levels    : %d\n"), szDesc, pResCase->uLevels);
			_stprintf(szDesc, _T("%s  Usage     : %s\n"), szDesc, pResCase->szUsage);
			break;

		case TYPE_VERTEXBUFFER :
			_stprintf(szDesc, _T("%s  Pool      : %s\n"), szDesc, pResCase->szPool);
			_stprintf(szDesc, _T("%s  Length    : %d\n"), szDesc, pResCase->uWidth);
			_stprintf(szDesc, _T("%s  Usage     : %s\n"), szDesc, pResCase->szUsage);
			_stprintf(szDesc, _T("%s  FVF       : %s\n"), szDesc, pResCase->szFVF);
			break;

		case TYPE_INDEXBUFFER :
			_stprintf(szDesc, _T("%s  Format    : %s\n"), szDesc, pResCase->szFormat);
			_stprintf(szDesc, _T("%s  Pool      : %s\n"), szDesc, pResCase->szPool);
			_stprintf(szDesc, _T("%s  Length    : %d\n"), szDesc, pResCase->uWidth);
			_stprintf(szDesc, _T("%s  Usage     : %s\n"), szDesc, pResCase->szUsage);
			break;
	}	
}

void CEnumResource::MakeCaseName(DWORD dwRTYPE, RESOURCE_CASE *pResCase, TCHAR* szName)
{
    _stprintf(szName, _T("%s - %s - "), szName, TypeName[dwRTYPE]);
    

	switch (dwRTYPE)
	{
		case TYPE_OFFSCREENPLAIN :
			_stprintf(szName, _T("%s %s"), szName, pResCase->szFormat);
			break;

		case TYPE_RENDERTARGET :
			_stprintf(szName, _T("%s %s"), szName, pResCase->szFormat);
			break;

		case TYPE_DEPTHSTENCIL :
			_stprintf(szName, _T("%s %s"), szName, pResCase->szFormat);
			break;

		case TYPE_TEXTURE :
			_stprintf(szName, _T("%s %s"), szName, pResCase->szFormat);
            _stprintf(szName, _T("%s - %s"), szName, pResCase->szUsage);
			break;

		case TYPE_CUBETEXTURE :
			_stprintf(szName, _T("%s %s"), szName, pResCase->szFormat);
			break;

		case TYPE_VOLUMETEXTURE :
			_stprintf(szName, _T("%s %s"), szName, pResCase->szFormat);
			break;

		case TYPE_VERTEXBUFFER :
			_stprintf(szName, _T("%s %s"), szName, pResCase->szUsage);
			break;

		case TYPE_INDEXBUFFER :
			_stprintf(szName, _T("%s %s"), szName, pResCase->szFormat);
			break;
    }
}

bool CEnumResource::IsFormatDXTn(FMT format)
{
    return ( format == FMT_DXT1 || 
             format == FMT_DXT2 || 
             format == FMT_DXT3 || 
             format == FMT_DXT4 || 
             format == FMT_DXT5
            );
}

bool CEnumResource::IsFormatFourCC(FMT format)
{
	return( format == FMT_UYVY || 
            format == FMT_YUY2 || 
            format == FMT_MULTI2_ARGB8 || 
            format == FMT_DXT1 || 
            format == FMT_DXT2 || 
            format == FMT_DXT3 || 
            format == FMT_DXT4 || 
            format == FMT_DXT5
          );
}

RESOURCE_CASE* CEnumResource::GetNextCase(DWORD dwRTYPE)
{
	if( 0 == m_listCases[dwRTYPE].size() )
		return NULL;

    if( m_iCurrentCase[dwRTYPE] + 1 > m_listCases[dwRTYPE].size() )
		return NULL;

	m_iCurrentCase[dwRTYPE]++;
	return m_listCases[dwRTYPE][ m_iCurrentCase[dwRTYPE] ];
}

RESOURCE_CASE* CEnumResource::GetNextCase(DWORD dwRTYPE, DWORD dwPoolRequested)
{
	for( int i = m_iCurrentCase[dwRTYPE] + 1; i < m_listCases[dwRTYPE].size(); i++ )
	{
		if( m_listCases[dwRTYPE][i]->dwPool == dwPoolRequested )
		{
			m_iCurrentCase[dwRTYPE] = i;
			return m_listCases[dwRTYPE][i];
			break;
		}
	}

	return NULL;
}

RESOURCE_CASE* CEnumResource::GetNextCase(DWORD dwRTYPE, DWORD dwPoolRequested, UINT &uRequestedCase)
{
    // loop on case
    if( m_iCurrentCase[dwRTYPE] + 1 >= m_listCases[dwRTYPE].size() )
        ResetCurrentCase(dwRTYPE);

    for( int i = m_iCurrentCase[dwRTYPE] + 1; i < m_listCases[dwRTYPE].size(); i++)
    {
		if ( m_listCases[dwRTYPE][i]->dwPool == dwPoolRequested )
		{
			m_iCurrentCase[dwRTYPE] = i;
			uRequestedCase = i;
			return m_listCases[dwRTYPE][i];
			break;
		}
	}

	return NULL;
}

RESOURCE_CASE* CEnumResource::GetCurrentCase(DWORD dwRTYPE)
{
    // loop on case
    if( m_iCurrentCase[dwRTYPE] >= m_listCases[dwRTYPE].size() )
        return NULL;

	return m_listCases[dwRTYPE][m_iCurrentCase[dwRTYPE]];
}

HRESULT CEnumResource::CreateResource(DWORD dwRTYPE, RESOURCE_CASE *pResCase, CObject **ppResource, HANDLE *pSharedHandle)
{
	HRESULT hr;

	switch( dwRTYPE )
	{
		case TYPE_OFFSCREENPLAIN :        
            hr = m_pDevice->CreateOffscreenPlainSurfaceEx(  pResCase->uWidth,
                                                            pResCase->uHeight,
                                                            pResCase->format, 
                                                            pResCase->dwPool,
                                                            (LPSURFACE*)ppResource,
                                                            pSharedHandle,
                                                            pResCase->dwUsage);
			break;

		case TYPE_RENDERTARGET :
            if( USAGE_RENDERTARGET == pResCase->dwUsage )
            {
                 hr = m_pDevice->CreateRenderTarget(  pResCase->uWidth,
				    							        pResCase->uHeight,
					    						        pResCase->format, 
						    					        pResCase->multiSample,
							    				        0,
								    			        pResCase->bLockable,
									    		        (LPSURFACE*)ppResource,
										    	        pSharedHandle);
            }
            else
            {
                hr = m_pDevice->CreateRenderTargetEx(  pResCase->uWidth,
				    							        pResCase->uHeight,
					    						        pResCase->format, 
						    					        pResCase->multiSample,
							    				        0,
								    			        pResCase->bLockable,
									    		        (LPSURFACE*)ppResource,
										    	        pSharedHandle,
                                                        pResCase->dwUsage);
            }
			break;

		case TYPE_DEPTHSTENCIL :
			hr = m_pDevice->CreateDepthStencilSurface(	pResCase->uWidth,
														pResCase->uHeight,
														pResCase->format, 
														pResCase->multiSample,
														0,
														pResCase->bDiscardable,
														(LPSURFACE*)ppResource,
														pSharedHandle);
			break;

		case TYPE_TEXTURE :
			hr = m_pDevice->CreateTexture(	pResCase->uWidth,
											pResCase->uHeight,
											pResCase->uLevels,
											pResCase->dwUsage,
											pResCase->format, 
											pResCase->dwPool,
											(LPTEXTURE*)ppResource,
											pSharedHandle);
			break;

		case TYPE_CUBETEXTURE :
			hr = m_pDevice->CreateCubeTexture(	pResCase->uWidth,
												pResCase->uLevels,
												pResCase->dwUsage,
												pResCase->format, 
												pResCase->dwPool,
												(LPCUBETEXTURE*)ppResource,
												pSharedHandle);
			break;

		case TYPE_VOLUMETEXTURE :
			hr = m_pDevice->CreateVolumeTexture(	pResCase->uWidth,
													pResCase->uHeight,
													pResCase->uDepth,
													pResCase->uLevels,
													pResCase->dwUsage,
													pResCase->format, 
													pResCase->dwPool,
													(LPVOLUMETEXTURE*)ppResource,
													pSharedHandle);
			break;

		case TYPE_VERTEXBUFFER :
			hr = m_pDevice->CreateVertexBuffer(		pResCase->uWidth,
													pResCase->dwUsage,
													pResCase->dwFVF, 
													pResCase->dwPool,
													(LPVERTEXBUFFER*)ppResource,
													pSharedHandle);
			break;

		case TYPE_INDEXBUFFER :
			hr = m_pDevice->CreateIndexBuffer(		pResCase->uWidth,
													pResCase->dwUsage,
													pResCase->format, 
													pResCase->dwPool,
													(LPINDEXBUFFER*)ppResource,
													pSharedHandle);
			break;

		default :
			hr = E_FAIL;
			break;
	}

	return hr;
}

FMT CEnumResource::GetAnotherValidFormatForUsage(DWORD dwRTYPE, DWORD dwUsage, FMT format)
{
	for( int i = 0; i < m_listCases[dwRTYPE].size(); i++)
	{
		if( dwUsage != m_listCases[dwRTYPE][i]->dwUsage )
			continue;

		if( m_listCases[dwRTYPE][i]->format != format )
		{
			return m_listCases[dwRTYPE][i]->format;
		}
	}

	return format;
}

DWORD CEnumResource::GetAnotherValidUsageForFormat(DWORD dwRTYPE, FMT format, DWORD dwUsage, bool bAllowZero)
{
	for( int i = 0; i < m_listCases[dwRTYPE].size(); i++ )
	{
		if( format != m_listCases[dwRTYPE][i]->format )
			continue;

		if( m_listCases[dwRTYPE][i]->dwUsage != dwUsage )
		{
			if( false == bAllowZero && 0 == m_listCases[dwRTYPE][i]->dwUsage )
				continue;

			return m_listCases[dwRTYPE][i]->dwUsage;
		}
	}

	return dwUsage;
}

DWORD CEnumResource::GetAnotherValidPool(DWORD dwRTYPE, DWORD dwPool, FMT format, DWORD dwUsage)
{
	for( int i = 0; i < m_listCases[dwRTYPE].size(); i++ )
	{
		if( format != m_listCases[dwRTYPE][i]->format || dwUsage != m_listCases[dwRTYPE][i]->dwUsage)
			continue;

		if( m_listCases[dwRTYPE][i]->dwPool != dwPool )
		{
			return m_listCases[dwRTYPE][i]->dwPool;
		}
	}

	return dwPool;
}

TCHAR* CEnumResource::GetUsageString(DWORD dwUsage)
{
	for( int iUsage=0; iUsage < cUsage; iUsage++ )
	{
		if( dwUsage == g_usages[iUsage].dwUsage )
			return g_usages[iUsage].szUsage;
	}

	return NULL;
}

TCHAR* CEnumResource::GetFormatString(FMT format)
{
	for( int iFormat=0; iFormat < cFormat; iFormat++ )
	{
		if( format == g_formats[iFormat].format )
			return g_formats[iFormat].szFormat;
	}

	return NULL;
}

void CEnumResource::FindMatchingCaseForResType(DWORD dwSrcRTYPE, RESOURCE_CASE *pSrcResCase, DWORD dwDestRTYPE, RESOURCE_CASE *pDestResCase)
{
	memset(pDestResCase, 0, sizeof(RESOURCE_CASE));
	pDestResCase->dwRType = dwDestRTYPE;

	switch(dwSrcRTYPE)
	{
		case TYPE_OFFSCREENPLAIN :
			pDestResCase->format = pSrcResCase->format;
			pDestResCase->dwPool = pSrcResCase->dwPool;
			pDestResCase->uWidth = pSrcResCase->uWidth;
			pDestResCase->uHeight = pSrcResCase->uHeight;
			break;

		case TYPE_RENDERTARGET :
			pDestResCase->format = pSrcResCase->format;
			pDestResCase->uWidth = pSrcResCase->uWidth;
			pDestResCase->uHeight = pSrcResCase->uHeight;
			pDestResCase->bLockable = pSrcResCase->bLockable;
			break;

		case TYPE_DEPTHSTENCIL :
			pDestResCase->format = pSrcResCase->format;
			pDestResCase->uWidth = pSrcResCase->uWidth;
			pDestResCase->uHeight = pSrcResCase->uHeight;
			break;

		case TYPE_TEXTURE :
			pDestResCase->format = pSrcResCase->format;
			pDestResCase->dwPool = pSrcResCase->dwPool;
			pDestResCase->dwUsage = pSrcResCase->dwUsage;
			pDestResCase->uWidth = pSrcResCase->uWidth;
			pDestResCase->uHeight = pSrcResCase->uHeight;
			pDestResCase->uLevels = pSrcResCase->uLevels;
			break;

		case TYPE_CUBETEXTURE :
			pDestResCase->format = pSrcResCase->format;
			pDestResCase->dwPool = pSrcResCase->dwPool;
			pDestResCase->dwUsage = pSrcResCase->dwUsage;
			pDestResCase->uWidth = pSrcResCase->uWidth;
			pDestResCase->uLevels = pSrcResCase->uLevels;
			break;

		case TYPE_VOLUMETEXTURE :
			pDestResCase->format = pSrcResCase->format;
			pDestResCase->dwPool = pSrcResCase->dwPool;
			pDestResCase->dwUsage = pSrcResCase->dwUsage;
			pDestResCase->uWidth = pSrcResCase->uWidth;
			pDestResCase->uHeight = pSrcResCase->uHeight;
			pDestResCase->uDepth = pSrcResCase->uDepth;
			pDestResCase->uLevels = pSrcResCase->uLevels;
			break;

		case TYPE_VERTEXBUFFER :
			pDestResCase->dwPool = pSrcResCase->dwPool;
			pDestResCase->dwUsage = pSrcResCase->dwUsage;
			pDestResCase->uWidth = pSrcResCase->uWidth;
			pDestResCase->dwFVF = pSrcResCase->dwFVF;
			break;

		case TYPE_INDEXBUFFER :
			pDestResCase->format = pSrcResCase->format;
			pDestResCase->dwPool = pSrcResCase->dwPool;
			pDestResCase->dwUsage = pSrcResCase->dwUsage;
			pDestResCase->uWidth = pSrcResCase->uWidth;
			break;
	}
}

void CEnumResource::ResetCurrentCase(DWORD dwRTYPE)
{
	m_iCurrentCase[dwRTYPE] = -1;
}

