#define D3D_OVERLOADS
#include "Lines.h"

// ----------------------------------------------------------------------------

bool Lines::SetDefaultMatrices(void)
{   
	D3DXMATRIX	Matrix1, Matrix2;
	D3DXMATRIX	PosMatrix, ProjMatrix, WorldMatrix; //m_RotMatrix, 
	float 		fAspect = 1.33f;
	float		fFOV = pi/4.0f;
    //int     	m_nFramesPerRot = 1;
   // float		m_fAngle = (1.0f * pi) / (float)m_nFramesPerRot;
	D3DXVECTOR3 	m_vLoc, m_vAt, m_vUp;
    //int 			m_nTPViewMatrix = 0;
    //D3DXMATRIX		m_TPViewMatrix[5];

    m_vLoc = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	m_vAt  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vUp  = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	//D3DXMatrixRotationY(&m_RotMatrix, -m_fAngle);
    D3DXMatrixIdentity(&PosMatrix);

    D3DXMatrixIdentity( &ProjMatrix );
	D3DXMatrixPerspectiveFovLH(&ProjMatrix, fFOV, fAspect, 1.0f, 1000.0f);
    if (!SetTransform(D3DTS_PROJECTION,&ProjMatrix))
        return false;
	
    D3DXMatrixIdentity( &WorldMatrix );
	if (!SetTransform(D3DTS_WORLD, &WorldMatrix))
        return false;
	
	// Setup the view matrices
	D3DXVECTOR3 v[] = {
		D3DXVECTOR3( 0.0f,  0.0f, -2.5f),
		/*D3DXVECTOR3( 3.0f,  0.0f, -1.0f),
		D3DXVECTOR3(-3.0f,  0.0f, -1.0f),
		D3DXVECTOR3( 0.0f,  3.0f, -1.0f),
		D3DXVECTOR3( 0.0f, -3.0f, -1.0f)*/
	};
	m_nViewMatrices = sizeof(v)/sizeof(D3DXVECTOR3);
	//for (int i =0; i < m_nViewMatrices; i++)
		//m_mTransform[i] = Matrix1;
	for (UINT i = 0; i < m_nViewMatrices; i++)
	{
        D3DXMatrixLookAtLH(&(m_ViewMatrices[i]), &(v[i]), &m_vAt, &m_vUp);
		D3DXMatrixMultiply(&m_mTransform[i], &WorldMatrix, &m_ViewMatrices[i]);
		Matrix1 = m_mTransform[i];
		D3DXMatrixMultiply(&m_mTransform[i], &Matrix1, &ProjMatrix);

		//viewport matrix
		D3DXMatrixIdentity(&Matrix2);
		Matrix2(0,0) =  0.f + (FLOAT)( m_pCurrentMode->nBackBufferWidth/2 );
		Matrix2(3,0) =  0.f + (FLOAT)( m_pCurrentMode->nBackBufferWidth/2 );
		Matrix2(1,1) = -1.f * (FLOAT)( m_pCurrentMode->nBackBufferHeight/2 );
		Matrix2(3,1) =  0.f + (FLOAT)( m_pCurrentMode->nBackBufferHeight/2 );
		Matrix2(2,2) = 1.f - 0.f;
		Matrix2(3,2) = 0.f;
		Matrix2(3,3) = 1.f;
	
		Matrix1 = m_mTransform[i];
		D3DXMatrixMultiply(&m_mTransform[i], &Matrix1, &Matrix2);
	}
	
   return true;
}

// ----------------------------------------------------------------------------

bool Lines::SetDefaultMaterials(void)
{   
	MATERIAL mat;
	memset(&mat, 0 , sizeof(MATERIAL));
	mat.diffuse.r = 1.0f;
	mat.diffuse.g = 1.0f;
	mat.diffuse.b = 1.0f;
	mat.diffuse.a = 1.0f;
	mat.ambient.r = 0.3f;
	mat.ambient.g = 0.3f;   
	mat.ambient.b = 0.3f; 
    mat.specular.r = 1.0f;
	mat.specular.g = 1.0f;   
	mat.specular.b = 1.0f;
	mat.power = 10.0f;
	SetMaterial(&mat);
	        
	return true;
}

