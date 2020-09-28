#include <d3d8.h>
#include <d3dx8.h>
#include "MVertex.h"

bool MVertex::SetDefaultMatrices(void) {
    D3DXMATRIX			Matrix;
    // We need to be far from scane to receive correct specular highlights
	D3DXVECTOR3			from(0.0f, 0.0f, 1.0f); 
    D3DXVECTOR3	        at(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3	        up(0.0f, 1.0f, 0.0f);
    float				fWidth = 100.f;
	float				fHeight = 100.f;
	float               fZn = -2.f;
	float               fZf = 2.f;

    
	// Projection matrix
    D3DXMatrixOrthoLH(&Matrix, 2.f, 2.f, fZn, fZf);
	if (!SetTransform(D3DTS_PROJECTION, (LPD3DMATRIX)&Matrix))
		return false;

	D3DXMatrixIdentity(&Matrix);
    if (!SetTransform(D3DTS_WORLD, (LPD3DMATRIX)&Matrix))
        return false;

    if (!SetTransform(D3DTS_VIEW, (LPD3DMATRIX)&Matrix))
        return false;

    return true;
}

bool MVertex::SetDefaultMaterials(void) {
    MATERIAL		Material;

    ZeroMemory(&Material,sizeof(MATERIAL));
    Material.diffuse.r  = 1.0f;
    Material.diffuse.g  = 1.0f;
    Material.diffuse.b  = 1.0f;
    Material.diffuse.a  = 1.0f;
    Material.ambient.r  = 1.0f;
    Material.ambient.g  = 1.0f;
    Material.ambient.b  = 1.0f;
    Material.ambient.a  = 1.0f;
    Material.specular.r = 1.0f;
    Material.specular.g = 1.0f;
    Material.specular.b = 1.0f;
    Material.specular.a = 1.0f;
    Material.emissive.r = 0.0f;
    Material.emissive.g = 0.0f;
    Material.emissive.b = 0.0f;
    Material.emissive.a = 0.0f;
    Material.power      = 20.0f;

    if (!SetMaterial(&Material))
        return false;

    return true;
}


bool MVertex::SetDefaultLights(void) {
    LIGHT		Light;
	D3DXVECTOR3	vLightPos = D3DXVECTOR3(0.f, 0.f, -500.f);
	D3DXVECTOR3	vLightDir = D3DXVECTOR3(0.f, 0.f, 1.f);

	ZeroMemory(&Light,sizeof(D3DLIGHT8));

	Light.dltType		= D3DLIGHT_SPOT;
	Light.Diffuse.r		= 1.0f;
	Light.Diffuse.g		= 1.0f;
	Light.Diffuse.b		= 1.0f;
	Light.Diffuse.a		= 1.0f;
	Light.Position		= vLightPos;
	Light.Direction		= vLightDir;
	Light.Range           = 10e10f;
	Light.Attenuation0    = 1.f;
	Light.Attenuation1    = 0.f;
	Light.Attenuation2    = 0.f;
    Light.Theta			= pi/50;
    Light.Phi				= pi/20;

	if (!SetLight(0,&Light))
		return false;
	if (!LightEnable(0, TRUE))
		return false;
	return true;
}


bool MVertex::SetDefaultRenderStates(void) {
	SetRenderState(D3DRS_AMBIENT,RGB(200,200,200));
	SetRenderState(D3DRS_LOCALVIEWER,(DWORD)TRUE);
	SetRenderState(D3DRS_CLIPPING, (DWORD)TRUE);
	SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
	SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_ZERO);
	SetRenderState(D3DRS_LIGHTING, (DWORD)TRUE);
	SetTextureStageState(0, D3DTSS_MINFILTER, (DWORD)TEXF_LINEAR);
	SetTextureStageState(0, D3DTSS_MIPFILTER, (DWORD)TEXF_POINT);
	SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_MIRROR);
	SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_MIRROR);
	return true;
}
