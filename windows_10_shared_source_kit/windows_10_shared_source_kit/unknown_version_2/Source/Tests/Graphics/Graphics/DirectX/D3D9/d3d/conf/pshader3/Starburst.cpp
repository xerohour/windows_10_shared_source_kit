#include <stdio.h>
#include <io.h>
#include "PShader.h"
#include "CTextures.h"
#include "CEffect.h"
#include "types.h"

//*******************************************************************************************************

//*******************************************************************************************************

typedef struct
{
    float   x;
    float   y;
    float   z;
} STARVERTEX, *PSTARVERTEX;

//*******************************************************************************************************
                        
CStarburst2_0::CStarburst2_0(LPTSTR str) : CPShaderX()
{
	m_Options.D3DOptions.fMinDXVersion = 9.f; // !!! 8 maybe?
	m_Options.D3DOptions.dwBehaviors  = DEVICECREATE_MIXED | DEVICECREATE_SW_OR_HW_VP;
	m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_MIXED | DEVICECREATE_SW_OR_HW_VP;
	
	m_szCommandKey="Star20";
	m_szTestName="Starburst, Pixel Shader 2.0";
	
	// Exclude all the texture formats. This test doesn't require textures.
	
	m_Options.ModeOptions.TextureFmt = FMT_UNKNOWN;
	m_Options.ModeOptions.dwTexExclude  = FMT_ALL;
   
    m_dwFXCreateFlags = 0;  // Perform optimization, else the HLSL compiled Starburst is too big.
}

//*******************************************************************************************************

bool CStarburst2_0::CapsCheck(void)
{
    if (!CPShaderX::CapsCheck())
        return false;
    
    DWORD dwSupported = m_pSrcDevice->GetCaps()->dwPixelShaderVersion;
    
    // Check major version
    
    if (D3DSHADER_VERSION_MAJOR(dwSupported) < 2)
    {
        WriteToLog(_T("Pixel shader version 2.0 required! Device supports %d.%d!\n"),
                   D3DSHADER_VERSION_MAJOR(dwSupported), D3DSHADER_VERSION_MINOR(dwSupported));
                   
        return false;
    }
/*    
    dwSupported = m_pSrcDevice->GetCaps()->dwVertexShaderVersion;
    
    // Check major version
    
    if (D3DSHADER_VERSION_MAJOR(dwSupported) < 2)
    {
        WriteToLog(_T("Vertex shader version 2.0 required! Device supports %d.%d!\n"),
                   D3DSHADER_VERSION_MAJOR(dwSupported), D3DSHADER_VERSION_MINOR(dwSupported));
                   
        return false;
    }
*/    
    return true;
}

//*******************************************************************************************************

UINT CStarburst2_0::TestInitialize(void)
{
    // Load the .FX file.
    
    LPD3DXBUFFER    pErrors;

    DWORD dwSupported = m_pSrcDevice->GetCaps()->dwVertexShaderVersion;
    
    // Check major version
    
    if (D3DSHADER_VERSION_MAJOR(dwSupported) < 2)
    {
        m_dwFXCreateFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
        if ( FAILED( m_pSrcDevice->SetSoftwareVertexProcessing(TRUE)) )
		{
			WriteToLog("SetSoftwareVertexProcessing() failed.\n");
			return D3DTESTINIT_SKIPALL;
		}
    }        
    
    bool bResult = CreateEffectFromResource(NULL, "Starburst_ps2_0.fx", m_pMacros, &m_Include, m_dwFXCreateFlags, NULL, &m_pEffect, &pErrors);

	if (!bResult)
		return D3DTESTINIT_ABORT;

    SetTestRange(1,40);

    return D3DTESTINIT_RUN;
}

//*******************************************************************************************************

bool CStarburst2_0::ExecuteTest(UINT uTestNumber)
{
    float 		fAspect = 1.33f;
    float		fFOV = pi/4.0f;
    D3DXMATRIX  Projection;
    D3DXMATRIX  View;
    D3DXMATRIX  World;
    D3DXVECTOR3 vCamera(0.0f,0.0f,10.0f);
    D3DXVECTOR3 vAt(0.0f,0.0f,0.0f);
    D3DXVECTOR3 vUp(0.0f,1.0f,0.0f);

    if (uTestNumber > 20)
        BeginTestCase("Starburst 2.0 HLSL Rendering",uTestNumber);
    else
        BeginTestCase("Starburst 2.0 ASM Rendering",uTestNumber);

    // Setup the camera

    D3DXMatrixPerspectiveFovRH(&Projection, fFOV, fAspect, 1.0f, 4000.0f);
    D3DXMatrixLookAtRH(&View,  &vCamera, &vAt, &vUp);
    D3DXMatrixIdentity(&World);

    D3DXMatrixMultiply(&Projection,&View,&Projection);

    m_pEffect->SetValue("Projection",&Projection,sizeof(D3DXMATRIX));

    // .ASM constants

    // HLSL constants
    
    float   TimeVal = (float)uTestNumber;
    
    m_pEffect->SetValue("Time",&TimeVal,sizeof(TimeVal));

    UINT uPasses = 0;

    // Define the plane to render the starburst onto

    STARVERTEX  StarPlane[6] = {   {-10.0f, -10.0f, 0.0f},
                                    {-10.0f, 10.0f, 0.0f},
                                    {10.0f, -10.0f, 0.0f},                                    
                                    {10.0f, -10.0f, 0.0f},
                                    {-10.0f, 10.0f, 0.0f},
                                    {10.0f, 10.0f, 0.0f}};

    
    if (uTestNumber > 20)
        m_pEffect->SetTechniqueByIndex(0);
    else
        m_pEffect->SetTechniqueByIndex(0);

    BeginScene();

    m_pEffect->Begin(&uPasses,0);
    m_pEffect->BeginPass(0);
    
    // Render the scene.

    SetFVF(D3DFVF_XYZ);
    SetRenderState(D3DRS_LIGHTING,FALSE);

    DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, StarPlane, sizeof(STARVERTEX));

    m_pEffect->EndPass();
    m_pEffect->End();

    EndScene();

    return true;
}

//*******************************************************************************************************

void CStarburst2_0::Cleanup(void)
{
    RELEASE(m_pEffect);
    
    CPShaderX::Cleanup();
}