// ----------------------------------------------------------------------------

bool Lines::SetDefaultLights(void)
{
	int				m_nLightCount;
	D3DXVECTOR3 	m_vLoc, m_vAt, m_vUp;
    DWORD dwPosLightsSupported = 1;
    LIGHT m_pLights[2];
	m_vLoc = D3DXVECTOR3(0.0f, 0.0f, -3.0f);
	m_vAt  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vUp  = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// Set the total light count
	m_nLightCount = 2;

    // Check what kind of lights are available
    if(m_pSrcDevice->GetCaps()->dwDevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        dwPosLightsSupported = (m_pSrcDevice->GetCaps()->dwVertexProcessingCaps & D3DVTXPCAPS_POSITIONALLIGHTS);

        if(!dwPosLightsSupported && !(m_pSrcDevice->GetCaps()->dwVertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS))
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

//-----------------------------------------------------------------------------

bool Lines::SetDefaultRenderStates(void) 
{
	if (KeySet("zenable"))
	{
		SetRenderState(D3DRS_ZENABLE, (DWORD)TRUE);
		WriteToLog("Enabled z-compare");
	}
	else
		SetRenderState(D3DRS_ZENABLE, (DWORD)FALSE);

	SetRenderState(D3DRS_NORMALIZENORMALS, (DWORD)TRUE);
	//SetRenderState(D3DRS_LIGHTING, (DWORD)TRUE);		//it's default value
	if (KeySet(_T("solid")))
		SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_SOLID);
	else
		SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_WIREFRAME);
	SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_NONE);

	if ( m_isAntiAliased )
	{
		//m_pFramework->m_ImageCmp.SetPassRequired(0.75f);
		SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		if (KeySet("zero"))
			SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);//INVSRCALPHA);
		else
			SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		//SetTextureStageState((DWORD)0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);		//this is default value
		SetTextureStageState((DWORD)0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_DIFFUSE);
		if ( KeySet(_T("srcaa")) )
			m_pSrcDevice->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, (DWORD)TRUE );
		else if ( KeySet(_T("refaa")) )
		{
			if (m_pRefDevice != NULL)
			m_pRefDevice->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, (DWORD)TRUE );
		}
		else
			SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, (DWORD)TRUE );
	}
	else
	{
		m_pFramework->m_ImageCmp.SetPassRequired(0.85f);
		SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, (DWORD)FALSE );
	}

	SetRenderState( D3DRS_SHADEMODE, m_ShadingMode);

	//by default we want to set linear filters
	//however if current format doesn't support linear filters, we'll use point filters
	int nAdapter = m_pSrcDevice->GetAdapterID();
	DISPLAYMODE Mode;
	FORMAT Format = m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
	if (m_dwVersion >= 0x0900)
	{
		if (!KeySet("pointf") && 
			SUCCEEDED(m_pD3D->GetAdapterDisplayMode( m_pSrcDevice->GetAdapterID(), &Mode )) &&
			SUCCEEDED(m_pD3D->CheckDeviceFormat(nAdapter, m_pSrcDevice->GetDevType(), Mode.Format, D3DUSAGE_QUERY_FILTER, RTYPE_TEXTURE, Format)))
		{
			SetSamplerState(0, SAMP_MAGFILTER, (DWORD)TEXF_LINEAR);
			SetSamplerState(0, SAMP_MINFILTER, (DWORD)TEXF_LINEAR);
		}
	}
	else
	{
		if (m_pSrcDevice->GetCaps()->dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)
		{
			SetSamplerState(0, SAMP_MAGFILTER, (DWORD)TEXF_LINEAR);
			SetSamplerState(0, SAMP_MINFILTER, (DWORD)TEXF_LINEAR);
		}
	}
	
	return true;
}


