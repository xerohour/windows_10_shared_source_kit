#define D3D_OVERLOADS
#include <d3d9.h>
#include "types.h"
#include "GammaCorrect.h"

bool GammaCorrect::SetDefaultMatrices(void) {
    D3DXMATRIX           Matrix;
    // We need to be far from scane to receive correct specular highlights
	D3DXVECTOR3         from(0.f, 0.0f, 1.0f); 
    D3DXVECTOR3	        at(0.f, 0.0f, 0.0f);
    D3DXVECTOR3	        up(0.0f, 1.0f, 0.0f);
    float				fWidth = 100.f;
	float				fHeight = 100.f;
	float               fZn = -1.f;
	float               fZf = 1.f;
    
	D3DXMatrixOrthoLH(&Matrix, 2.f, 2.f, -1.0f, 1.0f );
	if (!SetTransform(D3DTS_PROJECTION,&Matrix))
		return false;

    D3DXMatrixIdentity( &Matrix );
    if (!SetTransform(D3DTS_WORLD,&Matrix))
        return false;

	D3DXMatrixIdentity( &Matrix );
    D3DXMatrixLookAtLH(&(Matrix), &from, &at, &up);
    if (!SetTransform(D3DTS_VIEW,&Matrix))
        return false;

    return true;
}

//-----------------------------------------------------------------------------

bool GammaCorrect::SetDefaultRenderStates(void)
 {
	if ( m_Format.d3dfFormat == FMT_A8 )
	{
		SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		SetTextureStageState((DWORD)0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);
		SetTextureStageState((DWORD)0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE);
		SetTextureStageState((DWORD)1, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);
		SetTextureStageState((DWORD)1, D3DTSS_ALPHAARG1, (DWORD)D3DTA_CURRENT);
	}
	//this is only for blending cases for dx9.L
	else if ( m_isLDDMDriver )
	{
		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR );
		SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
	}

	SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_NONE);
	
	SetTextureStageState((DWORD)0, D3DTSS_TEXCOORDINDEX, 0);
	SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
	SetTextureStageState((DWORD)0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
        
	SetTextureStageState((DWORD)1, D3DTSS_TEXCOORDINDEX, 0);
	SetTextureStageState((DWORD)1, D3DTSS_COLOROP, (DWORD)D3DTOP_ADD);
	SetTextureStageState((DWORD)1, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
	SetTextureStageState((DWORD)1, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT);

	if ( m_isRead )
	{
		m_pSrcDevice->SetSamplerState(0, SAMP_SRGBTEXTURE, (DWORD)TRUE);
		m_pSrcDevice->SetSamplerState(1, SAMP_SRGBTEXTURE, (DWORD)TRUE);
		if ( m_pRefDevice )
		{
			m_pRefDevice->SetSamplerState(0, SAMP_SRGBTEXTURE, (DWORD)TRUE);
			m_pRefDevice->SetSamplerState(1, SAMP_SRGBTEXTURE, (DWORD)TRUE);
		}
	}
	else if ( m_isSrcRead )
	{
		m_pSrcDevice->SetSamplerState(0, SAMP_SRGBTEXTURE, (DWORD)TRUE);
		m_pSrcDevice->SetSamplerState(1, SAMP_SRGBTEXTURE, (DWORD)TRUE);
	}
	else if ( m_pRefDevice && m_isRefRead )
	{
		m_pRefDevice->SetSamplerState(0, SAMP_SRGBTEXTURE, (DWORD)TRUE);
		m_pRefDevice->SetSamplerState(1, SAMP_SRGBTEXTURE, (DWORD)TRUE);
	}

	if ( m_isWrite )
	{
		m_pSrcDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, (DWORD)TRUE);
		if ( m_pRefDevice )
			m_pRefDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, (DWORD)TRUE);
	}
	else if ( m_isSrcWrite )
	{
		m_pSrcDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, (DWORD)TRUE);
	}
	else if ( m_pRefDevice && m_isRefWrite )
	{
		m_pRefDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, (DWORD)TRUE);
	}

	return true;
}

 //-----------------------------------------------------------------------------

