#define D3D_OVERLOADS
//#include <d3d8.h>
#include "Tessellation.h"
// Global Test and App instance
#include "cd3dx.h"		//for D3DXAssembleShader9

CD3DWindowFramework App;
CRectPatchesTest    RectPatchesTest;
CTriPatchesTest     TriPatchesTest;
CNPatchesTest       NPatchesTest;
CMinimizingRectTest MinRectTest;
CMinimizingTriTest  MinTriTest;
CMaxMinRectTest     MaxMinRectTest;
CMaxMinTriTest      MaxMinTriTest;
CTessFactorRectTest TessFactorRectTest;
CTessFactorTriTest  TessFactorTriTest;
CDataTypesRectTest  DataTypesRectTest;
CDataTypesTriTest   DataTypesTriTest;
CDataTypesNTest     DataTypesNTest;
CNewNTest           NewNTest;
CMinNTest           MinNTest;
CTessFNTest         TessFNTest;
CMinMaxNTest        MinMaxNTest;

Tessellation::Tessellation()
{
	
	m_szTestName = "Tessellation";
	m_szCommandKey = "Tessellation";
	m_szLogName = "Tessellation";
	SetTestType(TESTTYPE_CONF);
	m_Options.D3DOptions.fMinDXVersion = 9.f;

	m_Options.D3DOptions.dwBehaviors = DEVICECREATE_HWVP | DEVICECREATE_STANDARD;
    m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_HWVP | DEVICECREATE_STANDARD;

	m_pTexture = NULL;

	m_nViewMatrices = 0;
	m_nWVMatrices = 0;
	m_MaxTess = 0.f;
	if(KeySet(_T("NotAdapTess")))
		m_AdapTess = false;
	else
		m_AdapTess = true;
	m_UseQuinticRT = true;
	
	Basises[0]   = D3DBASIS_BEZIER;       //Linear, Cubic and Quintic    Width = Height = (DWORD)order + 1
	Basises[1]   = D3DBASIS_BSPLINE;      //Cubic                        Width = Height > (DWORD)order
	Basises[2]   = D3DBASIS_CATMULL_ROM;  //Linear, Cubic and Quintic    Width = Height > (DWORD)order
	BasisStr[0]  = "BEZIER";     
	BasisStr[1]  = "BSPLINE";  
	BasisStr[2]  = "CATMULL_ROM";       
	Degrees[0]   = D3DDEGREE_LINEAR;     //1 //3
	Degrees[1]   = D3DDEGREE_QUADRATIC;  //2 //N/A 
	Degrees[2]   = D3DDEGREE_CUBIC;      //3 //10
	Degrees[3]   = D3DDEGREE_QUINTIC;    //5 //21
	DegreeStr[0] = "LINEAR";    
	DegreeStr[1] = "QUADRATIC"; 
	DegreeStr[2] = "CUBIC";     
	DegreeStr[3] = "QUINTIC";

	if( !KeySet(_T("decl-sh" )) && 
		!KeySet(_T("decl-fix")) && 
		!KeySet(_T("fvf-fix" )) && 
		!KeySet(_T("fvf-sh"  ))  )
	{
		m_UseDeclShader = true;
		m_UseFVFShader  = true;
		m_UseFVFFixed   = true;
		m_UseDeclFixed  = true;
	}
	else
	{
		m_UseDeclShader = false;
		m_UseFVFShader  = false;
		m_UseFVFFixed   = false;
		m_UseDeclFixed  = false;
		if(KeySet(_T("decl-sh")))
			m_UseDeclShader = true;
		if(KeySet(_T("decl-fix")))
			m_UseDeclFixed  = true;
		if(KeySet(_T("fvf-fix")))
			m_UseFVFFixed   = true;
		if(KeySet(_T("fvf-sh")))
			m_UseFVFShader  = true;
	}
}

