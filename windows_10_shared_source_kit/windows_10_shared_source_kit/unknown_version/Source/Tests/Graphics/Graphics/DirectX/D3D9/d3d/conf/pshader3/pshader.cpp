// ============================================================================
//
//  pshader.cpp
//  Written by robald.
//
// ============================================================================
#include <stdio.h>
#include <io.h>
#include "PShader.h"
#include "CTextures.h"
#include "CEffect.h"
#include "types.h"

CD3DWindowFramework App;
#ifndef FAST_TEST
CPShader1_x			PixelShader1_x_Test;
CPShader1_x			PixelShader1_x_Test_tex("tex");
CPShader1_x			PixelShader1_x_Test_Cube("cube");
CPShader1_x			PixelShader1_x_Test_Volume("vol");
CPShader2_0			PixelShader2_0_Test;
CPShader2_0			PixelShader2_0_Test_tex("tex");
CPShader2_0			PixelShader2_0_Test_cube("cube");
CPShader2_0			PixelShader2_0_Test_vol("vol");
CLandscape2_0       Landscape2_0;
CStarburst2_0       Starburst2_0;
CPShader2_x			PixelShader2_x_Test;
CPShader2_x			PixelShader2_x_Test_tex("tex");
CPShader2_x			PixelShader2_x_Test_cube("cube");
CPShader2_x			PixelShader2_x_Test_vol("vol");
#endif
CPShader3_0			PixelShader3_0_Test;
CPShader3_0			PixelShader3_0_Test_tex("tex");
CPShader3_0			PixelShader3_0_Test_cube("cube");
CPShader3_0			PixelShader3_0_Test_vol("vol");

#define MAX_STRIPES 8
#define TEXSIZE     32
#define RANGE       127


#ifdef FAST_TEST
#define SHADER_NAME_PREFIX "fast_"
static const FMT g_cTestFormat = FMT_A8R8G8B8;
#else
static const FMT g_cTestFormat = FMT_ALL;
#define SHADER_NAME_PREFIX ""
#endif

// --------------------------------------------------------------------------------------

CPShader2_0::CPShader2_0(LPTSTR str) : CPShaderX()
{
	m_Options.D3DOptions.fMinDXVersion = 9.f; // !!! 8 maybe?

	// Setup macro for pixel shader version:
	
	static D3DXMACRO macros_2_0[] = {{"USING_PS_2_0", ""}, {NULL,NULL}};
	
	m_pMacros = macros_2_0;

	if(!_tcsicmp(str, "tex"))
	{		
		m_szTestName="PShader2.0_Textures";
		m_szCommandKey="ps20_tex";
		m_szPSPrefix = "ps2_0tex";
		m_Options.ModeOptions.dwTexExclude  |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN;// | PF_YUV;
		m_Options.ModeOptions.TextureFmt = g_cTestFormat;
	}
	else if(!_tcsicmp(str, "cube"))
	{
        m_szTestName="PShader2.0_Cubes";
		m_szCommandKey="ps20_cube";
		m_szPSPrefix = "ps2_0cube";
		m_Options.ModeOptions.dwCubeExclude |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN;// | PF_YUV;
		m_Options.ModeOptions.CubeFmt = g_cTestFormat;
	}
	else if(!_tcsicmp(str, "vol"))
	{
		m_szTestName="PShader2.0_Volumes";
		m_szCommandKey="ps20_vol";
		m_szPSPrefix = "ps2_0vol";
		m_Options.ModeOptions.dwVolExclude  |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN;// | PF_YUV;
		m_Options.ModeOptions.VolumeFmt = g_cTestFormat;
	}
	else
	{
		m_szTestName="PShader2.0";
		m_szCommandKey="ps20";
		m_szPSPrefix = "ps2_0_";
	}
}

// ----------------------------------------------------------------------------

CPShader2_x::CPShader2_x(LPTSTR str) : CPShaderX()
{
	m_Options.D3DOptions.fMinDXVersion = 9.f; // !!! 8 maybe?

	// Setup macro for pixel shader version:
	static D3DXMACRO macros_2_x[] = {{"USING_PS_2_X", ""}, {NULL,NULL}};
	m_pMacros = macros_2_x;

	if(!_tcsicmp(str, "tex"))
	{		
		m_szTestName="PShader2x_Textures";
		m_szCommandKey="ps2x_tex";
		m_szPSPrefix = "ps2_xtex";
		m_Options.ModeOptions.dwTexExclude  |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN | PF_YUV;
		m_Options.ModeOptions.TextureFmt = g_cTestFormat;
	}
	else if(!_tcsicmp(str, "cube"))
	{
        m_szTestName="PShader2x_Cubes";
		m_szCommandKey="ps2x_cube";
		m_szPSPrefix = "ps2_xcube";
		m_Options.ModeOptions.dwCubeExclude |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN | PF_YUV;
		m_Options.ModeOptions.CubeFmt = g_cTestFormat;
	}
	else if(!_tcsicmp(str, "vol"))
	{
		m_szTestName="PShader2x_Volumes";
		m_szCommandKey="ps2x_vol";
		m_szPSPrefix = "ps2_xvol";
		m_Options.ModeOptions.dwVolExclude  |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN | PF_YUV;
		m_Options.ModeOptions.VolumeFmt = g_cTestFormat;
	}
	else
	{
		m_szTestName="PShader2x";
		m_szCommandKey="ps2x";
		m_szPSPrefix = "ps2_x_";
	}
}

// ----------------------------------------------------------------------------

CPShader3_0::CPShader3_0(LPTSTR str) : CPShaderX()
{
	m_Options.D3DOptions.fMinDXVersion = 9.f;

	// Setup macro for pixel shader version:
	static D3DXMACRO macros_3_0[] = {{"USING_PS_3_0", ""}, {NULL,NULL}};
	m_pMacros = macros_3_0;

	if(!_tcsicmp(str, "tex"))
	{		
		m_szTestName="PShader3.0_Textures";
		m_szCommandKey="ps30_tex";
		m_szPSPrefix = SHADER_NAME_PREFIX "ps3_0tex";
		m_Options.ModeOptions.dwTexExclude  |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN | PF_YUV;
		m_Options.ModeOptions.TextureFmt = g_cTestFormat;
	}
	else if(!_tcsicmp(str, "cube"))
	{
		m_szTestName="PShader3.0_Cubes";
		m_szCommandKey="ps30_cube";
		m_szPSPrefix = SHADER_NAME_PREFIX "ps3_0cube";
		m_Options.ModeOptions.dwCubeExclude |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN | PF_YUV;
		m_Options.ModeOptions.CubeFmt = g_cTestFormat;
	}
	else if(!_tcsicmp(str, "vol"))
	{
		m_szTestName="PShader3.0_Volumes";
		m_szCommandKey="ps30_vol";
		m_szPSPrefix = SHADER_NAME_PREFIX "ps3_0vol";
		m_Options.ModeOptions.dwVolExclude  |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN | PF_YUV;
		m_Options.ModeOptions.VolumeFmt = g_cTestFormat;
	}
	else
	{
		m_szTestName="PShader3.0";
		m_szCommandKey="ps30";
		m_szPSPrefix = SHADER_NAME_PREFIX "ps3_0_";
	}

}
    
// ----------------------------------------------------------------------------

CPShaderX::CPShaderX() {
    SetTestType(TESTTYPE_CONF);

	m_Options.D3DOptions.dwPSGPRegChange = PSGPREGCHANGE_DISABLE;
	m_Options.D3DOptions.dwRefPSGPRegChange = PSGPREGCHANGE_DISABLE;

	m_nTex2D = m_nTexCube = m_nTexVol = 0;
    // Setup the colors for the Stripes array
    m_Stripes[0] = RGBA_MAKE(255,255,255,255);  // White
    m_Stripes[1] = RGBA_MAKE(255,0,0,255);	    // Red
    m_Stripes[2] = RGBA_MAKE(0,255,0,255);	    // Green
    m_Stripes[3] = RGBA_MAKE(0,0,255,255);	    // Blue
    m_Stripes[4] = RGBA_MAKE(255,255,0,255);	// Yellow
    m_Stripes[5] = RGBA_MAKE(255,0,255,255);	// Magenta
    m_Stripes[6] = RGBA_MAKE(0,255,255,255);	// Cyan
    m_Stripes[7] = RGBA_MAKE(128,128,128,255);  // Grey

    m_nEffectDesc = 0;
    m_nEffectDescAlloc = 0;
    m_pEffectDesc = NULL;
    m_nCurrentEffect = 0;
	m_dwFXCreateFlags = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;

    // Parse the cmdline
    m_bLoadFromFile = KeySet(_T("FromFile"));
    m_bLoadFile = ReadString(_T("Load"), m_sFile, ARRAYSIZE(m_sFile));

    m_Include.SetLoadFromFile(m_bLoadFromFile);
	
    memset(m_pTex2D, 0, sizeof(CnTexture*) * m_nMaxTex2D);
    memset(m_pTexCube, 0, sizeof(CnCubeTexture*) * m_nMaxTexCube);
    memset(m_pTexVol, 0, sizeof(CnVolTexture*) * m_nMaxTexVol);

    m_szPSPrefix = "X"; // should not match any shader filenames

	m_pMacros = NULL;
}

// ----------------------------------------------------------------------------