bool GammaCorrect::CapsCheck()
{
	HRESULT hr;
	DEVICEDESC *pCaps = m_pSrcDevice->GetCaps(); 

	DWORD dwTextureCaps             = pCaps->dwTextureCaps;
	DWORD dwTextureFilterCaps       = pCaps->dwTextureFilterCaps;
	DWORD dwCubeTextureFilterCaps   = pCaps->dwCubeTextureFilterCaps;
	DWORD dwVolumeTextureFilterCaps = pCaps->dwVolumeTextureFilterCaps;

	switch (m_RType)
	{
		case RTYPE_TEXTURE:
		{
			m_Format = m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
            m_uLevels = 5;

			if ( (m_Format.ddpfPixelFormat.dwFlags & PF_FOURCC ) || KeySet(_T("nomipmap")) || !(dwTextureCaps  & D3DPTEXTURECAPS_MIPMAP) ||
				 (m_pD3D->GetRuntimeVersion() >= 9.029f && FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), 
																  m_pCurrentMode->pDisplay->Format, USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, m_Format)) ))
				m_uLevels = 1;
			else if (dwTextureFilterCaps  & D3DPTFILTERCAPS_MIPFPOINT)
			{
				SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_POINT);
				SetSamplerState(1, SAMP_MIPFILTER, (DWORD)TEXF_POINT);
			}
			else if (dwTextureFilterCaps  & D3DPTFILTERCAPS_MIPFLINEAR)
			{
				SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_LINEAR);		
				SetSamplerState(1, SAMP_MIPFILTER, (DWORD)TEXF_LINEAR);	
			}
			else
			{
				m_uLevels = 1;
				WriteToLog("CapsCheck: Mipmaping is set without corresponding filter caps\n");
			}
			break;
		}
		case RTYPE_CUBETEXTURE:
		{
			if (!(dwTextureCaps  & D3DPTEXTURECAPS_CUBEMAP)) 
			{
				WriteToLog("CapsCheck: cube maps are not supported by the driver.\n");
				return false;
			}
			m_Format = m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat];
			m_uLevels = 5;

			if ( (m_Format.ddpfPixelFormat.dwFlags & PF_FOURCC) || KeySet(_T("nomipmap")) || !(dwTextureCaps  & D3DPTEXTURECAPS_MIPCUBEMAP) ||
				 (m_pD3D->GetRuntimeVersion() >= 9.029f && FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), 
																  m_pCurrentMode->pDisplay->Format, USAGE_QUERY_WRAPANDMIP, RTYPE_CUBETEXTURE, m_Format)) ))
				m_uLevels = 1;
			else if (dwCubeTextureFilterCaps  & D3DPTFILTERCAPS_MIPFPOINT)
			{
				SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_POINT);
				SetSamplerState(1, SAMP_MIPFILTER, (DWORD)TEXF_POINT);
			}
			else if (dwCubeTextureFilterCaps  & D3DPTFILTERCAPS_MIPFLINEAR)
			{
				SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_LINEAR);		
				SetSamplerState(1, SAMP_MIPFILTER, (DWORD)TEXF_LINEAR);
			}
			else
			{
				m_uLevels = 1;
				WriteToLog("CapsCheck: Cube mipmaping is set without corresponding filter caps\n");
			}
			break;
		}
		case RTYPE_VOLUMETEXTURE:
		{
			if (!(dwTextureCaps  & D3DPTEXTURECAPS_VOLUMEMAP)) 
			{
				WriteToLog("CapsCheck: volumes are not supported by the driver.\n");
				return false;
			}
			m_Format = m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat];
			m_uLevels = 5;
			if ( (m_Format.ddpfPixelFormat.dwFlags & PF_FOURCC) || KeySet(_T("nomipmap")) || !(dwTextureCaps  & D3DPTEXTURECAPS_MIPVOLUMEMAP) ||
				 (m_pD3D->GetRuntimeVersion() >= 9.029f && FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), 
																  m_pCurrentMode->pDisplay->Format, USAGE_QUERY_WRAPANDMIP, RTYPE_VOLUMETEXTURE, m_Format)) ))
				m_uLevels = 1;
			else if (dwVolumeTextureFilterCaps  & D3DPTFILTERCAPS_MIPFPOINT)
			{
				SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_POINT);
				SetSamplerState(1, SAMP_MIPFILTER, (DWORD)TEXF_POINT);
			}
			else if (dwVolumeTextureFilterCaps  & D3DPTFILTERCAPS_MIPFLINEAR)
			{
				SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_LINEAR);		
				SetSamplerState(1, SAMP_MIPFILTER, (DWORD)TEXF_LINEAR);
			}
			else
			{
				m_uLevels = 1;
				WriteToLog("CapsCheck: Volume mipmaping is set without corresponding filter caps\n");
			}
			break;
		}
		default:
			WriteToLog("CapsCheck: Unknown RType %d\n.", m_RType);
			return false;
	}
    m_uLevels = min(m_uLevels, MAX_LEVEL_COUNT);

	if (!( pCaps->dwPrimitiveMiscCaps & D3DPMISCCAPS_CULLNONE ))
	{
        WriteToLog("CapsCheck: Culling is not supported.\n");
        return false;
    } 

	if (m_Pool == D3DPOOL_SYSTEMMEM && !( pCaps->dwDevCaps & D3DDEVCAPS_TEXTURESYSTEMMEMORY ))
	{
        WriteToLog("CapsCheck: System memory texture are not supported by this driver.\n");
        return false;
    } 

	if ( !(pCaps->dwLineCaps & D3DLINECAPS_TEXTURE) )
	{
		WriteToLog("CapsCheck: Texture-mapping is not supported\n");
		return false;
	}

	//checking if textures of this format can be linearized for ref device
	if ( m_pRefDevice && (m_isRead || m_isRefRead) )
	{
		int nAdapter = m_pRefDevice->GetAdapterID();
		DISPLAYMODE Mode;
		hr = m_pD3D->GetAdapterDisplayMode( m_pRefDevice->GetAdapterID(), &Mode );
		if( FAILED(hr) )
		{
			WriteToLog("CapsCheck: GetAdapterDisplayMode() on ref device returned %s (0x%x)\n", m_pD3D->HResultToString(hr), hr );
			return false;
		}
		hr = m_pD3D->CheckDeviceFormat(nAdapter, m_pRefDevice->GetDevType(), Mode.Format, D3DUSAGE_QUERY_SRGBREAD, m_RType, m_Format);
		if (FAILED(hr))
		{
			WriteToLog("CapsCheck: D3DUSAGE_SRGBREAD is not supported on ref device for %s textures.\n", m_RType == RTYPE_CUBETEXTURE ? "cube " : (m_RType == RTYPE_VOLUMETEXTURE ? "vol " : "") );
			return false;
		}
	}

	if ( m_pRefDevice && (m_isWrite || m_isRefWrite) )
	{
		int nAdapter = m_pRefDevice->GetAdapterID();
		DISPLAYMODE Mode;
		hr = m_pD3D->GetAdapterDisplayMode( m_pRefDevice->GetAdapterID(), &Mode );
		if( FAILED(hr) )
		{
			WriteToLog("CapsCheck: GetAdapterDisplayMode() on ref device returned %s (0x%x)\n", m_pD3D->HResultToString(hr), hr );
			return false;
		}
		hr = m_pD3D->CheckDeviceFormat(nAdapter, m_pRefDevice->GetDevType(), Mode.Format, D3DUSAGE_QUERY_SRGBWRITE, m_RType, m_Format);
		if (FAILED(hr))
		{
			WriteToLog("CapsCheck: D3DUSAGE_SRGBWRITE is not supported on ref device for %s textures.\n", m_RType == RTYPE_CUBETEXTURE ? "cube " : (m_RType == RTYPE_VOLUMETEXTURE ? "vol " : "") );
			return false;
		}
	}

    //checling if textures of this format can be linearized for hal device
	if ((NULL != m_pSrcDevice) && (m_isRead || m_isSrcRead))
	{
		int nAdapter = m_pSrcDevice->GetAdapterID();
		DISPLAYMODE Mode;
		hr = m_pD3D->GetAdapterDisplayMode( m_pSrcDevice->GetAdapterID(), &Mode );
		if( FAILED(hr) )
		{
			WriteToLog("CapsCheck: GetAdapterDisplayMode() on src device returned %s (0x%x)\n", m_pD3D->HResultToString(hr), hr );
			return false;
		}
		hr = m_pD3D->CheckDeviceFormat(nAdapter, m_pSrcDevice->GetDevType(), Mode.Format, D3DUSAGE_QUERY_SRGBREAD, m_RType, m_Format);
		if (FAILED(hr))
		{
			WriteToLog("CapsCheck: D3DUSAGE_SRGBREAD is not supported on src device for %s textures.\n", m_RType == RTYPE_CUBETEXTURE ? "cube " : (m_RType == RTYPE_VOLUMETEXTURE ? "vol " : "") );
			return false;
		}
	}

	if ((NULL != m_pSrcDevice) && (m_isWrite || m_isSrcWrite))
	{
		int nAdapter = m_pSrcDevice->GetAdapterID();
		DISPLAYMODE Mode;
		hr = m_pD3D->GetAdapterDisplayMode( m_pSrcDevice->GetAdapterID(), &Mode );
		if( FAILED(hr) )
		{
			WriteToLog("CapsCheck: GetAdapterDisplayMode() on src device returned %s (0x%x)\n", m_pD3D->HResultToString(hr), hr );
			return false;
		}
		hr = m_pD3D->CheckDeviceFormat(nAdapter, m_pSrcDevice->GetDevType(), Mode.Format, D3DUSAGE_QUERY_SRGBWRITE, m_RType, m_Format);
		if (FAILED(hr))
		{
			WriteToLog("CapsCheck: D3DUSAGE_SRGBWRITE is not supported on src device for %s textures.\n", m_RType == RTYPE_CUBETEXTURE ? "cube " : (m_RType == RTYPE_VOLUMETEXTURE ? "vol " : "") );
			return false;
		}
	}

	return true;
}



