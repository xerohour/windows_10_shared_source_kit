#define D3D_OVERLOADS
#include "DisplacementMapping.h"
// ----------------------------------------------------------------------------

bool DisplacementMapping::SetDefaultMatrices(void)
{   
	D3DXMATRIX	Matrix;
	D3DXMATRIX	m_RotMatrix, m_PosMatrix;
	float 		fAspect = 1.33f;
	float		fFOV = pi/4.0f;
    int     	m_nFramesPerRot = 1;
    float		m_fAngle = (1.0f * pi) / (float)m_nFramesPerRot;
	D3DXVECTOR3 	m_vLoc, m_vAt, m_vUp;
	D3DXMATRIX		m_TexTrans[3];
    int 			m_nTPViewMatrix = 0;
    D3DXMATRIX		m_TPViewMatrix[VIEW_COUNT];

    m_vLoc = D3DXVECTOR3(0.0f, 0.0f, -3.0f);
	m_vAt  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vUp  = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	D3DXMatrixRotationY(&m_RotMatrix, -m_fAngle);
    D3DXMatrixIdentity(&m_PosMatrix);

    D3DXMatrixIdentity( &m_ProjMatrix );
	D3DXMatrixPerspectiveFovLH(&m_ProjMatrix, fFOV, fAspect, 1.0f, 1000.0f);
    if (!SetTransform(D3DTS_PROJECTION,&m_ProjMatrix))
        return false;
	
    D3DXMatrixIdentity( &Matrix );
	if (!SetTransform(D3DTS_WORLD, &Matrix))
        return false;

	/*// Setup the view matrices
	D3DXVECTOR3 v = D3DXVECTOR3(0.0f, 0.0f, -3.0f);
    D3DXMatrixLookAtLH(&(m_ViewMatrices[0]), &m_vLoc, &m_vAt, &m_vUp);
	if (!SetTransform(D3DTS_VIEW, &m_ViewMatrices[0]))
        return false;
	v = D3DXVECTOR3(3.0f, 0.0f, -1.0f);
    D3DXMatrixLookAtLH(&(m_ViewMatrices[1]), &v, &m_vAt, &m_vUp);
    v = D3DXVECTOR3(-3.0f, 0.0f, -1.0f);
	D3DXMatrixLookAtLH(&(m_ViewMatrices[2]), &v, &m_vAt, &m_vUp);
    v = D3DXVECTOR3(0.0f, 3.0f, -1.0f);
    D3DXMatrixLookAtLH(&(m_ViewMatrices[3]), &v, &m_vAt, &m_vUp);
    v = D3DXVECTOR3(0.0f, -3.0f, -1.0f);
	D3DXMatrixLookAtLH(&(m_ViewMatrices[4]), &v, &m_vAt, &m_vUp);*/


   // Setup the view matrices
	D3DXVECTOR3 v[] = {
		D3DXVECTOR3( 0.0f,  0.0f, -3.0f),
		D3DXVECTOR3( 3.0f,  0.0f, -1.0f),
        D3DXVECTOR3( 0.0f,  3.0f, -1.0f),
		//D3DXVECTOR3(-3.0f,  0.0f, -1.0f),
		//D3DXVECTOR3( 0.0f, -3.0f, -1.0f)
	};
	m_nViewMatrices = sizeof(v)/sizeof(D3DXVECTOR3);
	for (UINT i = 0; i < m_nViewMatrices; i++)
	{
		D3DXMatrixIdentity( &Matrix );
        D3DXMatrixLookAtLH(&(m_ViewMatrices[i]), &(v[i]), &m_vAt, &m_vUp);
		D3DXMatrixMultiply(&(m_WVMatrices[i]), &Matrix, &(m_ViewMatrices[i]));
	}
   return true;
}

// ----------------------------------------------------------------------------

bool DisplacementMapping::SetDefaultMaterials(void)
{   
	MATERIAL mat;
	memset(&mat, 0 , sizeof(MATERIAL));
	mat.diffuse.r = 1.0f;
	mat.diffuse.g = 0.0f;
	mat.diffuse.b = 0.0f;
	mat.diffuse.a = 1.0f;
	mat.ambient.r = 0.f;
	mat.ambient.g = 0.f;   
	mat.ambient.b = 0.f; 
    mat.specular.r = 1.0f;
	mat.specular.g = 1.0f;   
	mat.specular.b = 1.0f;
	mat.power = 10.0f;
	SetMaterial(&mat);
	        
	return true;
}


