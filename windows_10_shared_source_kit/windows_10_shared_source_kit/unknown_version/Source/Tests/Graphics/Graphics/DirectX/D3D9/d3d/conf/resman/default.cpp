#define D3D_OVERLOADS
#include <ddraw.h>
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "ResMan.h"

bool ResMan::SetDefaultMatrices(void) {
    D3DXMATRIX   Matrix;
    // We need to be far from scane to receive correct specular highlights
	D3DXVECTOR3  from(0.f, 0.0f, 1.0f); 
    D3DXVECTOR3  at  (0.f, 0.0f, 0.0f);
    D3DXVECTOR3  up  (0.0f, 1.0f, 0.0f);
    float	     fWidth  = 100.f;
	float		 fHeight = 100.f;
    
	// Projection matrix
	D3DXMatrixOrthoLH(&Matrix, 2.f, 2.f, -1.0f, 1.0f );
	//m_mProjectionMatrix = ProjectionMatrix(pi/4, 1.f, fZn, fZf);
	if (!SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&Matrix))
		return false;

    D3DXMatrixIdentity( &Matrix );
    if (!SetTransform(D3DTS_WORLD,(D3DMATRIX*)&Matrix))
        return false;

    D3DXMatrixLookAtLH(&Matrix, &from, &at, &up);
    if (!SetTransform(D3DTS_VIEW,(D3DMATRIX*)&Matrix))
        return false;

    return true;
}

//-----------------------------------------------------------------------------

bool ResMan::SetDefaultMaterials(void) {
    MATERIAL	Material;

    ZeroMemory(&Material,sizeof(MATERIAL));
    Material.diffuse.r  = 1.0f;
    Material.diffuse.g  = 1.0f;
    Material.diffuse.b  = 1.0f;
    Material.diffuse.a  = 0.0f;
    Material.ambient.r  = 1.0f;
    Material.ambient.g  = 1.0f;
    Material.ambient.b  = 1.0f;
    Material.ambient.a  = 0.0f;
    Material.specular.r = 1.0f;
    Material.specular.g = 1.0f;
    Material.specular.b = 1.0f;
    Material.specular.a = 0.0f;
    Material.emissive.r = 0.0f;
    Material.emissive.g = 0.0f;
    Material.emissive.b = 0.0f;
    Material.emissive.a = 0.0f;
    Material.power      = 20.0f;

    if (!SetMaterial(&Material))
        return false;

    return true;
}

//-----------------------------------------------------------------------------

bool ResMan::SetDefaultLights(void) {
    LIGHT		Light;
	D3DXVECTOR3	vLightPos = D3DXVECTOR3(0.f, 0.f, 500.f);
	D3DXVECTOR3	vLightDir = D3DXVECTOR3(0.f, 0.f, 1.f);//0.f, -0.707f, 0.707f

	ZeroMemory(&Light,sizeof(LIGHT));

	Light.dltType		= D3DLIGHT_SPOT;
	Light.Diffuse.r		= 1.0f;
	Light.Diffuse.g		= 1.0f;
	Light.Diffuse.b		= 1.0f;
	Light.Diffuse.a		= 1.0f;
	Light.Position		= vLightPos;
	Light.Direction		= vLightDir;
	Light.Range         = 10e10f;
    Light.Falloff		= 0.001f;
	Light.Attenuation0  = 1.0f;
	Light.Attenuation1  = 0.0f;
	Light.Attenuation2  = 0.0f;
    Light.Theta			= pi/50;
    Light.Phi			= pi/20;

	if (!SetLight(0,&Light))
		return false;
	if (!LightEnable(0, TRUE))
		return false;
	return true;
}

//-----------------------------------------------------------------------------

bool ResMan::SetDefaultRenderStates(void) {
	if (m_dwVersion >= 0x0700) 
	{
		SetRenderState(D3DRS_AMBIENT, RGB_MAKE(255,255,255));
		//SetRenderState(D3DRS_LOCALVIEWER,(DWORD)TRUE);					//it's default
		//SetRenderState(D3DRS_CLIPPING, (DWORD)TRUE);						//it's default
	}
//	else
//		SetLightState(D3DLIGHTSTATE_AMBIENT, RGB_MAKE(255,255,255));		
	
	//SetRenderState(D3DRS_LIGHTING, (DWORD)TRUE);							//it's default
	SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_NONE);
	//SetRenderState(D3DRS_WRAP0, (DWORD)0); //it's default
	//SetTextureStageState(0, D3DTSS_MINFILTER, (DWORD)TEXF_POINT);					//it's default
	//SetTextureStageState(0, D3DTSS_MAGFILTER, (DWORD)TEXF_POINT);					//it's default
	//SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE );				//it's default
	SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
	SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	//for A8 textures
	SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	//SetTextureStageState((DWORD)0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);		//it's default
	//SetTextureStageState((DWORD)0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE);		//it's default

	return true;
}

//-----------------------------------------------------------------------------

bool ResMan::CapsCheck()
{
	m_MaxTextureWidth  = 0;
	m_MaxTextureHeight = 0;
	m_MaxCubeWidth     = 0;
	m_MaxVolumeExtent  = 0;
	m_UseLODBias         = true;
	m_UseTextures        = true;
	m_UseCubeMaps        = true;
	m_UseVolMaps         = true;
	m_UseMipMaps         = true;
	m_UseMipCubeMaps     = true;
	m_UseMipVolMaps      = true;
	m_Use32BitIB         = true;
	m_CubeMapIsPow2      = false;
	m_VolumeMapIsPow2    = false;
	m_TextureIsPow2      = false;
	m_SquareOnly         = false;
	m_NonPow2Conditional = false;

	if (m_dwVersion >= 0x0900)
	{
		//we don't run the test with driver resource management any more since nobody supports it
		if(KeySet(_T("DriverManage")) && !( m_pSrcDevice->GetCaps()->dwCaps2 & D3DCAPS2_CANMANAGERESOURCE ))
		{
			WriteToLog("CheckCaps: Driver resource management is not supported.\n");
			return false;
		}
		CheckDX9Caps();
	}
	else if (m_dwVersion >= 0x0800)
	{
		//we don't run the test with driver resource management any more since nobody supports it
		if(KeySet(_T("DriverManage")) && !( m_pSrcDevice->GetCaps()->dwCaps2 & D3DCAPS2_CANMANAGERESOURCE ))
		{
			WriteToLog("CheckCaps: Driver resource management is not supported.\n");
			return false;
		}
		CheckDX8Caps();
	}
	else if (m_dwVersion >= 0x0700)
	{
		//we don't run the test with driver resource management any more since nobody supports it
		if(KeySet(_T("DriverManage")) && !( m_pD3D->GetDDCaps()->dwCaps2 & DDCAPS2_CANMANAGETEXTURE ))
		{
			WriteToLog("CheckCaps: Driver resource management is not supported.\n");
			return false;
		}
		CheckDX7Caps();
	}
//	else
//	{
		/*if (!( m_pSrcDevice->GetCaps()->dwCaps2 & DDCAPS2_CANMANAGETEXTURE ))
		{
			WriteToLog("CheckCaps: Driver resource management is not supported.\n");
			return false;
		}*/
//		CheckDX6Caps();
//	}

	if (!(m_pSrcDevice->GetCaps()->dwPrimitiveMiscCaps & D3DPMISCCAPS_CULLNONE))
	{
		WriteToLog("CheckCaps: CULLNONE is not supported.\n");
		return false;
	}

	return true;
}
