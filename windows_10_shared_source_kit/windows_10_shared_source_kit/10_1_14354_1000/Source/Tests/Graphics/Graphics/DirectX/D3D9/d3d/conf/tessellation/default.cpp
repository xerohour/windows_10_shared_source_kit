#define D3D_OVERLOADS
#include "Tessellation.h"
// ----------------------------------------------------------------------------

bool Tessellation::SetDefaultMatrices(void)
{   
    HRESULT     hr;
	float 		fAspect = 1.33f;
	float		fFOV = pi/4.0f;
	D3DXVECTOR3 	m_vAt, m_vUp;

	m_vAt  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vUp  = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

    D3DXMatrixIdentity( &m_ProjMatrix );
	D3DXMatrixPerspectiveFovLH(&m_ProjMatrix, fFOV, fAspect, 1.0f, 1000.0f);
    if (!SetTransform(D3DTS_PROJECTION, &m_ProjMatrix))
        return false;
	
    D3DXMatrixIdentity( &m_WorldMatrix );
	if (!SetTransform(D3DTS_WORLD, &m_WorldMatrix))
        return false;

	// Setup the view matrices
	D3DXVECTOR3 v[] = {
		D3DXVECTOR3( 0.0f,  0.0f, -3.0f),
		D3DXVECTOR3( 3.0f,  0.0f, -2.0f),
		D3DXVECTOR3( 0.0f,  3.0f, -2.0f),
		//D3DXVECTOR3(-3.0f,  0.0f, -2.0f),
		//D3DXVECTOR3( 0.0f, -3.0f, -2.0f)
	};
	m_nWVMatrices = m_nViewMatrices = sizeof(v)/sizeof(D3DXVECTOR3);
	for (UINT i = 0; i < m_nViewMatrices; i++)
	{
        D3DXMatrixLookAtLH(&(m_ViewMatrices[i]), &(v[i]), &m_vAt, &m_vUp);
		D3DXMatrixMultiply(&(m_WVMatrices[i]), &m_WorldMatrix, &(m_ViewMatrices[i]));
	}
	
    return true;
}

// ----------------------------------------------------------------------------

bool Tessellation::SetDefaultLights(void)
{
	int				m_nLightCount;
	D3DXVECTOR3 	m_vLoc, m_vAt;
    DWORD dwPosLightsSupported = 1;
    
	m_vLoc = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_vAt  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	CAPS caps;
	m_pSrcDevice->GetDeviceCaps(&caps); 

    // Light1: D3DLIGHT_DIRECTIONAL (no position, not affected by attenuation or range)
	m_pLights[0].Type           = D3DLIGHT_DIRECTIONAL;
	m_pLights[0].Diffuse.r      = 1.0f;
	m_pLights[0].Diffuse.g      = 1.0f;
	m_pLights[0].Diffuse.b      = 1.0f;
	m_pLights[0].Ambient.r      = 1.0f;
	m_pLights[0].Ambient.g      = 1.0f;
	m_pLights[0].Ambient.b      = 1.0f;
	m_pLights[0].Specular.r     = 1.0f;
	m_pLights[0].Specular.g     = 1.0f;
	m_pLights[0].Specular.b     = 1.0f;
	m_pLights[0].Direction      = m_vAt;
	m_pLights[0].Falloff		= 1.0f;

	//Light2: D3DLIGHT_POINT (no direction)
	m_pLights[1].Type           = D3DLIGHT_POINT;
	m_pLights[1].Diffuse.r      = 1.0f;
	m_pLights[1].Diffuse.g      = 1.0f;
	m_pLights[1].Diffuse.b      = 1.0f;
	m_pLights[1].Ambient.r      = 1.0f;
	m_pLights[1].Ambient.g      = 1.0f;
	m_pLights[1].Ambient.b      = 1.0f;
	m_pLights[1].Specular.r     = 1.0f;
	m_pLights[1].Specular.g     = 1.0f;
	m_pLights[1].Specular.b     = 1.0f;
	m_pLights[1].Position       = D3DXVECTOR3(0.0f, 0.0f, -3.0f);
	m_pLights[1].Attenuation0   = 0.98f;//0.98f;
	m_pLights[1].Attenuation1   = 0.015f;//0.015f;
	m_pLights[1].Attenuation2   = 0.005f;//0.05f;
	m_pLights[1].Range          = 1000.0f;
	m_pLights[1].Falloff		= 1.0f;

	//Light0: D3DLIGHT_SPOT
	m_pLights[2].Type           = D3DLIGHT_SPOT;
    m_pLights[2].Diffuse.r      = 1.0f;
    m_pLights[2].Diffuse.g      = 1.0f;
    m_pLights[2].Diffuse.b      = 1.0f;
    m_pLights[2].Ambient.r      = 1.0f;
    m_pLights[2].Ambient.g      = 1.0f;
    m_pLights[2].Ambient.b      = 1.0f;
    m_pLights[2].Specular.r     = 1.0f;
    m_pLights[2].Specular.g     = 1.0f;
    m_pLights[2].Specular.b     = 1.0f;
    m_pLights[2].Position       = m_vLoc;
    m_pLights[2].Direction      = m_vAt;
    m_pLights[2].Attenuation0   = 0.98f;
	m_pLights[2].Attenuation1   = 0.015f;
	m_pLights[2].Attenuation2   = 0.005f;
	m_pLights[2].Theta			= D3DX_PI/10;
    m_pLights[2].Phi			= D3DX_PI/6;
    m_pLights[2].Range          = 1000.0f;
	m_pLights[2].Falloff	    = 1.0f; 

    // Check what kind of lights are available
    if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        dwPosLightsSupported = (caps.VertexProcessingCaps & D3DVTXPCAPS_POSITIONALLIGHTS);
		if ( dwPosLightsSupported )
			SetLight(0, &(m_pLights[1]));
        else 
		{
			if( !(caps.VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS) )
			{
				WriteToLog("(FAIL) T&L Device does not support at least Directional Lights.  Test cannot continue.\n");
				return false;
			}
			else 
				SetLight(0, &(m_pLights[0])); 
		}
	}
	// set spot light
	if (KeySet(_T("l2"))) 
		SetLight(0, &(m_pLights[2]));
	// set point light
	else if (KeySet(_T("l1"))) 
		SetLight(0, &(m_pLights[1]));
	// set directional light
	else if (KeySet(_T("l0")))
		SetLight(0, &(m_pLights[0]));
   	
	LightEnable(0, TRUE);

    return true;
}