void Tessellation::SetNewMaterial(UINT index)
{
	m_HOMat.diffuse.r = 0.0f;
	m_HOMat.diffuse.g = 0.0f;
	m_HOMat.diffuse.b = 0.0f;
	m_HOMat.diffuse.a = 1.0f;
	m_HOMat.ambient.r = 0.f;
	m_HOMat.ambient.g = 0.f;   
	m_HOMat.ambient.b = 0.f; 
	m_HOMat.specular.r = 0.0f;
	m_HOMat.specular.g = 0.0f;   
	m_HOMat.specular.b = 0.0f;
	switch (index%7)
	{
		case 0: 
		{
            m_HOMat.specular.r = 1.0f;
			m_HOMat.diffuse.r  = 1.0f;
			m_HOMat.ambient.r  = 0.3f;
			break;
		}
		case 1: 
		{
            m_HOMat.specular.g = 1.0f;   
			m_HOMat.diffuse.g  = 1.0f;
			m_HOMat.ambient.g  = 0.3f;
			break;
		}
		case 2: 
		{
            m_HOMat.specular.b = 1.0f;
			m_HOMat.diffuse.b  = 1.0f;
			m_HOMat.ambient.b  = 0.3f;
			break;
		}
		case 3:
		{
			m_HOMat.specular.r = 1.0f;
			m_HOMat.specular.g = 1.0f;   
			m_HOMat.diffuse.r  = 1.0f;
			m_HOMat.diffuse.g  = 1.0f;
			m_HOMat.ambient.r  = 0.3f;
			m_HOMat.ambient.g  = 0.3f;
			break;
		}
		case 4:
		{
			m_HOMat.specular.r = 1.0f;
			m_HOMat.specular.b = 1.0f;
			m_HOMat.diffuse.r  = 1.0f;
			m_HOMat.diffuse.b  = 1.0f;
			m_HOMat.ambient.r  = 0.3f;
			m_HOMat.ambient.b  = 0.3f;
			break;
		}
		case 5:
		{
			m_HOMat.specular.g = 1.0f;   
			m_HOMat.specular.b = 1.0f;
			m_HOMat.diffuse.g  = 1.0f;
			m_HOMat.diffuse.b  = 1.0f;
			m_HOMat.ambient.g  = 0.3f;
			m_HOMat.ambient.b  = 0.3f;
			break;
		}
		case 6:
		{
			m_HOMat.specular.r = 1.0f;
			m_HOMat.specular.g = 1.0f;   
			m_HOMat.specular.b = 1.0f;
			m_HOMat.diffuse.r  = 1.0f;
			m_HOMat.diffuse.g  = 1.0f;
			m_HOMat.diffuse.b  = 1.0f;
			m_HOMat.ambient.r  = 0.3f;
			m_HOMat.ambient.g  = 0.3f;
			m_HOMat.ambient.b  = 0.3f;
			break;
		}
	}
	m_HOMat.power = 10.0f;
	SetMaterial(&m_HOMat);
	float diffuse_color[4] = { m_HOMat.diffuse.r, m_HOMat.diffuse.g, m_HOMat.diffuse.b, 0 };
	if ( !SetVertexShaderConstantF( 4, diffuse_color, 1 ))
		WriteToLog("SetVertexShaderConstantF: Failed to set new diffuse color.\n");
	float specular_color[4] = { m_HOMat.specular.r, m_HOMat.specular.g, m_HOMat.specular.b, 0 };
	if ( !SetVertexShaderConstantF( 5, specular_color, 1 ))
		WriteToLog("SetVertexShaderConstantF: Failed to set new ambient color.\n");
	float ambient_color[4] = { m_HOMat.ambient.r, m_HOMat.ambient.g, m_HOMat.ambient.b, 0 };
	if ( !SetVertexShaderConstantF( 6, ambient_color, 1 ))
		WriteToLog("SetVertexShaderConstantF: Failed to set new ambient color.\n");
}

bool Tessellation::Setup()
{
	//create and set texture
	if ( !CreateTexture(256, 256, 1, 0, FMT_A8R8G8B8, POOL_MANAGED, &m_pTexture) )
	{
		WriteToLog("Tessellation::Setup - CreateTexture failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
		return false;
	}
	if ( !m_pTexture->LoadTexture(0, "tex.bmp") )
	{
		WriteToLog("Tessellation::Setup - LoadTexture failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
		return false;
	}
	if (!KeySet(_T("notex")) && !SetTexture(0, m_pTexture))
	{
		WriteToLog("Tessellation::Setup - SetTexture failed. hr=%s.\n", m_pD3D->HResultToString(GetLastError()));
		return false;
	}
	
	float lightDir[4] = {0.f, 0.0f, -1.0f, 0}; 
	SetVertexShaderConstant( 7, lightDir, 1 );
	
	return (SetDefaultMatrices() && SetDefaultRenderStates() && SetDefaultLights());
}

void Tessellation::Cleanup()
{
	RELEASE(m_pTexture);
}