bool CPShaderX::Setup() 
{
	m_fPrevTolPerPixel = m_pFramework->m_ImageCmp.GetTolPerPixel();
	m_fPrevTolPerPixelChannel = m_pFramework->m_ImageCmp.GetTolPerPixelChannel();
	
	float 		fAspect = 1.33f;
    float		fFOV = pi/4.0f;

    // Set the viewport data
    m_vLoc = D3DXVECTOR3(0.0f, 0.0f, -2.4f);
    m_vAt  = D3DXVECTOR3(0.0f, 0.0f, 10.0f);
    m_vUp  = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    
	D3DXMatrixPerspectiveFovLH(&m_mPerc, fFOV, fAspect, 1.0f, 1000.0f);
	D3DXMatrixOrthoLH(&m_mOrtho, 2.f, 2.f, -3.0f, 3.0f);
    D3DXMatrixLookAtLH(&m_mView,  &m_vLoc, &m_vAt, &m_vUp);
    D3DXMatrixIdentity(&m_mWorld);
    D3DXMatrixMultiply(&m_mTotal[0], &m_mWorld, &m_mView);
    D3DXMatrixMultiply(&m_mTotal[0], &m_mTotal[0], &m_mOrtho);
	D3DXMatrixMultiply(&m_mTotal[1], &m_mWorld, &m_mView);
    D3DXMatrixMultiply(&m_mTotal[1], &m_mTotal[1], &m_mPerc);

    // Set the FixedFunction Pipeline	
    if(!SetTransform(D3DTS_PROJECTION, &m_mOrtho) ||
       !SetTransform(D3DTS_WORLD,      &m_mWorld) ||
       !SetTransform(D3DTS_VIEW,       &m_mView )  )
        return false;

    // Create the geometry
    CShapes	Quad, Sphere, TinyQuad;
    Quad.NewShape(CS_MESH, 3, 3);
    Quad.Scale(2.7f,2.f,2.f);
    Sphere.NewShape(CS_SPHERE, 20, 20);
    Sphere.Scale(2.4f,1.8f,1.8f);
    TinyQuad.NewShape(CS_MESH, 3, 3);
    TinyQuad.Scale(.27f,.2f,.2f);

	VERTEXELEMENT decl[] = 
	{
            {0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
            {0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
            {0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
            DECL_END()
	};
    if(!CreateVertexDeclaration(decl, &(m_QuadInfo.pDecl    )) ||
       !CreateVertexDeclaration(decl, &(m_SphereInfo.pDecl  )) ||
       !CreateVertexDeclaration(decl, &(m_TinyQuadInfo.pDecl))  )
    {
        WriteToLog("CPShaderX::Setup - CreateVertexDeclaration failed.\n");
        return false;
    }
    m_QuadInfo.pVertices = new D3DVERTEX[Quad.m_nVertices];
    m_QuadInfo.pIndices = new WORD[Quad.m_nIndices];
    m_QuadInfo.nVertices = Quad.m_nVertices;
    m_QuadInfo.nIndices = Quad.m_nIndices;
    memcpy( m_QuadInfo.pVertices, Quad.m_pVertices, Quad.m_nVertices*sizeof(D3DVERTEX) );
    memcpy( m_QuadInfo.pIndices, Quad.m_pIndices, Quad.m_nIndices*sizeof(WORD) );
    
	m_SphereInfo.pVertices = new D3DVERTEX[Sphere.m_nVertices];
    m_SphereInfo.pIndices = new WORD[Sphere.m_nIndices];
    m_SphereInfo.nVertices = Sphere.m_nVertices;
    m_SphereInfo.nIndices = Sphere.m_nIndices;
    memcpy( m_SphereInfo.pVertices, Sphere.m_pVertices, Sphere.m_nVertices*sizeof(D3DVERTEX) );
    memcpy( m_SphereInfo.pIndices, Sphere.m_pIndices, Sphere.m_nIndices*sizeof(WORD) );
    
	m_TinyQuadInfo.pVertices = new D3DVERTEX[TinyQuad.m_nVertices];
    m_TinyQuadInfo.pIndices = new WORD[TinyQuad.m_nIndices];
    m_TinyQuadInfo.nVertices = TinyQuad.m_nVertices;
    m_TinyQuadInfo.nIndices = TinyQuad.m_nIndices;
    memcpy( m_TinyQuadInfo.pVertices, TinyQuad.m_pVertices, TinyQuad.m_nVertices*sizeof(D3DVERTEX) );
    memcpy( m_TinyQuadInfo.pIndices, TinyQuad.m_pIndices, TinyQuad.m_nIndices*sizeof(WORD) );

	if ( !CreatePrecisionBuffers() )
    {
        WriteToLog("CPShaderX::Setup - CreatePrecisionBuffers failed.\n");
        return false;
    }

    if ( !CreateTextures() )
    {
        WriteToLog("CPShaderX::Setup - CreateTextures failed.\n");
        return false;
    }
    if ( !CreateCubeTextures() )
    {
        WriteToLog("CPShaderX::Setup - CreateCubeTextures failed.\n");
        return false;
    }
    if ( !CreateVolTextures() )
    {
        WriteToLog("CPShaderX::Setup - CreateVolTextures failed.\n");
        return false;
    }

	/*float v0[4] = {-1.96875, -0.96875, -1.6875,  1.8125}; 
	float v1[4] = {-1.78125,  0.09375, -0.46875, 1.34375}; 
	float v2[4] = {-1.625,   -1.46875,  0.625,  -1.125};

	float lrp[4];
	for(int i = 0; i < 4; i++)
		lrp[i] = v0[i]*(v1[i]-v2[i]) + v2[i];*/

	//GenerateTechniques();
	/*	float a[] = {512, 513, 0.5f};
	int n = 3;
	
	for (int i =0; i < n; i++)
	{
		float x = a[i];
		float y = log(x)/log(2.f);
		float z = floor(abs(y));
        z = z- 15;
		float e = exp((float)z);
		e =e;
	}
*/
	return true;
}

// ----------------------------------------------------------------------------

BOOL CPShaderX::GetEffectDesc(LPTSTR lpszName)
{
    CEffect *pEffect = NULL;
	HRESULT hr = S_OK;
    LPEFFECTDESC pEffectDesc = NULL;
    const UINT nGrow = 16;
    LPD3DXBUFFER pErrors = NULL;
    D3DXEFFECT_DESC FXDesc;

    // Grow the list as needed
    if(m_nEffectDesc + 1 >= m_nEffectDescAlloc)
    {
        pEffectDesc = new EFFECTDESC[m_nEffectDescAlloc + nGrow];
        
        if(NULL == pEffectDesc)
        {
            DPF(1, _T("CPShaderX::GetEffectDesc() - Out of memory.\n"));
            return FALSE;
        }
        
        m_nEffectDescAlloc += nGrow;
        
        memset(pEffectDesc, 0, sizeof(EFFECTDESC) * m_nEffectDescAlloc);

        if(NULL != m_pEffectDesc)
        {
            //memcpy(pEffectDesc, m_pEffectDesc, m_nEffectDesc * sizeof(EFFECTDESC));

			for (UINT i = 0; i < m_nEffectDesc; i++)
			{
				pEffectDesc[ i ].sName = m_pEffectDesc[ i ].sName;
				pEffectDesc[ i ].nTechniques = m_pEffectDesc[ i ].nTechniques;
				
				if ( m_pEffectDesc[ i ].pIndices )
				{
					pEffectDesc[ i ].pIndices = new int[ m_pEffectDesc[ i ].nTechniques ];
					memcpy(pEffectDesc[ i ].pIndices, m_pEffectDesc[ i ].pIndices, m_pEffectDesc[ i ].nTechniques * sizeof(int));
					delete [] m_pEffectDesc[ i ].pIndices;
				}
				else 
					pEffectDesc[ i ].pIndices = NULL;

			}
		}

		SAFEDELETEA(m_pEffectDesc);
        m_pEffectDesc = pEffectDesc;
    }
    
    // The test absolutely crawls when loading all the shader files. Provide a nice update to tell the user what's going on.
    
    TCHAR szStatus[MAX_PATH];
    
    wsprintf(szStatus,"Load: %s",lpszName);
    
    ::SendMessage(m_pFramework->m_pStatusBar->m_hWindow,WM_SETTEXT,0,(LPARAM) szStatus);
    
    // Provide test interactivity
    
    m_pFramework->ProcessMessages();

    // Load the effect and count the techniques
    
    if(m_bLoadFromFile)
        hr = m_pSrcDevice->CreateEffectFromFile(lpszName, m_pMacros, &m_Include, m_dwFXCreateFlags, NULL, &pEffect, &pErrors);
    else
        hr = m_pSrcDevice->CreateEffectFromResource(NULL, lpszName, m_pMacros, &m_Include, m_dwFXCreateFlags, NULL, &pEffect, &pErrors);
        
    if(FAILED(hr))
    {
        DPF(1, _T("CPShaderX::GetEffectDesc() - Failed to load effect(%s).\n"), lpszName);
        if(pErrors)
        {
            DPF(1, _T("CPShaderX::GetEffectDesc() - Compile error: %s.\n"), pErrors->GetBufferPointer());
        }
        
        goto Exit;
    }
    
    pEffect->GetDesc(&FXDesc);

    m_pEffectDesc[m_nEffectDesc].nTechniques = FXDesc.Techniques;

	int *pTempIndices = new int[ FXDesc.Techniques ];
	//m_pEffectDesc[m_nEffectDesc].pIndices    = new int[ FXDesc.Techniques ];

	for (UINT i = 0; i < FXDesc.Techniques; i++)
		pTempIndices[ i ] = i;

	DEVICEDESC *pCaps = m_pSrcDevice->GetCaps();

	//we want to skip all the techniques which depend on caps which are not supported
	if ( !KeySet("show_skips") )
	{
		for (UINT i = 0; i < FXDesc.Techniques; i++)
		{
			if ( NULL != strstr(lpszName, "PS2_X") || NULL != strstr(lpszName, "PS1_X") )
			{
				if (FAILED(pEffect->ValidateTechnique( pEffect->GetTechnique(i) )))
				{
					pTempIndices[ i ] = -1;
					m_pEffectDesc[m_nEffectDesc].nTechniques--;
					continue;
				}
			}

			//ValidateTechnique() doesn't validate sampler states
			if ( !(pCaps->dwRasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS) )
			{
				D3DXHANDLE hAnnot = NULL;
				LPCSTR     sName = NULL;
				
				hAnnot = pEffect->GetAnnotationByName(pEffect->GetTechnique( i ), "Name");
				pEffect->GetString( hAnnot, &sName );

				if ( NULL != strstr(sName, "MipMapLODBias") )
				{
					pTempIndices[ i ] = -1;
					m_pEffectDesc[m_nEffectDesc].nTechniques--;
				}
			}
		}
		m_pFramework->m_DXGLog.SkipTest( FXDesc.Techniques - m_pEffectDesc[m_nEffectDesc].nTechniques );
	}

	if ( m_pEffectDesc[m_nEffectDesc].nTechniques != 0 )
		m_pEffectDesc[ m_nEffectDesc ].pIndices = new int[ m_pEffectDesc[m_nEffectDesc].nTechniques ];
	else 
		m_pEffectDesc[ m_nEffectDesc ].pIndices = NULL;

	UINT j = 0;
	for (UINT i = 0; i < FXDesc.Techniques; i++)
	{
		if ( pTempIndices[ i ] != -1 )
		{
			ASSERT(j < m_pEffectDesc[m_nEffectDesc].nTechniques);
			m_pEffectDesc[ m_nEffectDesc ].pIndices[ j++ ] = i;
		}
	}
	delete [] pTempIndices;

    COPYSTRING(m_pEffectDesc[ m_nEffectDesc ].sName, lpszName);
    m_nEffectDesc++;

  Exit:
    RELEASE(pEffect);
    RELEASE(pErrors);

    return FAILED(hr) ? FALSE : TRUE;
}

// ----------------------------------------------------------------------------

BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam)
{
    CPShaderX *pTest = (CPShaderX*)lParam;
    
    if(_tcsicmp(lpszName, _T("include.fx")))
    {
        // if the glove don't fit, you must acquit
        
        if (0 == _tcsnicmp(pTest->m_szPSPrefix, lpszName, _tcslen(pTest->m_szPSPrefix)))
        {
            return pTest->GetEffectDesc(lpszName);
        }
    }

    return TRUE;
}

// ----------------------------------------------------------------------------

bool CPShaderX::CapsCheck(void) {
    HRESULT hr = S_OK;
    CAPS    D3DRawCapsSrc;
    DWORD   dwMaxPSVersion = 0;

    // Check if caps from 2 GetDeviceCaps() functions are in agreement
    // Source device
    
    hr = m_pD3D->GetDeviceCaps(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), &D3DRawCapsSrc);
    
    if (FAILED(hr))
    {
        WriteToLog("PShader: GetDeviceCaps() call failed\n");
        return false;
    }
    else
    {
        if (D3DRawCapsSrc.PixelShaderVersion != m_pSrcDevice->GetCaps()->dwPixelShaderVersion)
        {
            WriteToLog("PShader: Caps are not consistent on source device\n");
            return false;
        }
    }
	
    dwMaxPSVersion = m_pSrcDevice->GetCaps()->dwPixelShaderVersion;
    
    WriteToLog( _T("Pixel shader version:  %8X\n"), dwMaxPSVersion);
    WriteToLog( _T("Pixel shader version:  %d.%d\n"), D3DSHADER_VERSION_MAJOR(dwMaxPSVersion), D3DSHADER_VERSION_MINOR(dwMaxPSVersion));
                
    if ( FLT_MAX == m_pSrcDevice->GetCaps()->dvMaxPixelShaderValue )
        WriteToLog( _T("Pixel shader max value: FLT_MAX\n"));
    else
        WriteToLog( _T("Pixel shader max value: %f\n"), m_pSrcDevice->GetCaps()->dvMaxPixelShaderValue);

    if (0xFFFF0000 != (dwMaxPSVersion & 0xFFFF0000))
    {
        WriteToLog( _T("ERROR: High word of version (%4X) should be FFFF.\n"), (dwMaxPSVersion >> 16));
    }
    //define some texture related variables (used for texldp)
    
    m_MaxTexRepeat[0] = (float)m_pSrcDevice->GetCaps()->dwMaxTextureRepeat;
    
    if ( m_MaxTexRepeat[0] == 0 )
        m_MaxTexRepeat[0] = 1000.f;
        
	m_MaxTexRepeat[1] = -m_MaxTexRepeat[0];
	m_MaxTexRepeat[2] = 1.f/m_MaxTexRepeat[0];
	m_MaxTexRepeat[3] = 1.f/(4.f*m_pSrcDevice->GetCaps()->dwMaxTextureWidth);

    return true;
}

// ----------------------------------------------------------------------------

UINT CPShaderX::TestInitialize(void)
{
    HRESULT		hr = S_OK;
    UINT		nTests = 0;

	m_bCheckPrecision = false;
	m_uExponentPrecision = 0;
	m_uMantissaPrecision = 0;

	//!!! We don't need this, right?
#if 0
    // Do Version / Cap checking
    if( m_dwVersion < 0x900 )
    {
        WriteToLog(_T("This test requires DX9.\n"));
        SetTestRange(1,1);
        return D3DTESTINIT_SKIPALL;
    }
#endif

    // Check if the cmdline overload is set to load a specific file
    if(m_bLoadFile)
    {
        // Load the file requested from the cmdline
        m_bLoadFromFile = true;
        m_Include.SetLoadFromFile(m_bLoadFromFile);
        if(!GetEffectDesc(m_sFile))
        {
            WriteToLog(_T("Failed to load %s.\n"), m_sFile);
            return D3DTESTINIT_ABORT;
        }
    }
    else
    {
        // Enumerate all the FX files in the resources
        if(!EnumResourceNames(NULL, RT_RCDATA, (ENUMRESNAMEPROCA)EnumResNameProc, (LONG_PTR)this))
        {
            WriteToLog(_T("Failed to enumerate the effects.\n"));
            return D3DTESTINIT_ABORT;
        }
    }

    // Calc the test range
    for(UINT i=0; i<m_nEffectDesc; i++)
        nTests += m_pEffectDesc[i].nTechniques;

    // Set the test range we've calculated
    SetTestRange(1,nTests);

	if ( (m_pCommonTextureFormats && m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == FMT_A8) ||
		 (m_pCommonCubeFormats    && m_pCommonCubeFormats   [m_pCurrentMode->nCubeFormat   ].d3dfFormat == FMT_A8) ||
		 (m_pCommonVolumeFormats  && m_pCommonVolumeFormats [m_pCurrentMode->nVolumeFormat ].d3dfFormat == FMT_A8)  )
	{
		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);

		SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
	}

	return D3DTESTINIT_RUN;
}
// --------------------------------------------------------------------------------------