// ----------------------------------------------------------------------------

bool DisplacementMapping::SetDefaultLights(void)
{
	int				m_nLightCount;
	D3DXVECTOR3 	m_vLoc, m_vAt, m_vUp;
    DWORD dwPosLightsSupported = 1;
    LIGHT m_pLights[2];
	CAPS caps;
	m_pSrcDevice->GetDeviceCaps(&caps); 

	m_vLoc = D3DXVECTOR3(0.0f, 0.0f, -3.0f);
	m_vAt  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vUp  = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// Set the total light count
	m_nLightCount = 2;

    // Check what kind of lights are available
    if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        dwPosLightsSupported = (caps.VertexProcessingCaps & D3DVTXPCAPS_POSITIONALLIGHTS);

        if(!dwPosLightsSupported && !(caps.VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS))
        {
            WriteToLog("(FAIL) T&L Device does not support at least Directional Lights.  Test cannot continue.\n");
            return false;
        }
    }                                    

    // Light0
    m_pLights[0].Type             = dwPosLightsSupported ? D3DLIGHT_POINT : D3DLIGHT_DIRECTIONAL;
    m_pLights[0].Diffuse.r        = 1.0f;
    m_pLights[0].Diffuse.g        = 1.0f;
    m_pLights[0].Diffuse.b        = 1.0f;
    m_pLights[0].Ambient.r        = 1.0f;
    m_pLights[0].Ambient.g        = 1.0f;
    m_pLights[0].Ambient.b        = 1.0f;
    m_pLights[0].Specular.r       = 1.0f;
    m_pLights[0].Specular.g       = 1.0f;
    m_pLights[0].Specular.b       = 1.0f;
    m_pLights[0].Position       = D3DXVECTOR3(m_vLoc.x, m_vLoc.y, m_vLoc.z);
    m_pLights[0].Direction      = m_vAt;
    m_pLights[0].Attenuation0   = 0.98f;
    m_pLights[0].Attenuation1   = 0.015f;
    m_pLights[0].Attenuation2   = 0.005f;
    m_pLights[0].Range          = 100.0f;
	m_pLights[0].Falloff		   = 1.0f;  
	
	//m_pLights[0].Position     = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVec3Normalize( (D3DXVECTOR3*)&m_pLights[0].Direction, &D3DXVECTOR3( 0.0f, 1.0f, 1.0f ) );

    SetLight(0, &(m_pLights[0]));
	LightEnable(0, TRUE);

    // Light1
	m_pLights[1].Type             = dwPosLightsSupported ? D3DLIGHT_POINT : D3DLIGHT_DIRECTIONAL;
	m_pLights[1].Diffuse.r        = 1.0f;
	m_pLights[1].Diffuse.g        = 1.0f;
	m_pLights[1].Diffuse.b        = 1.0f;
	m_pLights[1].Ambient.r        = 1.0f;
	m_pLights[1].Ambient.g        = 1.0f;
	m_pLights[1].Ambient.b        = 1.0f;
	m_pLights[1].Specular.r       = 1.0f;
	m_pLights[1].Specular.g       = 1.0f;
	m_pLights[1].Specular.b       = 1.0f;
	m_pLights[1].Position       = D3DXVECTOR3(m_vLoc.x, m_vLoc.y, -m_vLoc.z);
	m_pLights[1].Direction      = m_vAt;
	m_pLights[1].Attenuation0   = 0.98f;
	m_pLights[1].Attenuation1   = 0.015f;
	m_pLights[1].Attenuation2   = 0.005f;
	m_pLights[1].Range          = 100.0f;
	m_pLights[1].Falloff		   = 1.0f;
    SetLight(1, &(m_pLights[1]));
	LightEnable(1, TRUE);

    return true;
}

bool DisplacementMapping::SetDefaultRenderStates(void) 
{
	SetRenderState(D3DRS_NORMALIZENORMALS, (DWORD)TRUE);
	SetRenderState(D3DRS_LIGHTING, (DWORD)TRUE);
	
	SetSamplerState(0, SAMP_BORDERCOLOR, 0xff969696);
	if (!(KeySet(_T("noDisMap")) || KeySet(_T("fixed"))))
		SetSamplerState(DMAPSAMPLER, SAMP_BORDERCOLOR, 0xff969696);
    SetTextureStageState((DWORD)0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
	SetTextureStageState((DWORD)0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
	if(KeySet(_T("tex")))
		SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
	else if(!m_Tex)
		SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG2);
	else
		SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE);

	if(KeySet(_T("wire")))
		SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	//enable N-Patches
	SetNPatchMode(1.f);

	float MinTessLevel = 1.f;
	float MaxTessLevel = 100.f;
	SetRenderState(D3DRS_MINTESSELLATIONLEVEL, *(DWORD*)&MinTessLevel);
	SetRenderState(D3DRS_MAXTESSELLATIONLEVEL, *(DWORD*)&MaxTessLevel);

	return true;
}