bool Tessellation::SetDefaultRenderStates(void)
{
	SetRenderState(D3DRS_AMBIENT,0x00505050);
    SetRenderState(D3DRS_NORMALIZENORMALS, (DWORD)TRUE);
	SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_NONE);
	if (KeySet(_T("ccw")))
		SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_CCW);
	SetRenderState(D3DRS_LIGHTING, (DWORD)TRUE);
	SetRenderState(D3DRS_ZENABLE, (DWORD)FALSE);
	if(KeySet(_T("wire")))
		SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_WIREFRAME);
	else if(KeySet(_T("wiresrc")))
		m_pSrcDevice->SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_WIREFRAME);
	else if(KeySet(_T("wireref")))
	{
		if ( NULL != m_pRefDevice )
			m_pRefDevice->SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_WIREFRAME);
	}
	else
		SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_SOLID);
	if (KeySet(_T("continuous")))
		SetRenderState(D3DRS_PATCHEDGESTYLE, (DWORD)D3DPATCHEDGE_CONTINUOUS);
	else
		SetRenderState(D3DRS_PATCHEDGESTYLE, (DWORD)D3DPATCHEDGE_DISCRETE);
	if (m_AdapTess)
	{
		float MinTessLevel = 1.f;
		SetRenderState(D3DRS_MINTESSELLATIONLEVEL, *(DWORD*)&MinTessLevel);
		SetRenderState(D3DRS_MAXTESSELLATIONLEVEL, *(DWORD*)&m_MaxTess);
		SetRenderState(D3DRS_ENABLEADAPTIVETESSELLATION, (DWORD)TRUE);
	}
		
	SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	
	return true;
}

bool Tessellation::CapsCheck()
{
	CAPS caps;
	m_pSrcDevice->GetDeviceCaps(&caps); 
	if ( !( caps.PrimitiveMiscCaps & D3DPMISCCAPS_CULLNONE) )
	{
		WriteToLog("CapsCheck:: - Culling  is not supported.\n");
        return false;
    }
	if ( !KeySet(_T("notex")) && !(caps.LineCaps & D3DLINECAPS_TEXTURE) )
	{
		WriteToLog("CapsCheck: Texture-mapping is not supported\n");
		return false;
	}
	if (!(caps.DevCaps & D3DDEVCAPS_QUINTICRTPATCHES))
		m_UseQuinticRT = false;
	
	m_MaxTess =  min(20.f, caps.MaxNpatchTessellationLevel);
	
	return true;
}