bool CPShaderX::ClearFrame(void)
{
	if ( (m_pCommonTextureFormats && m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == FMT_A8) ||
		 (m_pCommonCubeFormats    && m_pCommonCubeFormats   [m_pCurrentMode->nCubeFormat   ].d3dfFormat == FMT_A8) ||
		 (m_pCommonVolumeFormats  && m_pCommonVolumeFormats [m_pCurrentMode->nVolumeFormat ].d3dfFormat == FMT_A8)  )
		return Clear(0, NULL, FMWK_DEFAULT, 0xFFffffff, 1.0f, 0);
	else
		return Clear(0, NULL, FMWK_DEFAULT, 0xFF000050, 1.0f, 0);
}

// ----------------------------------------------------------------------------

bool CPShaderX::ExecuteTest(UINT uTestNumber)
{
    HRESULT         hr = S_OK;
    bool			result = true;
	TCHAR           tcsDesc[512];
    LPD3DXBUFFER    pErrors = NULL;
    D3DXHANDLE      hAnnot = NULL;
    UINT            uPasses = 0;
    LPCSTR          sEffectTestCase = NULL, sShape = NULL;
    DECL_INFO      *pShape0_Info = NULL, *pShape1_Info = NULL;
	int             NumIndices = 0;
	int             offset = 0;
	UINT            Stride;
    UINT            nEffect = 0;
    UINT            nTechnique = uTestNumber-1;
	CAPS            caps;
	
	m_pSrcDevice->GetDeviceCaps(&caps); 

    // Decode the test number
    for(UINT i = 0; i < m_nEffectDesc; i++)
    {
        if(nTechnique >= m_pEffectDesc[i].nTechniques)
            nTechnique -= m_pEffectDesc[i].nTechniques;
        else
        {
            nEffect = i;
			nTechnique = m_pEffectDesc[ i ].pIndices[ nTechnique ];
            break;
        }
    }

    // Load the next effect if needed
    if(m_pCurrentEffect == NULL || nEffect != m_nCurrentEffect)
    {
        // Release existing state
        RELEASE(m_pCurrentEffect);

        m_nCurrentEffect = nEffect;

        // Load the effect
		if(m_bLoadFromFile)
			result = CreateEffectFromFile((TCHAR*)m_pEffectDesc[m_nCurrentEffect].sName, m_pMacros, &m_Include, m_dwFXCreateFlags, NULL, &m_pCurrentEffect, &pErrors);
		else
			result = CreateEffectFromResource(NULL, (TCHAR*)m_pEffectDesc[m_nCurrentEffect].sName, m_pMacros, &m_Include, m_dwFXCreateFlags, NULL, &m_pCurrentEffect, &pErrors);
        
        if(!result)
        {
            _stprintf(tcsDesc, _T("Effect %s, Technique %d"), m_pEffectDesc[m_nCurrentEffect].sName, nTechnique);
            BeginTestCase(tcsDesc, uTestNumber);
			if(pErrors)
			{
				DPF(1, _T("ExecuteTest: failed to create effect - Compile error: %s.\n"), pErrors->GetBufferPointer());
				RELEASE (pErrors);
			}
            Fail();
            return false;
        }
    }

	if (!m_pCurrentEffect->SetTechniqueByIndex(nTechnique))
	{ 
		BeginTestCase("Could not extract test name from effect!", uTestNumber);
		WriteToLog(_T("ExecuteTest: SetTechniqueByIndex() failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr);
		Fail(); 
		return false;
	}
	
    hAnnot = m_pCurrentEffect->GetAnnotationByName(m_pCurrentEffect->GetCurrentTechnique(), "Name");
    m_pCurrentEffect->GetString(hAnnot, &sEffectTestCase);

    hAnnot = m_pCurrentEffect->GetAnnotationByName(m_pCurrentEffect->GetCurrentTechnique(), "Shape");
    m_pCurrentEffect->GetString(hAnnot, &sShape);

	//for some cases we increase pixel tolerance
	hAnnot = m_pCurrentEffect->GetAnnotationByName(m_pCurrentEffect->GetCurrentTechnique(), "TolPerPixel");

	m_fPrevTolPerPixel = m_pFramework->m_ImageCmp.GetTolPerPixel();

	if (hAnnot != NULL)
	{
        float  fTolPerPixel = 0.f;

        m_pCurrentEffect->GetFloat(hAnnot, &fTolPerPixel);
		m_pFramework->m_ImageCmp.SetTolPerPixel(fTolPerPixel);
	}

	hAnnot = m_pCurrentEffect->GetAnnotationByName(m_pCurrentEffect->GetCurrentTechnique(), "TolPerPixelPerChannel");

    m_fPrevTolPerPixelChannel = m_pFramework->m_ImageCmp.GetTolPerPixelChannel();

	if (hAnnot != NULL)
	{
        float  fTolPerPixelChannel = 0.f;

        m_pCurrentEffect->GetFloat(hAnnot, &fTolPerPixelChannel);
		m_pFramework->m_ImageCmp.SetTolPerPixelChannel(fTolPerPixelChannel);
	}

	//we want to generate pixel shader ID for each shader. We'll keep two separate constants:
	//c10.x - Test Number
	//c10.y - sum of characters in the string
	LPCSTR tempStr = sEffectTestCase;
	UINT sum = 0;
	float TechniqueID[4] = {(float)uTestNumber, 0.f, 0.f, 0.f}; 
	while (NULL != tempStr && tempStr[0] != '\0')
	{
		sum += tempStr[0];
		tempStr++;
	}
	TechniqueID[1] = (float)sum;
	
	UINT uConst = 10;
	
#pragma warning( disable : 38001 )
	if (lstrcmpi(m_pMacros->Name,"USING_PS_1_X") == 0)
	    uConst = 7;
#pragma warning( default : 38001 )
	    
	SetPixelShaderConstant(uConst, TechniqueID, 1); 

    // Begin the test case
	TCHAR testName[200];
	_stprintf(testName, "(%d) ", sum);
	
	if ( NULL != sEffectTestCase )
		strncat(testName, sEffectTestCase, 200 - strlen(testName));

    BeginTestCase(testName, uTestNumber);

	//if case uses LODBias then need to check if LODBiasing is supported by current adapter
	//unless show_skips command line is set we have already checked for this cap in GetEffectDesc()
	if ( KeySet("show_skips") && NULL != sEffectTestCase )
	{
		if ( !(m_pSrcDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS) )
		{
			if ( NULL != strstr(sEffectTestCase, "MipMapLODBias") )
			{
				WriteToLog( _T("ExecuteTest: D3DPRASTERCAPS_MIPMAPLODBIAS is not set.\n") ); 
				Skip();
				return true;
			}
		}
	}

	if (!m_pCurrentEffect->ValidateTechniqueByIndex(nTechnique))
	{
        WriteToLog(_T("ExecuteTest: ValidateTechniqueByIndex() failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr); 
		Skip();
		return true;
    }

	// Set the params
    if (!m_pCurrentEffect->SetValue("mWorld",        &m_mWorld,       sizeof(D3DXMATRIX))  ||
	    !m_pCurrentEffect->SetValue("fvPosLimits",   &m_vPosLimits,   4*sizeof(float))     ||
        !m_pCurrentEffect->SetValue("fMaxTexRepeat", &m_MaxTexRepeat, 4*sizeof(float))     ||
        !m_pCurrentEffect->SetValue("fTexSizes",     &m_TexSizes,     4*sizeof(float))     ||
	    !m_pCurrentEffect->SetValue("fCubeSizes",    &m_CubeSizes,    4*sizeof(float))     ||
	    !m_pCurrentEffect->SetValue("fVolSizes",     &m_VolSizes,     4*sizeof(float))      )
	{
		WriteToLog(_T("ExecuteTest: SetValue() failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr); 
		Fail(); 
		return false;
    }

	//choose m_mTotal matrix to use based on what shape is used. 
	//If shpere is used then we use percpective projection, otherwise we use orthogonal projection
	if(!_tcsicmp(sShape, _T("sphere")) || KeySet("perc"))
    	m_pCurrentEffect->SetValue("mFinal", &m_mTotal[1], sizeof(D3DXMATRIX));
	else
		m_pCurrentEffect->SetValue("mFinal", &m_mTotal[0], sizeof(D3DXMATRIX));

    for(UINT i=0; i<m_nTex2D; i++)
    {
        _stprintf(tcsDesc, _T("Tex2D_%d"), i);
        if (!m_pCurrentEffect->SetTexture(tcsDesc, m_pTex2D[i]))
		{
			WriteToLog(_T("ExecuteTest: SetTexture() failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr); 
			Fail(); 
			return false;
		}
    }
    for(UINT i=0; i<m_nTexCube; i++)
    {
        _stprintf(tcsDesc, _T("TexCube_%d"), i);
        if (!m_pCurrentEffect->SetCubeTexture(tcsDesc, m_pTexCube[i]))
		{
			WriteToLog(_T("ExecuteTest: SetCubeTexture() failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr); 
			Fail(); 
			return false;
		}
    }
    for(UINT i=0; i<m_nTexVol; i++)
    {
        _stprintf(tcsDesc, _T("TexVol_%d"), i);
        if (!m_pCurrentEffect->SetVolTexture(tcsDesc, m_pTexVol[i]))
		{
			WriteToLog(_T("ExecuteTest: SetVolTexture() failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr); 
			Fail(); 
			return false;
		}
    }
	
    // Render the scene
    if(!m_pCurrentEffect->Begin(&uPasses, 0))
    {
        WriteToLog(_T("ExecuteTest: m_pCurrentEffect->Begin() failed! hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr);
		Fail(); 
		return false;
    }

    BeginScene();

    // Render all passes
    
    for(UINT i=0; i<uPasses; i++) 
	{
        if (!m_pCurrentEffect->BeginPass(i)) 
		{
            WriteToLog(_T("Pass(%d) failed. hr=%s(%x)\n"), i, m_pD3D->HResultToString(GetLastError()), hr); 
			Fail(); 
			break;
        }

		bool bUseIndices = true;

		// choose a shape to use
		
		if(!_tcsicmp(sShape, _T("Precision")))
        {
            pShape0_Info = &m_PrecisionInfo;
            Stride = sizeof(TVERTEX);
			bUseIndices = false;
			m_bCheckPrecision = true;
        }
        else if(!_tcsicmp(sShape, _T("quad")))
        {
            pShape0_Info = &m_QuadInfo;
            Stride = sizeof(D3DVERTEX);
        }
        else if(!_tcsicmp(sShape, _T("sphere")))
        {
            pShape0_Info = &m_SphereInfo;
            Stride = sizeof(D3DVERTEX);
        }
        else if(!_tcsicmp(sShape, _T("tinyquad")))
        {
            pShape0_Info = &m_TinyQuadInfo;
            Stride = sizeof(D3DVERTEX);
        }
		else if(!_tcsnicmp(sShape, _T("textl"), _tcslen("textl")))
        {
			LPSTR temp = NULL;
			temp = _tcsninc(sShape, _tcslen("textl"));
			int l = atoi(temp);

			if (l >= m_pTex2D[0]->GetLevelCount())
			{
                EndScene();
				m_pCurrentEffect->End();
				SkipTestRange(uTestNumber, uTestNumber);
				return true;
			}

			offset = m_TexTLInfo.nIndices*l;
			m_TexTLInfo.pIndices = m_TexTLInfo.pIndices + offset;
			pShape0_Info = &m_TexTLInfo;
			Stride = sizeof(TLVERTEX);
        }
		else if(!_tcsnicmp(sShape, _T("cubetl"), _tcslen("cubetl")))
        {
			LPSTR temp = NULL;
			temp = _tcsninc(sShape, _tcslen("cubetl"));
			int l = atoi(temp);

			if (l >= m_pTexCube[0]->GetLevelCount())
			{
                EndScene();
				m_pCurrentEffect->End();
				SkipTestRange(uTestNumber, uTestNumber);
				return true;
			}

			offset = m_CubeTLInfo.nIndices*l;
			m_CubeTLInfo.pIndices = m_CubeTLInfo.pIndices + offset;
			pShape0_Info = &m_CubeTLInfo;
			Stride = sizeof(TLVOLVERTEX);
        }
		else if(!_tcsnicmp(sShape, _T("voltlz"), _tcslen("voltlz")))
        {
			LPSTR temp = NULL;
			temp = _tcsninc(sShape, _tcslen("voltlz"));
			int l = atoi(temp);

			if (l >= m_pTexVol[0]->GetLevelCount())
			{
                EndScene();
				m_pCurrentEffect->End();
				SkipTestRange(uTestNumber, uTestNumber);
				return true;
			}

			NumIndices = m_VolTLInfo.nIndices;
			offset = m_VolTLInfo.nIndices*l;
			m_VolTLInfo.nIndices = NumIndices-6*2;
			m_VolTLInfo.pIndices = m_VolTLInfo.pIndices + offset;
			pShape0_Info = &m_VolTLInfo;
			Stride = sizeof(TLVOLVERTEX);
        }
		else if(!_tcsnicmp(sShape, _T("voltlx"), _tcslen("voltlx")))
        {
			LPSTR temp = NULL;
			temp = _tcsninc(sShape, _tcslen("voltlx"));
			int l = atoi(temp);

			if (l >= m_pTexVol[0]->GetLevelCount())
			{
                EndScene();
				m_pCurrentEffect->End();
				SkipTestRange(uTestNumber, uTestNumber);
				return true;
			}

			NumIndices = m_VolTLInfo.nIndices;
			offset = m_VolTLInfo.nIndices*(l+1)-12;
			m_VolTLInfo.nIndices = 6;
			m_VolTLInfo.pIndices = m_VolTLInfo.pIndices + offset;
			pShape0_Info = &m_VolTLInfo;
			Stride = sizeof(TLVOLVERTEX);
        }
		else if(!_tcsnicmp(sShape, _T("voltly"), _tcslen("voltly")))
        {
			LPSTR temp = NULL;
			temp = _tcsninc(sShape, _tcslen("voltly"));
			int l = atoi(temp);

			if (l >= m_pTexVol[0]->GetLevelCount())
			{
                EndScene();
				m_pCurrentEffect->End();
				SkipTestRange(uTestNumber, uTestNumber);
				return true;
			}

			NumIndices = m_VolTLInfo.nIndices;
			offset = m_VolTLInfo.nIndices*(l+1)-6;
			m_VolTLInfo.nIndices = 6;
			m_VolTLInfo.pIndices = m_VolTLInfo.pIndices + offset;
			pShape0_Info = &m_VolTLInfo;
			Stride = sizeof(TLVOLVERTEX);
        }
       	//these are only used with texldp which was removed for cube maps
		/*else if ( !_tcsicmp(sShape, _T("Faces01")) )
        {
			NumIndices = m_CubeTLInfo.nIndices;
			m_CubeTLInfo.nIndices = 12;
			pShape0_Info = &m_CubeTLInfo;
			Stride = sizeof(TLVOLVERTEX);
        }
		else if ( !_tcsicmp(sShape, _T("Faces23")) )
        {
			NumIndices = m_CubeTLInfo.nIndices;
			m_CubeTLInfo.nIndices = 12;
			offset = 12;
			m_CubeTLInfo.pIndices = m_CubeTLInfo.pIndices + offset;
			pShape0_Info = &m_CubeTLInfo;
			Stride = sizeof(TLVOLVERTEX);
        }
		else if ( !_tcsicmp(sShape, _T("Faces45")) )
        {
			NumIndices = m_CubeTLInfo.nIndices;
			m_CubeTLInfo.nIndices = 12;
			offset = 24;
			m_CubeTLInfo.pIndices = m_CubeTLInfo.pIndices + offset;
			pShape0_Info = &m_CubeTLInfo;
			Stride = sizeof(TLVOLVERTEX);
        }*/
						
        // Draw the fist shape
        
        if ( pShape0_Info )
        {
            if ( !SetVertexDeclaration(pShape0_Info->pDecl) ) 
            {
                WriteToLog(_T("SetVertexDeclaration failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr);
                Fail();
				break;
            }
            
            if (bUseIndices)
			{
				if( !DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, pShape0_Info->nVertices, pShape0_Info->nIndices / 3,
											pShape0_Info->pIndices, FMT_INDEX16, pShape0_Info->pVertices, Stride) )
				{
					WriteToLog(_T("RenderPrimitive failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr);
					Fail();
					break;
				}
			}
			else
			{
				if( !DrawPrimitiveUP(D3DPT_POINTLIST, pShape0_Info->nVertices, pShape0_Info->pVertices, Stride) )
				{
					WriteToLog(_T("RenderPrimitive failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr);
					Fail();
					break;
				}
			}
        }
		
        // Draw the second shape
        
        if( pShape1_Info )
        {
            if ( !SetVertexDeclaration(pShape1_Info->pDecl) ) 
            {
                WriteToLog(_T("SetVertexDeclaration failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr);
                Fail();
				break;
            }			
            if (bUseIndices)
			{
				if ( !DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, pShape1_Info->nVertices, pShape1_Info->nIndices / 3,
											pShape1_Info->pIndices, FMT_INDEX16, pShape1_Info->pVertices, Stride) )
				{
					WriteToLog(_T("RenderPrimitive failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr);
					Fail();
					break;
				}
			}
			else
			{
				if( !DrawPrimitiveUP(D3DPT_POINTLIST, pShape0_Info->nVertices, pShape0_Info->pVertices, Stride) )
				{
					WriteToLog(_T("RenderPrimitive failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr);
					Fail();
					break;
				}
			}
        }

		if(!_tcsnicmp(sShape, _T("textl"), _tcslen("textl")))
        {
			m_TexTLInfo.pIndices -= offset;
        }
		if(!_tcsnicmp(sShape, _T("cubetl"), _tcslen("cubetl")))
		{
			m_CubeTLInfo.pIndices -= offset;
        }
		if(!_tcsnicmp(sShape, _T("voltl"), _tcslen("voltl")))
        {
			m_VolTLInfo.pIndices -= offset;
			m_VolTLInfo.nIndices = NumIndices;
        }
		/*if(!_tcsnicmp(sShape, _T("faces"), _tcslen("faces")))
		{
			m_CubeTLInfo.pIndices -= offset;
			m_CubeTLInfo.nIndices = NumIndices;
        }*/

		if (!m_pCurrentEffect->EndPass()) 
		{
            WriteToLog(_T("EndPass() failed. hr=%s(%x)\n"), i, m_pD3D->HResultToString(GetLastError()), hr); 
			Fail(); 
			break;
        }
    }
    
    EndScene();
    m_pCurrentEffect->End();
	
    return true;
}

// ----------------------------------------------------------------------------

void CPShaderX::Cleanup(void)
{
	UINT i;
	for(i=0; i<m_nTex2D; i++)
	{
		RELEASE(m_pTex2D[i]);
	}
	m_nTex2D = 0;

	for(i=0; i<m_nTexCube; i++)
	{
		RELEASE(m_pTexCube[i]);
	}
	m_nTexCube = 0;

	for(UINT i=0; i<m_nTexVol; i++)
	{
		RELEASE(m_pTexVol[i]);
	}
	m_nTexVol = 0;

	delete [] m_PrecisionInfo.pVertices;
	m_PrecisionInfo.pVertices = NULL;
	delete [] m_PrecisionInfo.pIndices;
	m_PrecisionInfo.pIndices = NULL;
	RELEASE(m_PrecisionInfo.pDecl);

	delete [] m_QuadInfo.pVertices;
	m_QuadInfo.pVertices = NULL;
	delete [] m_QuadInfo.pIndices;
	m_QuadInfo.pIndices = NULL;
	RELEASE(m_QuadInfo.pDecl);

	delete [] m_SphereInfo.pVertices;
	m_SphereInfo.pVertices = NULL;
	delete [] m_SphereInfo.pIndices;
	m_SphereInfo.pIndices = NULL;
	RELEASE(m_SphereInfo.pDecl);

	delete [] m_TinyQuadInfo.pVertices;
	m_TinyQuadInfo.pVertices = NULL;
	delete [] m_TinyQuadInfo.pIndices;
	m_TinyQuadInfo.pIndices = NULL;
	RELEASE(m_TinyQuadInfo.pDecl);

	delete [] m_TexTLInfo.pVertices;
	m_TexTLInfo.pVertices = NULL;
	delete [] m_TexTLInfo.pIndices;
	m_TexTLInfo.pIndices = NULL;
	RELEASE(m_TexTLInfo.pDecl);

	delete [] m_CubeTLInfo.pVertices;
	m_CubeTLInfo.pVertices = NULL;
	delete [] m_CubeTLInfo.pIndices;
	m_CubeTLInfo.pIndices = NULL;
	RELEASE(m_CubeTLInfo.pDecl);

	delete [] m_VolTLInfo.pVertices;
	m_VolTLInfo.pVertices = NULL;
	delete [] m_VolTLInfo.pIndices;
	m_VolTLInfo.pIndices = NULL;
	RELEASE(m_VolTLInfo.pDecl);
}

// ----------------------------------------------------------------------------

bool CPShaderX::TestTerminate(void)
{
	RELEASE(m_pCurrentEffect);

	for(UINT i = 0; i < m_nEffectDesc; i++)
	{
		if ( m_pEffectDesc[ i ].pIndices )
			delete [] m_pEffectDesc[ i ].pIndices;
        SAFEDELETEA( m_pEffectDesc[ i ].sName );
	}
	
	SAFEDELETEA(m_pEffectDesc);
	m_nEffectDesc = 0;
	m_nEffectDescAlloc = 0;

	return CD3DTest::TestTerminate();
}

// ----------------------------------------------------------------------------

bool CPShaderX::ProcessFrame(void)
{
	CSurface *pSurf1 = NULL;
	HRESULT   hr;
	bool      result = true;

    //we run standard ProcessFrame() for all cases but those which try to determine pixel shader precision

	if (false == m_bCheckPrecision)
	{
		result = CD3DTest::ProcessFrame();
		
		//for some cases we changed pixel tolerance

		if (m_fPrevTolPerPixel != m_pFramework->m_ImageCmp.GetTolPerPixel())
			m_pFramework->m_ImageCmp.SetTolPerPixel(m_fPrevTolPerPixel);

		if (m_fPrevTolPerPixelChannel != m_pFramework->m_ImageCmp.GetTolPerPixelChannel())
			m_pFramework->m_ImageCmp.SetTolPerPixelChannel(m_fPrevTolPerPixelChannel);

		return result;
	}

	hr = m_pSrcDevice->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &pSurf1);

	if(FAILED(hr))
	{
		WriteToLog("ProcessFrame: GetBackBufferCached failed with result hr=%s(%x).\n",	m_pD3D->HResultToString(hr), hr);
		return false;
	}

    int         nPitch1;
    LOCKED_RECT LockRect1;
    void	    *pBuf1;
	DWORD RedMask1   = pSurf1->m_Mask.dwRedMask;
	DWORD GreenMask1 = pSurf1->m_Mask.dwGreenMask;
	DWORD BlueMask1  = pSurf1->m_Mask.dwBlueMask;
	DWORD amount = 0;

    // Get the Surface data pointer for our subrect
    hr = pSurf1->LockRect( &LockRect1, NULL, NULL );
    if (FAILED(hr))
	{
		WriteToLog("CheckPrimitiveColor: LockRect failed with HResult = %s(%x).\n", m_pD3D->HResultToString(hr), hr);
		RELEASE(pSurf1);
        return false;
	}
	pBuf1 = LockRect1.pBits;
    nPitch1 = LockRect1.iPitch;

	DWORD Width  = m_Options.D3DOptions.nBackBufferWidth;
	DWORD Height = m_Options.D3DOptions.nBackBufferHeight;
	DWORD color = RGBA_MAKE(0xff, 0xff, 0, 0xff);
	void *pColor1;
	DWORD c1;

	m_uExponentPrecision = 0;
	m_uMantissaPrecision = 0;

	for (UINT y = 0; y < Height; y++)		// Height loop
	{
		pColor1 = (char*)pBuf1 + (nPitch1 * y);

		UINT x;
		for (x = 0; x < Width; x++)
		{
			c1 = pSurf1->ReadColor(&pColor1);
				
			//if already get exponent then don't need to compare any more
			if (m_uExponentPrecision == 0)
			if ( (RGBA_GETRED(c1) & RedMask1) != (RGBA_GETRED(color) & RedMask1) )
				m_uExponentPrecision = y*Width + x;

			//if already get mantissa then don't need to compare any more
			if (m_uMantissaPrecision == 0)
			if ( (RGBA_GETGREEN(c1) & GreenMask1) != (RGBA_GETGREEN(color) & GreenMask1) )
				m_uMantissaPrecision = y*Width + x;

			if (y*Width + x >= RANGE)
				break;
		}

		if (y*Width + x >= RANGE)
			break;
	}

	hr = pSurf1->UnlockRect();
    if (FAILED(hr))
    {
	    WriteToLog("CheckPrimitiveColor: UnlockRect failed with HResult = %s(%x).\n", m_pD3D->HResultToString(hr), hr);
		RELEASE(pSurf1);
        return false;
    }
	RELEASE(pSurf1);

	UINT i = 1;

	while (m_uExponentPrecision > 0)
	{
		m_uExponentPrecision >>= 1;
		i++;
	}
	m_uExponentPrecision = i;

	WriteToLog("exponent bits = %d\n", m_uExponentPrecision);
	WriteToLog("mantissa bits = %d\n", m_uMantissaPrecision);

	if (m_uExponentPrecision >= 7 && m_uMantissaPrecision >= 16)
		Pass();
	else
		Fail();

	m_bCheckPrecision = false;
	
	return result;
}

// ----------------------------------------------------------------------------

bool CPShaderX::CreatePrecisionBuffers()
{
	DWORD   Pool  = POOL_MANAGED;
	DWORD   Usage = FMWK_DEFAULT;
	
	TVERTEX  *pVB   = NULL;
	PWORD      pIB   = NULL;
	bool       bResult = true;
	
	//create and fill out vertex buffer
	//normals are going to be uses to store variable based on which we'll find out shader precision
	//n.x and n.y are used to figure out shader's exponent precision
	//n.z and n.w are used to figure out shader's mantissa precision
	
	pVB = new TVERTEX[RANGE];
        
    if (NULL == pVB)
		return false;

	float x, y;
	float nx = 1.f, ny = 2.f;
	float epsilon = 0.5f;
	float nz = 1.f, nw = nz + epsilon;
     
    for (UINT i = 0; i < RANGE; i++)
	{
		x = (float)(i%m_pCurrentMode->nBackBufferWidth);
		y = (float)(i/m_pCurrentMode->nBackBufferWidth);
					
		pVB[i] = TVERTEX(D3DXVECTOR3( x, y, 0.f), D3DXVECTOR4(nx, ny, nz, nw));

		nx *= 2.f;
		ny *= 2.f;

		epsilon /= 2.f;
		nw = 1.f + epsilon;
	}

	//create and fill out index buffer used
	pIB = new WORD[3 * RANGE];

    if (NULL == pIB)
		return false;
    	
	for (UINT i = 0; i < RANGE; i++)
	{
		pIB[3*i + 0] = (WORD)i;
		pIB[3*i + 1] = (WORD)i;
		pIB[3*i + 2] = (WORD)i;
	}
    	
    m_PrecisionInfo.nVertices = RANGE;
    m_PrecisionInfo.nIndices = 3*
		RANGE;
    m_PrecisionInfo.pVertices = pVB;
    m_PrecisionInfo.pIndices = pIB;

	VERTEXELEMENT declT[] = 
	{
		{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT4, DECLMETHOD_DEFAULT, DECLUSAGE_POSITIONT, 0},
        {0, 16, (DECLTYPE)D3DDECLTYPE_FLOAT4, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD,  0},
		//{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD,  1},
        DECL_END()
	};
    	
    if(!CreateVertexDeclaration(declT, &(m_PrecisionInfo.pDecl)))
    {
		WriteToLog("CreatePrecisionBuffers: CreateVertexDeclaration() failed.\n");
        return false;
    }

	return true;
}

// ----------------------------------------------------------------------------

bool CPShaderX::CreateTextures()
{
    if (m_uCommonTextureFormats == 0)
        return true;    // No textures;

	FORMAT *pTexFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
	DWORD   Pool  = POOL_MANAGED;
	DWORD   Usage = FMWK_DEFAULT;
	
	CImage   **ppImages = NULL;
	TLVERTEX  *pTexVB   = NULL;
	PWORD      pTexIB   = NULL;
	UINT       uWidth   = TEXSIZE;
	UINT       uHeight  = TEXSIZE;
	UINT       uLevels  = 6;
	UINT	   uStripes = 8;
	CAPS       caps;
	bool       bResult = true;
	
	m_pSrcDevice->GetDeviceCaps(&caps); 

	if (KeySet("nomiptex") || !(m_pSrcDevice->GetCaps()->dwTextureCaps & D3DPTEXTURECAPS_MIPMAP))
		uLevels = 1;

	if ( pTexFormat->d3dfFormat == FMT_A1 )
		uLevels = 1;

	if (m_pD3D->GetRuntimeVersion() >= 9.029f && FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format, USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, *pTexFormat)))
	{
		uLevels = 1;

		for(int i = 0; i < 16; i++)
		{
			SetSamplerState(i, SAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			SetSamplerState(i, SAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		}
	}

	m_TexSizes[0] = (float) uWidth;  //Width  of the primitive
    m_TexSizes[1] = (float) uHeight; //height of the primitive
    m_TexSizes[2] = (float) uWidth;  //Width  of the texture
    m_TexSizes[3] = (float) uHeight; //height of the texture

    //create and fill out vertex buffer used
	
	pTexVB = new TLVERTEX[4*uLevels];
        
    if (NULL == pTexVB)
		return false;
        
    for (UINT level = 0; level < uLevels; level++)
	{
		float x1 = (float)(m_pCurrentMode->nBackBufferWidth/2 - uWidth/(1<<(level+1)));
		float y1 = (float)(m_pCurrentMode->nBackBufferHeight/2 - uHeight/(1<<(level+1)));
		float x2 = (float)(x1 + uWidth/(1<<level));
		float y2 = (float)(y1 + uHeight/(1<<level));
    		
		pTexVB[0+4*level] = TLVERTEX(D3DXVECTOR3( x1, y1, 0.0f), 0xffffffff, D3DXVECTOR4(0.00f, 0.00f, 0.f, 0.f), D3DXVECTOR4(0.99f, 0.99f, 0.f, 1.f), D3DXVECTOR4(0.00f, 0.00f, 0.f, 5.f));
		pTexVB[1+4*level] = TLVERTEX(D3DXVECTOR3( x2, y1, 0.0f), 0xffffffff, D3DXVECTOR4(0.99f, 0.00f, 0.f, 0.f), D3DXVECTOR4(0.00f, 0.99f, 0.f, 1.f), D3DXVECTOR4(0.00f, 0.99f, 0.f, 5.f));
		pTexVB[2+4*level] = TLVERTEX(D3DXVECTOR3( x1, y2, 0.0f), 0xffffffff, D3DXVECTOR4(0.00f, 0.99f, 0.f, 0.f), D3DXVECTOR4(0.99f, 0.00f, 0.f, 1.f), D3DXVECTOR4(0.99f, 0.00f, 0.f, 5.f));
		pTexVB[3+4*level] = TLVERTEX(D3DXVECTOR3( x2, y2, 0.0f), 0xffffffff, D3DXVECTOR4(0.99f, 0.99f, 0.f, 0.f), D3DXVECTOR4(0.00f, 0.00f, 0.f, 1.f), D3DXVECTOR4(0.99f, 0.99f, 0.f, 5.f));
	}

	for(UINT i = 0; i < 4*uLevels; i++)
	{
		pTexVB[i].x -= 0.5f;
		pTexVB[i].y -= 0.5f;
	}

	m_vPosLimits[0] = pTexVB[0].x;
	m_vPosLimits[1] = pTexVB[0].y;
	m_vPosLimits[2] = pTexVB[3].x;
	m_vPosLimits[3] = pTexVB[3].y;
    	
	//create and fill out index buffer used

	pTexIB = new WORD[6*uLevels];

    if (NULL == pTexIB)
		return false;
    	
	for (UINT level = 0; level < uLevels; level++)
	{
		pTexIB[0+6*level] = 0+4*level;
		pTexIB[1+6*level] = 1+4*level; 
		pTexIB[2+6*level] = 3+4*level;
		pTexIB[3+6*level] = 0+4*level;	
		pTexIB[4+6*level] = 3+4*level;
		pTexIB[5+6*level] = 2+4*level;
	}
    	
    m_TexTLInfo.nVertices = 4*uLevels;
    m_TexTLInfo.nIndices = 6;
    m_TexTLInfo.pVertices = pTexVB;
    m_TexTLInfo.pIndices = pTexIB;

	VERTEXELEMENT declTL[] = 
	{
		{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITIONT, 0},
        {0, 16, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_COLOR,    0},
        {0, 20, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
		{0, 36, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 1},
		{0, 52, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 2},
        DECL_END()
	};
    	
    if(!CreateVertexDeclaration(declTL, &(m_TexTLInfo.pDecl)))
    {
		WriteToLog("CreateTextures: CreateVertexDeclaration() failed.\n");
        return false;
    }

	//create images used to fill out texture mip maps

	ppImages = new CImage*[uLevels];
    	
	if (NULL == ppImages)
		return false;

	ZeroMemory(ppImages,sizeof(CImage *) * uLevels);
    		
	for (UINT j = 0; j < uLevels; j++)
	{
		ppImages[j] = new CImage();
    		
		if (ppImages[j] == NULL)
		{
			bResult = false;
			goto Exit;
		}
	}
    	
	// Create the textures
    	
	for (UINT i = 0; i < m_nMaxTex2D; i++)
	{
		uWidth  = TEXSIZE;
		uHeight = TEXSIZE;

		if (i == 2)
			uStripes = 1;
		else
			uStripes = 8;
    		
		if( !CreateTexture(uWidth, uHeight, uLevels, Usage, *pTexFormat, Pool, m_pTex2D+m_nTex2D++) )
		{
			WriteToLog("CreateTextures: CreateTexture() failed on %dth texture.\n", m_nTex2D-1);
			bResult = false;
			goto Exit;
		}
    		
		if(m_pTex2D[m_nTex2D-1]->IsPalettized())
			m_pTex2D[m_nTex2D-1]->SetPalette(NULL);
    			
		for (UINT j = 0; j < uLevels; j++)
		{
			if (strcmp(m_szTestName, "PShader3.0") == 0 && i == 2)
			{
				m_Stripes[0] = RGBA_MAKE(255,  0, 0, 255);  // Red
				m_Stripes[1] = RGBA_MAKE( 0, 255, 0, 255);	// Green
				m_Stripes[2] = RGBA_MAKE( 0, 255, 0, 255);	// Green
				m_Stripes[3] = RGBA_MAKE(255,  0, 0, 255);  // Red
				uStripes = 4;
			}
			else if ( pTexFormat->d3dfFormat == FMT_R16F || pTexFormat->d3dfFormat == FMT_R32F )
			{
				for(UINT k = 0; k < MAX_STRIPES; k++)
					m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(255-25*k, 0, 0, 0xff);
			}
			else if ( pTexFormat->d3dfFormat == FMT_A8 )
			{
				for(UINT k = 0; k < MAX_STRIPES; k++)
					m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(0, 0, 0, 25*k);
			}
			else if ( pTexFormat->ddpfPixelFormat.dwFlags & PF_LUMINANCE )
			{
				for(UINT k = 0; k < MAX_STRIPES; k++)
					m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(0, 255-20*k, 0, 0xff);
			}
			else if ( pTexFormat->d3dfFormat == FMT_UYVY || pTexFormat->d3dfFormat == FMT_YUY2 )
			{
				for(UINT k = 0; k < MAX_STRIPES; k++)
				{
					switch ((i+j)%7)
					{
						case 0:	m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	255-20*k,	255-20*k,	255-20*k); break;
						case 1:	m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	0,			0,			255-20*k); break;
						case 2:	m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(0,			255-20*k,	0,			255-20*k); break;
						case 3:	m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(0,			0,			255-20*k,	255-20*k); break;
						case 4:	m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	255-20*k,	0,			255-20*k); break;
						case 5:	m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	0,			255-20*k,	255-20*k); break;
						case 6:	m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(0,			255-20*k,	255-20*k,	255-20*k); break;
					}
				}
			}
			else
			{
				m_Stripes[(i+j+0)%8] = RGBA_MAKE(255,255,255,255);  // White
				m_Stripes[(i+j+1)%8] = RGBA_MAKE(255,  0,  0,219);	// Red
				m_Stripes[(i+j+2)%8] = RGBA_MAKE(  0,255,  0,183);	// Green
				m_Stripes[(i+j+3)%8] = RGBA_MAKE(  0,  0,255,147);	// Blue
				m_Stripes[(i+j+4)%8] = RGBA_MAKE(255,255,  0,111);	// Yellow
				m_Stripes[(i+j+5)%8] = RGBA_MAKE(255,  0,255, 75);	// Magenta
				m_Stripes[(i+j+6)%8] = RGBA_MAKE(  0,255,255, 39);	// Cyan
				m_Stripes[(i+j+7)%8] = RGBA_MAKE(128,128,128,  0);  // Grey
			 }
    			
			 if (!ppImages[j]->LoadStripes(uWidth, uHeight, uStripes, m_Stripes, ((m_pCurrentMode->nTextureFormat+i+j)%2 || uStripes == 4)?true:false))
			 {
				 bResult = false;
				 goto Exit;
			 }
    			
			 uWidth  = max(1, uWidth  >> 1);
			 uHeight = max(1, uHeight >> 1);
		}
    		
		if( !m_pTex2D[m_nTex2D-1]->LoadImage( 0, ppImages, TEXLOAD_DEFAULT ) )
		{
			bResult = false;
			goto Exit;
		}
    }
    
Exit:
	if (NULL != ppImages)
	for (UINT k = 0; k < uLevels; k++)
		SAFEDELETE( ppImages[k] );

	SAFEDELETEA( ppImages );

	return bResult;
}

// ----------------------------------------------------------------------------

bool CPShaderX::CreateCubeTextures()
{
    if (m_uCommonCubeFormats == 0)
        return true;    // No textures;

	FORMAT *pCubeFormat = &m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat];
	DWORD   Pool  = POOL_MANAGED;
	DWORD   Usage = FMWK_DEFAULT;
	
	CImage    ***pppImages;
	TLVOLVERTEX *pCubeVB = NULL;
	PWORD        pCubeIB = NULL;
	UINT         uWidth = 64;
	UINT         uLevels = 7;
	CAPS         caps;
	bool         bResult = true;

	m_pSrcDevice->GetDeviceCaps(&caps); 

	if (!(caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP))
	{
		WriteToLog("CPShaderX::CreateCubeTextures - cube maps are not supported.\n");
		return true;
	}
	
	if (KeySet("nomipcube") || !(caps.TextureCaps  & D3DPTEXTURECAPS_MIPCUBEMAP))
		uLevels = 1;

	if (m_pD3D->GetRuntimeVersion() >= 9.029f && FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format, USAGE_QUERY_WRAPANDMIP, RTYPE_CUBETEXTURE, *pCubeFormat))) 
	{
		uLevels = 1;

		for(int i = 0; i < 16; i++)
		{
			SetSamplerState(i, SAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			SetSamplerState(i, SAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		}
	}

	m_CubeSizes[0] = (float) uWidth; //width  of the primitive
    m_CubeSizes[1] = (float) uWidth; //height of the primitive
    m_CubeSizes[2] = (float) uWidth; //width  of the texture
    m_CubeSizes[3] = (float) uWidth; //height of the texture
	
	//create and fill out vertex buffer used

	pCubeVB = new TLVOLVERTEX[24*uLevels];

	if (NULL == pCubeVB)
		return false;

	for(UINT level = 0; level < uLevels; level++)
	{
		float x0 = (float)( m_pCurrentMode->nBackBufferWidth/2 - uWidth*1.5f );
		float y0 = (float)( m_pCurrentMode->nBackBufferHeight/2 - uWidth );
		float x1 = x0 + uWidth;
		float y1 = y0 + uWidth;

        float fPrecision = (1.0f / 32.0f) / (float) uWidth;

        // Maximum range of texture coordinate
		
		float fRange = 1.0f - fPrecision;
		float fStart = -1.0f;
		
		//face 0
		pCubeVB[ 0+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fRange,  fRange, 0.f), D3DXVECTOR4(  fRange,  fStart,  fStart, 1.f), D3DXVECTOR4(  fRange,  fRange,  fRange, 6.f));
		pCubeVB[ 1+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fRange,  fStart, 0.f), D3DXVECTOR4(  fRange,  fStart,  fRange, 1.f), D3DXVECTOR4(  fRange,  fStart,  fRange, 6.f));
		pCubeVB[ 2+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fStart,  fRange, 0.f), D3DXVECTOR4(  fRange,  fRange,  fStart, 1.f), D3DXVECTOR4(  fRange,  fRange,  fStart, 6.f));
		pCubeVB[ 3+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fStart,  fStart, 0.f), D3DXVECTOR4(  fRange,  fRange,  fRange, 1.f), D3DXVECTOR4(  fRange,  fStart,  fStart, 6.f));
		
		//face 1
		x0 = x1+4; x1 += uWidth+4;
		pCubeVB[ 4+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y0, 0.f), 0xffffffff, D3DXVECTOR4(  -1.0f,  fRange,  fStart, 0.f), D3DXVECTOR4(  -1.0f,  fStart,  fRange, 1.f), D3DXVECTOR4(  -1.0f,  fStart,  fRange, 6.f));
		pCubeVB[ 5+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y0, 0.f), 0xffffffff, D3DXVECTOR4(  -1.0f,  fRange,  fRange, 0.f), D3DXVECTOR4(  -1.0f,  fStart,  fStart, 1.f), D3DXVECTOR4(  -1.0f,  fRange,  fRange, 6.f));
		pCubeVB[ 6+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y1, 0.f), 0xffffffff, D3DXVECTOR4(  -1.0f,  fStart,  fStart, 0.f), D3DXVECTOR4(  -1.0f,  fRange,  fRange, 1.f), D3DXVECTOR4(  -1.0f,  fStart,  fStart, 6.f));
		pCubeVB[ 7+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y1, 0.f), 0xffffffff, D3DXVECTOR4(  -1.0f,  fStart,  fRange, 0.f), D3DXVECTOR4(  -1.0f,  fRange,  fStart, 1.f), D3DXVECTOR4(  -1.0f,  fRange,  fStart, 6.f));
		
		//face 2
		x0 = x1+4; x1 += uWidth+4;
		pCubeVB[ 8+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fStart,  fRange,  fStart, 0.f), D3DXVECTOR4(  fRange,  fRange,  fRange, 1.f), D3DXVECTOR4(  fStart,  fRange,  fStart, 6.f));
		pCubeVB[ 9+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fRange,  fStart, 0.f), D3DXVECTOR4(  fStart,  fRange,  fRange, 1.f), D3DXVECTOR4(  fStart,  fRange,  fRange, 6.f));
		pCubeVB[10+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fStart,  fRange,  fRange, 0.f), D3DXVECTOR4(  fRange,  fRange,  fStart, 1.f), D3DXVECTOR4(  fRange,  fRange,  fStart, 6.f));
		pCubeVB[11+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fRange,  fRange, 0.f), D3DXVECTOR4(  fStart,  fRange,  fStart, 1.f), D3DXVECTOR4(  fRange,  fRange,  fRange, 6.f));
		
		//face 3
		x0 = (float)( m_pCurrentMode->nBackBufferWidth/2 - 1.5f*uWidth ); 
		x1 = x0 + uWidth;
		y0 = y1+4; y1 += uWidth+4;
		pCubeVB[12+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fStart,  -1.0f,  fRange, 0.f), D3DXVECTOR4(  fRange,  -1.0f,  fStart, 1.f), D3DXVECTOR4(  fRange,  -1.0f,  fStart, 6.f));
		pCubeVB[13+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  -1.0f,  fRange, 0.f), D3DXVECTOR4(  fStart,  -1.0f,  fStart, 1.f), D3DXVECTOR4(  fRange,  -1.0f,  fRange, 6.f));
		pCubeVB[14+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fStart,  -1.0f,  fStart, 0.f), D3DXVECTOR4(  fRange,  -1.0f,  fRange, 1.f), D3DXVECTOR4(  fStart,  -1.0f,  fStart, 6.f));
		pCubeVB[15+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  -1.0f,  fStart, 0.f), D3DXVECTOR4(  fStart,  -1.0f,  fRange, 1.f), D3DXVECTOR4(  fStart,  -1.0f,  fRange, 6.f));
		
		//face 4
		x0 = x1+4; x1 += uWidth+4;
		pCubeVB[16+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fStart,  fRange,  fRange, 0.f), D3DXVECTOR4(  fRange,  fStart,  fRange, 1.f), D3DXVECTOR4(  fRange,  fStart,  fRange, 6.f));
		pCubeVB[17+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fRange,  fRange, 0.f), D3DXVECTOR4(  fStart,  fStart,  fRange, 1.f), D3DXVECTOR4(  fRange,  fRange,  fRange, 6.f));
		pCubeVB[18+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fStart,  fStart,  fRange, 0.f), D3DXVECTOR4(  fRange,  fRange,  fRange, 1.f), D3DXVECTOR4(  fStart,  fStart,  fRange, 6.f));
		pCubeVB[19+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fStart,  fRange, 0.f), D3DXVECTOR4(  fStart,  fRange,  fRange, 1.f), D3DXVECTOR4(  fStart,  fRange,  fRange, 6.f));
		
		//face 5
		x0 = x1+4; x1 += uWidth+4;
		pCubeVB[20+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fRange,  -1.0f, 0.f), D3DXVECTOR4(  fStart,  fStart,  -1.0f, 1.f), D3DXVECTOR4(  fRange,  fRange,  -1.0f, 6.f));
		pCubeVB[21+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fStart,  fRange,  -1.0f, 0.f), D3DXVECTOR4(  fRange,  fStart,  -1.0f, 1.f), D3DXVECTOR4(  fRange,  fStart,  -1.0f, 6.f));
		pCubeVB[22+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fStart,  -1.0f, 0.f), D3DXVECTOR4(  fStart,  fRange,  -1.0f, 1.f), D3DXVECTOR4(  fStart,  fRange,  -1.0f, 6.f));
		pCubeVB[23+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fStart,  fStart,  -1.0f, 0.f), D3DXVECTOR4(  fRange,  fRange,  -1.0f, 1.f), D3DXVECTOR4(  fStart,  fStart,  -1.0f, 6.f));
		uWidth >>= 1;
	}
	
	for(UINT i = 0; i < 24*uLevels; i++)
	{
		pCubeVB[i].x -= 0.5f;
		pCubeVB[i].y -= 0.5f;
	}

	//create and fill out index buffer used
	
	pCubeIB = new WORD[36*uLevels];

	if (NULL == pCubeIB)
		return false;
	
	for(UINT l = 0; l < uLevels; l++)
    for(UINT i = 0; i < 6;       i++)
	{
		pCubeIB[i*6+0+36*l]=i*4+0+24*l;
		pCubeIB[i*6+1+36*l]=i*4+1+24*l;
		pCubeIB[i*6+2+36*l]=i*4+3+24*l;
		pCubeIB[i*6+3+36*l]=i*4+0+24*l;
		pCubeIB[i*6+4+36*l]=i*4+3+24*l;
		pCubeIB[i*6+5+36*l]=i*4+2+24*l;
	}

	m_CubeTLInfo.nVertices = 24*uLevels;
    m_CubeTLInfo.nIndices = 36;
    m_CubeTLInfo.pVertices = pCubeVB;
	m_CubeTLInfo.pIndices = pCubeIB;
	
	VERTEXELEMENT declTL[] = 
	{
            {0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITIONT, 0},
            {0, 16, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_COLOR,    0},
            {0, 20, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD,  0},
			{0, 36, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD,  1},
			{0, 52, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD,  2},
            DECL_END()
	};
	
    if(!CreateVertexDeclaration(declTL, &(m_CubeTLInfo.pDecl)))
    {
		WriteToLog("CreateCubeTextures: CreateVertexDeclaration() failed.\n");
        return false;
    }

	//create images used to fill out texture mip maps

	pppImages = new CImage**[uLevels];
	
	if (pppImages == NULL)
		return false;

	ZeroMemory(pppImages, sizeof(CImage **) * uLevels);

	for (UINT j = 0; j < uLevels; j++)
	{
		pppImages[j] = new CImage*[6];
		
		if (pppImages[j] == NULL)
		{
			bResult = false;
			goto Exit;
		}

		ZeroMemory(pppImages[j], sizeof(CImage *) * 6);
		
		for (UINT jj = 0; jj < 6; jj++)
		{
			pppImages[j][jj] = new CImage();

			if ( pppImages[j][jj] == NULL )
			{
				bResult = false;
				goto Exit;
			}
		}
	}
	
	// Create the cube textures
	
	for (UINT i = 0; i < m_nMaxTexCube; i++)
	{
		uWidth =  (UINT) m_CubeSizes[2];
		
		if( !CreateCubeTexture(uWidth, uLevels, Usage, *pCubeFormat, Pool, m_pTexCube+m_nTexCube++) )
		{
			WriteToLog("CreateCubeTextures: CreateCubeTexture() failed on %dth cube texture.\n", m_nTexCube-1);
			bResult = false;
			goto Exit;
		}
		
		if(m_pTexCube[m_nTexCube-1]->IsPalettized())
			m_pTexCube[m_nTexCube-1]->SetPalette(NULL);
			
		for (UINT j = 0; j < uLevels; j++)
		{
			for (UINT jj = 0; jj < 6; jj++)
			{
				if ( pCubeFormat->d3dfFormat == FMT_R16F || pCubeFormat->d3dfFormat == FMT_R32F )
				{
					for(UINT k = 0; k < MAX_STRIPES; k++)
						m_Stripes[(1+i+j+jj+k)%MAX_STRIPES] = RGBA_MAKE(255-25*k, 0, 0, 0xff);
				}
				else if ( pCubeFormat->d3dfFormat == FMT_A8 )
				{
					for(UINT k = 0; k < MAX_STRIPES; k++)
						m_Stripes[(1+i+j+jj+k)%MAX_STRIPES] = RGBA_MAKE(0, 0, 0, 25*k);
				}
				else if ( pCubeFormat->ddpfPixelFormat.dwFlags & PF_LUMINANCE )
				{
					for(UINT k = 0; k < MAX_STRIPES; k++)
						m_Stripes[(1+i+j+jj+k)%MAX_STRIPES] = RGBA_MAKE(0, 255-20*k, 0, 0xff);
				}
				else if ( pCubeFormat->d3dfFormat == FMT_UYVY || pCubeFormat->d3dfFormat == FMT_YUY2 )
				{
					for(UINT k = 0; k < MAX_STRIPES; k++)
					{
						switch ((1+i+j+jj)%7)
						{
							case 0:	m_Stripes[(1+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	255-20*k,	255-20*k,	255-20*k); break;
							case 1:	m_Stripes[(1+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	0,			0,			255-20*k); break;
							case 2:	m_Stripes[(1+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(0,		255-20*k,	0,			255-20*k); break;
							case 3:	m_Stripes[(1+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(0,		0,			255-20*k,	255-20*k); break;
							case 4:	m_Stripes[(1+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	255-20*k,	0,			255-20*k); break;
							case 5:	m_Stripes[(1+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	0,			255-20*k,	255-20*k); break;
							case 6:	m_Stripes[(1+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(0,		255-20*k,	255-20*k,	255-20*k); break;
						}
					}
				}
				else
				{
					m_Stripes[(1+i+j+jj+0)%8] = RGBA_MAKE(255,255,255,255);  // White
					m_Stripes[(1+i+j+jj+1)%8] = RGBA_MAKE(255,  0,  0,225);  // Red
					m_Stripes[(1+i+j+jj+2)%8] = RGBA_MAKE(  0,255,  0,200);	 // Green
					m_Stripes[(1+i+j+jj+3)%8] = RGBA_MAKE(  0,  0,255,175);  // Blue
					m_Stripes[(1+i+j+jj+4)%8] = RGBA_MAKE(255,255,  0,150);	 // Yellow
					m_Stripes[(1+i+j+jj+5)%8] = RGBA_MAKE(255,  0,255,125);	 // Magenta
					m_Stripes[(1+i+j+jj+6)%8] = RGBA_MAKE(  0,255,255,100);	 // Cyan
					m_Stripes[(1+i+j+jj+7)%8] = RGBA_MAKE(128,128,128, 75);  // Grey
				}

				if (!pppImages[j][jj]->LoadStripes(uWidth, uWidth, i==0 ? 8 : 1, m_Stripes, (m_pCurrentMode->nCubeFormat+j+jj)%2?true:false))
				{
					bResult = false;
					goto Exit;
				}
			}
			uWidth  = max(1, uWidth >> 1);
		}

		if( !m_pTexCube[m_nTexCube-1]->LoadImage( pppImages, TEXLOAD_DEFAULT ) )
		{
			bResult = false;
			goto Exit;
		}
	}

Exit:	
	if (NULL != pppImages)
	for (UINT k = 0; k < uLevels; k++)
	{
		if (NULL != pppImages[k])
		for (UINT kk = 0; kk < 6; kk++)
			SAFEDELETE(pppImages[k][kk]);

		SAFEDELETEA(pppImages[k]);
	}
	SAFEDELETEA( pppImages );

	return bResult;
}

// ----------------------------------------------------------------------------

bool CPShaderX::CreateVolTextures()
{
    if (m_uCommonVolumeFormats == 0)
        return true;    // No textures;

	FORMAT *pVolFormat = &m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat];
	DWORD Pool  = POOL_MANAGED;
	DWORD Usage = FMWK_DEFAULT;

	CImage     ***pppImages = NULL;
	TLVOLVERTEX  *pVolVB    = NULL;
	PWORD         pVolIB    = NULL;
	UINT          uWidth    = TEXSIZE;
	UINT          uHeight   = TEXSIZE;
	UINT          uSlices   = 16;
	UINT          uLevels   = 6;
	CAPS          caps;
	bool          bResult = true;

	m_pSrcDevice->GetDeviceCaps(&caps);

	if (!(caps.TextureCaps  & D3DPTEXTURECAPS_VOLUMEMAP))
	{
		WriteToLog("CPShaderX::CreateVolTextures - volumes are not supported.\n");
		return true;
	}

	if (KeySet("nomipvol") || !(caps.TextureCaps  & D3DPTEXTURECAPS_MIPVOLUMEMAP))
		uLevels = 1;

	if (m_pD3D->GetRuntimeVersion() >= 9.029f && FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format, USAGE_QUERY_WRAPANDMIP, RTYPE_VOLUMETEXTURE, *pVolFormat)))
	{
		uLevels = 1;

		for(int i = 0; i < 16; i++)
		{
			SetSamplerState(i, SAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			SetSamplerState(i, SAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			SetSamplerState(i, SAMP_ADDRESSW, D3DTADDRESS_CLAMP);
		}
	}

	m_VolSizes[0] = (float) uWidth;  //width  of the primitive
    m_VolSizes[1] = (float) uHeight; //height of the primitive
    m_VolSizes[2] = (float) uWidth;  //width  of the texture
    m_VolSizes[3] = (float) uHeight; //height of the texture

	//create and fill out vertex buffer used
	
	pVolVB = new TLVOLVERTEX[(uSlices+2)*4*uLevels];

	if (NULL == pVolVB)
		return false;

	for (UINT l = 0; l < uLevels; l++)
	{
		float w = 1.f/(2.f*uSlices);
		int row = (int) sqrt((float)uSlices);
		float x0 = m_pCurrentMode->nBackBufferWidth/2 - m_VolSizes[2]*(row>>1)/(1<<l);
		float y0 = m_pCurrentMode->nBackBufferHeight/2 - m_VolSizes[3]*((row>>1)+1)/(1<<l);
		float x1 = x0 + m_VolSizes[2]/(1<<l);
		float y1 = y0 + m_VolSizes[3]/(1<<l);

		//front view
		for(UINT i = 0; i < uSlices; i++)
		{
			if (i%row)
			{
				x0 = x1 + 3.f; 
				x1 += m_VolSizes[2]/(1<<l) + 3.f;
			}
			else
			{	
				x0 = m_pCurrentMode->nBackBufferWidth/2 - m_VolSizes[2]*(row>>1)/(1<<l); 
				x1 = x0 + m_VolSizes[2]/(1<<l);
				y0 = y1 + 3.f;
				y1 += m_VolSizes[3]/(1<<l) + 3.f; 
			}

			pVolVB[(uSlices+2)*4*l+i*4+0] = TLVOLVERTEX(D3DXVECTOR3( x0, y0, 0.f), 0xffffffff, D3DXVECTOR4( 0.00f, 0.00f, w, 0.f), D3DXVECTOR4( 0.99f, 0.99f, w, 1.f), D3DXVECTOR4( 0.00f, 0.00f, w, 5.f));
			pVolVB[(uSlices+2)*4*l+i*4+1] = TLVOLVERTEX(D3DXVECTOR3( x1, y0, 0.f), 0xffffffff, D3DXVECTOR4( 0.99f, 0.00f, w, 0.f), D3DXVECTOR4( 0.00f, 0.99f, w, 1.f), D3DXVECTOR4( 0.00f, 0.99f, w, 5.f));
			pVolVB[(uSlices+2)*4*l+i*4+2] = TLVOLVERTEX(D3DXVECTOR3( x0, y1, 0.f), 0xffffffff, D3DXVECTOR4( 0.00f, 0.99f, w, 0.f), D3DXVECTOR4( 0.99f, 0.00f, w, 1.f), D3DXVECTOR4( 0.99f, 0.00f, w, 5.f));
			pVolVB[(uSlices+2)*4*l+i*4+3] = TLVOLVERTEX(D3DXVECTOR3( x1, y1, 0.f), 0xffffffff, D3DXVECTOR4( 0.99f, 0.99f, w, 0.f), D3DXVECTOR4( 0.00f, 0.00f, w, 1.f), D3DXVECTOR4( 0.99f, 0.99f, w, 5.f));
			w += 1.f/uSlices;
		}

		//side views
		x0 = m_pCurrentMode->nBackBufferWidth/2 - m_VolSizes[2]/(1<<(l+1));
		y0 = m_pCurrentMode->nBackBufferHeight/2 - m_VolSizes[3]/(1<<(l+1));
		x1 = x0 + m_VolSizes[2]/(1<<l); 
		y1 = y0 + m_VolSizes[3]/(1<<l);
		//need to get integer values to later offset it by 0.5f
		//which helps to match texturing between REF and HW
		x0 = (float)((int)x0);
		x1 = (float)((int)x1);
        y0 = (float)((int)y0);
		y1 = (float)((int)y1);

		pVolVB[(uSlices+2)*4*l+uSlices*4+0] = TLVOLVERTEX(D3DXVECTOR3( x0, y0, 0.f), 0xffffffff, D3DXVECTOR4( 0.00f, 0.00f, 0.00f, 0.f), D3DXVECTOR4( 0.00f, 0.99f, 0.99f, 1.f), D3DXVECTOR4( 0.00f, 0.00f, 0.00f, 5.f));
		pVolVB[(uSlices+2)*4*l+uSlices*4+1] = TLVOLVERTEX(D3DXVECTOR3( x1, y0, 0.f), 0xffffffff, D3DXVECTOR4( 0.00f, 0.00f, 0.99f, 0.f), D3DXVECTOR4( 0.00f, 0.99f, 0.00f, 1.f), D3DXVECTOR4( 0.00f, 0.99f, 0.00f, 5.f));
		pVolVB[(uSlices+2)*4*l+uSlices*4+2] = TLVOLVERTEX(D3DXVECTOR3( x0, y1, 0.f), 0xffffffff, D3DXVECTOR4( 0.00f, 0.99f, 0.00f, 0.f), D3DXVECTOR4( 0.00f, 0.00f, 0.99f, 1.f), D3DXVECTOR4( 0.00f, 0.00f, 0.99f, 5.f));
		pVolVB[(uSlices+2)*4*l+uSlices*4+3] = TLVOLVERTEX(D3DXVECTOR3( x1, y1, 0.f), 0xffffffff, D3DXVECTOR4( 0.00f, 0.99f, 0.99f, 0.f), D3DXVECTOR4( 0.00f, 0.00f, 0.00f, 1.f), D3DXVECTOR4( 0.00f, 0.99f, 0.99f, 5.f));

		pVolVB[(uSlices+2)*4*l+uSlices*4+4] = TLVOLVERTEX(D3DXVECTOR3( x0, y0, 0.f), 0xffffffff, D3DXVECTOR4( 0.00f, 0.00f, 0.00f, 0.f), D3DXVECTOR4( 0.99f, 0.00f, 0.99f, 1.f), D3DXVECTOR4( 0.00f, 0.00f, 0.00f, 5.f));
		pVolVB[(uSlices+2)*4*l+uSlices*4+5] = TLVOLVERTEX(D3DXVECTOR3( x1, y0, 0.f), 0xffffffff, D3DXVECTOR4( 0.00f, 0.00f, 0.99f, 0.f), D3DXVECTOR4( 0.99f, 0.00f, 0.00f, 1.f), D3DXVECTOR4( 0.99f, 0.00f, 0.00f, 5.f));
		pVolVB[(uSlices+2)*4*l+uSlices*4+6] = TLVOLVERTEX(D3DXVECTOR3( x0, y1, 0.f), 0xffffffff, D3DXVECTOR4( 0.99f, 0.00f, 0.00f, 0.f), D3DXVECTOR4( 0.00f, 0.00f, 0.99f, 1.f), D3DXVECTOR4( 0.00f, 0.00f, 0.99f, 5.f));
		pVolVB[(uSlices+2)*4*l+uSlices*4+7] = TLVOLVERTEX(D3DXVECTOR3( x1, y1, 0.f), 0xffffffff, D3DXVECTOR4( 0.99f, 0.00f, 0.99f, 0.f), D3DXVECTOR4( 0.00f, 0.00f, 0.00f, 1.f), D3DXVECTOR4( 0.99f, 0.00f, 0.99f, 5.f));
	}
	for(UINT i = 0; i < (uSlices+2)*4*uLevels; i++)
	{
		pVolVB[i].x -= 0.5f;
		pVolVB[i].y -= 0.5f;
	}

	//create and fill out index buffer used
	
	pVolIB = new WORD[(uSlices+2)*6*uLevels];

	if (NULL == pVolIB)
		return false;

	for (UINT l = 0; l < uLevels;   l++)
	for (UINT i = 0; i < uSlices+2; i++)
	{
		pVolIB[(uSlices+2)*6*l+i*6+0] = (uSlices+2)*4*l+i*4+0;
		pVolIB[(uSlices+2)*6*l+i*6+1] = (uSlices+2)*4*l+i*4+1;
		pVolIB[(uSlices+2)*6*l+i*6+2] = (uSlices+2)*4*l+i*4+3;
		pVolIB[(uSlices+2)*6*l+i*6+3] = (uSlices+2)*4*l+i*4+0;
		pVolIB[(uSlices+2)*6*l+i*6+4] = (uSlices+2)*4*l+i*4+3;
		pVolIB[(uSlices+2)*6*l+i*6+5] = (uSlices+2)*4*l+i*4+2;
	}

	m_VolTLInfo.nVertices = (uSlices+2)*4*uLevels;
	m_VolTLInfo.nIndices  = (uSlices+2)*6;
	m_VolTLInfo.pVertices = pVolVB;
	m_VolTLInfo.pIndices  = pVolIB;

	VERTEXELEMENT declTL[] = 
	{
            {0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITIONT, 0},
            {0, 16, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_COLOR,     0},
            {0, 20, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD,  0},
			{0, 36, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD,  1},
			{0, 52, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD,  2},
            DECL_END()
	};
	if(!CreateVertexDeclaration(declTL, &(m_VolTLInfo.pDecl)))
	{
		WriteToLog("CreateVolTextures: CreateVertexDeclaration() failed.\n");
		return false;
	}

	//create images used to fill out texture mip maps

	pppImages = new CImage**[uLevels];

	if (pppImages == NULL)
		return false;

	ZeroMemory(pppImages, sizeof(CImage **) * uLevels);

	UINT uDepth = uSlices;
	for (UINT j = 0; j < uLevels; j++)
	{
		pppImages[j] = new CImage*[uDepth];

		if (pppImages[j] == NULL)
		{
			bResult = false;
			goto Exit;
		}

		ZeroMemory(pppImages[j], sizeof(CImage **) * uDepth);

		for (UINT jj = 0; jj < uDepth; jj++)
		{
			pppImages[j][jj] = new CImage();

			if ( pppImages[j][jj] == NULL )
			{
				bResult = false;
				goto Exit;
			}
		}
		uDepth = max( 1, uDepth>>1 );
	}

	// Create the volume textures

	for (UINT i = 0; i < m_nMaxTexVol; i++)
	{
		uWidth  = TEXSIZE;
		uHeight = TEXSIZE;
		uDepth  = uSlices;
		if( !CreateVolumeTexture(uWidth, uHeight, uDepth, uLevels, Usage, *pVolFormat, Pool, m_pTexVol+m_nTexVol++) )
		{
			WriteToLog("CreateVolTextures: CreateVolumeTexture() failed on %dth texture.\n", m_nTexVol-1);
			bResult = false;
			goto Exit;
		}

		if(m_pTexVol[m_nTexVol-1]->IsPalettized())
			m_pTexVol[m_nTexVol-1]->SetPalette(NULL);

		for (UINT j = 0; j < uLevels; j++)
		{
			for (UINT jj = 0; jj < uDepth; jj++)
			{
				if ( pVolFormat->d3dfFormat == FMT_R16F || pVolFormat->d3dfFormat == FMT_R32F )
				{
					for(UINT k = 0; k < MAX_STRIPES; k++)
						m_Stripes[(2+i+j+jj/2+k)%MAX_STRIPES] = RGBA_MAKE(255-25*k, 0, 0, 0xff);
				}
				else if ( pVolFormat->d3dfFormat == FMT_A8 )
				{
					for(UINT k = 0; k < MAX_STRIPES; k++)
						m_Stripes[(2+i+j+jj/2+k)%MAX_STRIPES] = RGBA_MAKE(0, 0, 0, 25*k);
				}
				else if ( pVolFormat->ddpfPixelFormat.dwFlags & PF_LUMINANCE )
				{
					for(UINT k = 0; k < MAX_STRIPES; k++)
						m_Stripes[(2+i+j+jj/2+k)%MAX_STRIPES] = RGBA_MAKE(0, 255-20*k, 0, 0xff);
				}
				else if ( pVolFormat->d3dfFormat == FMT_UYVY || pVolFormat->d3dfFormat == FMT_YUY2 )
				{
					for(UINT k = 0; k < MAX_STRIPES; k++)
					{
						switch ((2+i+j+jj)%7)
						{
							case 0:	m_Stripes[(2+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	255-20*k,	255-20*k,	255-20*k); break;
							case 1:	m_Stripes[(2+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	0,			0,			255-20*k); break;
							case 2:	m_Stripes[(2+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(0,			255-20*k,	0,			255-20*k); break;
							case 3:	m_Stripes[(2+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(0,			0,			255-20*k,	255-20*k); break;
							case 4:	m_Stripes[(2+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	255-20*k,	0,			255-20*k); break;
							case 5:	m_Stripes[(2+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	0,			255-20*k,	255-20*k); break;
							case 6:	m_Stripes[(2+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(0,			255-20*k,	255-20*k,	255-20*k); break;
						}
					}
				}
				else
				{
					m_Stripes[(2+i+j+jj/2+0)%8] = RGBA_MAKE(255,255,255,255); // White
					m_Stripes[(2+i+j+jj/2+1)%8] = RGBA_MAKE(255,  0,  0,225);	// Red
					m_Stripes[(2+i+j+jj/2+2)%8] = RGBA_MAKE(  0,255,  0,200);	// Green
					m_Stripes[(2+i+j+jj/2+3)%8] = RGBA_MAKE(  0,  0,255,175);	// Blue
					m_Stripes[(2+i+j+jj/2+4)%8] = RGBA_MAKE(255,255,  0,150);	// Yellow
					m_Stripes[(2+i+j+jj/2+5)%8] = RGBA_MAKE(255,  0,255,125);	// Magenta
					m_Stripes[(2+i+j+jj/2+6)%8] = RGBA_MAKE(  0,255,255,100);	// Cyan
					m_Stripes[(2+i+j+jj/2+7)%8] = RGBA_MAKE(128,128,128, 75); // Grey
				}
				if (!pppImages[j][jj]->LoadStripes(uWidth, uWidth, i==0 ? 8 : 1, m_Stripes, (m_pCurrentMode->nVolumeFormat+j+jj)%2?true:false))
				{
					bResult = false;
					goto Exit;
				}
			}
			uWidth  = max(1, uWidth  >> 1);
			uHeight = max(1, uHeight >> 1);
			uDepth  = max(1, uDepth  >> 1);
		}
		if( !m_pTexVol[m_nTexVol-1]->LoadImage( pppImages, TEXLOAD_DEFAULT ) )
		{
			bResult = false;
			goto Exit;
		}
	}

Exit:
    uDepth = uSlices;
	if (NULL != pppImages)
	for (UINT k = 0; k < uLevels; k++)
	{	
		if (NULL != pppImages[k])
		for (UINT kk = 0; kk < uDepth; kk++)
			SAFEDELETE(pppImages[k][kk]);

		SAFEDELETEA(pppImages[k]);
		uDepth = max( 1, uDepth>>1 );
	}

	SAFEDELETEA( pppImages );
	
	return bResult;
}

// ----------------------------------------------------------------------------

bool CPShader2_0::CapsCheck(void)
{
    if (!CPShaderX::CapsCheck())
        return false;
    
    DWORD dwMaxPSVersion = m_pSrcDevice->GetCaps()->dwPixelShaderVersion;
    
    // Check major version
    
    if (D3DSHADER_VERSION_MAJOR(dwMaxPSVersion) < 2)
    {
        WriteToLog(_T("Pixel shader version 2.0 required! Device supports %d.%d!\n"),
                   D3DSHADER_VERSION_MAJOR(dwMaxPSVersion), D3DSHADER_VERSION_MINOR(dwMaxPSVersion));
                   
        return false;
    }
    
    return true;
}
// ----------------------------------------------------------------------------

bool CPShader2_x::CapsCheck(void)
{
    if (!CPShaderX::CapsCheck())
        return false;
    
    DWORD dwMaxPSVersion = m_pSrcDevice->GetCaps()->dwPixelShaderVersion;
    
    // Check major version
    
    if (D3DSHADER_VERSION_MAJOR(dwMaxPSVersion) < 2)
    {
        WriteToLog(_T("Pixel shader version 2.x required! Device supports %d.%d!\n"),
                   D3DSHADER_VERSION_MAJOR(dwMaxPSVersion), D3DSHADER_VERSION_MINOR(dwMaxPSVersion));
                   
        return false;
    }
    if (m_pSrcDevice->GetCaps()->PS20Caps.Caps == 0 &&
        m_pSrcDevice->GetCaps()->PS20Caps.DynamicFlowControlDepth == 0 &&
        m_pSrcDevice->GetCaps()->PS20Caps.StaticFlowControlDepth == 0 &&
        m_pSrcDevice->GetCaps()->PS20Caps.NumTemps == D3DPS20_MIN_NUMTEMPS &&
        m_pSrcDevice->GetCaps()->PS20Caps.NumInstructionSlots == D3DPS20_MIN_NUMINSTRUCTIONSLOTS)
    {
        WriteToLog(_T("Pixel shader version 2.x required! Device supports 2.0 but none of the 2.x features!\n"));
        
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------

bool CPShader3_0::CapsCheck(void)
{
    if (!CPShaderX::CapsCheck())
        return false;
    
    DWORD dwMaxPSVersion = m_pSrcDevice->GetCaps()->dwPixelShaderVersion;
    
    // Check major version
    
    if ((D3DSHADER_VERSION_MAJOR(dwMaxPSVersion) < 3))
    {
        WriteToLog( _T("Minimum pixel shader version of 3.0 required for this test.\n") );
        
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------

HRESULT CInclude::Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{
	FILE *pFile = NULL;
	
	if(	m_bLoadFromFile )
	{
		pFile = fopen(pFileName, "r");
		if(NULL == pFile)
			return E_FAIL;
		
		*pBytes = _filelength(_fileno(pFile));
		*ppData = new BYTE[*pBytes];
		if(NULL == *ppData)
			return E_OUTOFMEMORY;

		memset((LPVOID)*ppData, 0, sizeof(BYTE) * *pBytes);
		*pBytes = (UINT) fread(*((char**)ppData), sizeof(TCHAR), *pBytes, pFile);
		fclose(pFile);
	}
	else
	{
		LPVOID pData = NULL;

		if(!LoadFromResource(pFileName, NULL, (DWORD*)pBytes))
			return E_FAIL;

		*ppData = new BYTE[*pBytes];
		if(NULL == *ppData)
			return E_OUTOFMEMORY;

		if(!LoadFromResource(pFileName, &pData, NULL))
		   return E_FAIL;
		memcpy((LPVOID)*ppData, pData, *pBytes);
	}

    return S_OK;
}

// ----------------------------------------------------------------------------

HRESULT CInclude::Close(LPCVOID pData)
{
	SAFEDELETEA(pData);
	return S_OK;
}