bool DisplacementMapping::CapsCheck()
{
	CAPS caps;
	m_pSrcDevice->GetDeviceCaps(&caps); 

	//CHECK FOR N-PATCHES SUPPORT
	if (!(caps.DevCaps & D3DDEVCAPS_NPATCHES))
	{
        WriteToLog("CapsCheck: N-Patches is not supported.\n");
        return false;
    }
	
	//CHECK FOR DISPLACEMENT MAPPING SUPPORT
	if (!(KeySet(_T("noDisMap")) || KeySet(_T("fixed"))))
	{
		if (!(caps.DevCaps2 & D3DDEVCAPS2_DMAPNPATCH))
		{
			WriteToLog("CapsCheck: Lookup Displacement Mapping is not supported.\n");
			m_Lookup = false; 		
			if (!(caps.DevCaps2 & D3DDEVCAPS2_PRESAMPLEDDMAPNPATCH) )
			{
				WriteToLog("CapsCheck: Presampling is not supported.\n");
				return false;
			}
		}
		else if (!(caps.DevCaps2 & D3DDEVCAPS2_PRESAMPLEDDMAPNPATCH) )
		{
			m_Presampled = false;
			WriteToLog("CapsCheck: Presampled Displacement Mapping is not supported.\n");
		}
		
		//CHECK VERTEX SHADER VERSION SUPPORT
		if( caps.VertexShaderVersion < D3DVS_VERSION(1,1) )
		{
			WriteToLog("CapsCheck: vertex shaders version 1.1 or higher are not supported\n");
			return false;
		}
		//check if this format is valid for displacement mapping
		HRESULT hr;
		FORMAT Format = m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
		int nAdapter = m_pSrcDevice->GetAdapterID();
		DISPLAYMODE Mode;
		hr = m_pD3D->GetAdapterDisplayMode( m_pSrcDevice->GetAdapterID(), &Mode );
		if( FAILED(hr) )
		{
			WriteToLog("CapsCheck: GetAdapterDisplayMode() on src device returned %s (0x%x)\n", m_pD3D->HResultToString(hr), hr );
			return false;
		}
		hr = m_pD3D->CheckDeviceFormat(nAdapter, m_pSrcDevice->GetDevType(), Mode.Format,
									USAGE_DMAP, RTYPE_TEXTURE, Format);
		if (FAILED(hr))
		{
			WriteToLog("CapsCheck: This format can not be used for displacement mapping.\n");
			return false;
		}
		if ( m_pRefDevice )
		{
			nAdapter = m_pRefDevice->GetAdapterID();
			hr = m_pD3D->GetAdapterDisplayMode( m_pRefDevice->GetAdapterID(), &Mode );
			if( FAILED(hr) )
			{
				WriteToLog("CapsCheck: GetAdapterDisplayMode() on ref device returned %s (0x%x)\n", m_pD3D->HResultToString(hr), hr );
				return false;
			}
			hr = m_pD3D->CheckDeviceFormat(nAdapter, m_pRefDevice->GetDevType(), Mode.Format,
										USAGE_DMAP, RTYPE_TEXTURE, Format);
			if (FAILED(hr))
			{
				WriteToLog("CapsCheck: This format can not be used for displacement mapping.\n");
				return false;
			}
		}
	}

	if (m_AdapTess && !(caps.DevCaps2 & D3DDEVCAPS2_ADAPTIVETESSNPATCH)) 
	{
        WriteToLog("CapsCheck: adaptive tesselation is not supported for N-patches.\n");
        m_AdapTess = false;
    }

	if ( !(caps.LineCaps & D3DLINECAPS_TEXTURE) )
	{
		WriteToLog("CapsCheck: Texture-mapping is not supported\n");
		return false;
	}

	m_MaxTess = min(20, caps.MaxNpatchTessellationLevel);

	return true;
}



