// ============================================================================
//
//  Mesh.cpp
//  Written by robald.
//
// ============================================================================

#include "HOSurf.h"

#include "cd3dx.h"		// for D3DXAssembleShader9


char *g_sBasis[] = {	_T("Bezier"),
                        _T("BSpline"),
			  			_T("Catmull-Rom(INTERP)")};

char *g_sDegree[] = {	_T("UNKNOWN"),
			   			_T("Linear"),
			   		    _T("Quadratic"),
			   			_T("Cubic"),
						_T("Quadric(unsupported)"),
						_T("Quintic")};


// ----------------------------------------------------------------------------


CHOSurf::CHOSurf()
{
	// Are we doing the D3DXTessellation test or a device test?
	
	if(KeySet(_T("d3dx")))
	{
		// Set the test type
		SetTestType(TESTTYPE_FUNC);

		// set the test name
		m_szLogName = _T("D3DXTess");
		m_bD3DXTessellation = true;

		m_Options.D3DOptions.fMinDXVersion = 9.f;
		// we only want ref to eliminate driver conformance issues affecting 
		// the d3dx test.
		m_Options.D3DOptions.dwDevTypes = DEVICETYPE_REF;
		m_Options.D3DOptions.dwBehaviors = DEVICECREATE_HWVP_OR_PURE;// DEVICECREATE_STANDARD | DEVICECREATE_HWVP;
	}
	else
	{
		// Set the test type
		SetTestType(TESTTYPE_CONF);

		// set the test name
		m_szLogName = _T("HOSurf");
		m_bD3DXTessellation = false;

		m_Options.D3DOptions.fMinDXVersion = 8.f;
	}

    m_szCommandKey=_T("HOSurf");

	// Set the viewport data
    m_vLoc = D3DXVECTOR3(0.0f, 0.0f, -3.0f);
	m_vAt  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vUp  = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_fYaw = 0.f;
	m_fPitch = 0.f;
    m_nViewMatrix = 0;

    m_pTextures = NULL;
    m_pLights = NULL;
	m_pTextures = NULL;
    m_bQuintic = false;
    m_bWireFrame = false;
	m_bTexture = true;
	m_bSpecular = true;
	m_dwUsage = D3DUSAGE_RTPATCHES;
	m_nShaders = 0;
	memset(&m_pShaders, 0, sizeof(CnVertexShader*) * MAX_SHADERS);
	memset(&m_pDecls, 0, sizeof(LPDIRECT3DVERTEXDECLARATION9) * MAX_SHADERS);
	m_fAngle = (1.0f * pi) / (float)m_nFramesPerRot;
	m_fMaxSegments = 8.0f;

	if(!KeySet(_T("WHQL")))
	{
		m_bWireFrame = KeySet(_T("WIRE"));
		m_bSpecular  = !KeySet(_T("!SPECULAR"));
		m_bTexture  = !KeySet(_T("!TEXTURE"));
		m_bInspect = ReadInteger(_T("inspect"), &m_nPickTest);
		m_bRotate = KeySet(_T("rotate"));
		if(m_bInspect) m_Options.D3DOptions.bReference = false;
	}
	m_nFramesPerRot = 1;

	// Set the mode options
	
	if(m_bD3DXTessellation)
	{
		m_Options.D3DOptions.dwBehaviors = DEVICECREATE_SWVP | DEVICECREATE_STANDARD;		// We aren't testing the device, but software tessellation.
	}
	else
	{
		m_Options.D3DOptions.dwBehaviors = DEVICECREATE_HWVP_OR_PURE | DEVICECREATE_STANDARD;
	}
	m_pFramework->GetImageCmp()->SetMinPixels(0);
    m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_HWVP | DEVICECREATE_STANDARD;
	
	// Clear shader/decl arrays:
	for(int i=0; i < MAX_SHADERS; i++)
	{
		m_pShaders8[i] = NULL;
		m_pShaders[i] = NULL;
		m_pDecls[i] = NULL;
		m_pDecls_Out[i] = NULL;
		m_pRawDecls[i] = NULL;
		m_pRawDecls_Out[i] = NULL;
	}
}

void CHOSurf::CheckInput()
{
	bool bRes = false;
/*
	if(m_bInspect)
	{
		float fSpeed = .1f;
		float fTheta = pi / 180;
		D3DXVECTOR3 vVel(0.f, 0.f, 0.f);
		D3DXMATRIX mTemp;
		
		D3DXMatrixInverse(&mTemp, NULL, &(m_ViewMatrix[0]));

		if(KeyPress(VK_NUMPAD6)) {m_fYaw += fSpeed;bRes = true;}
		if(KeyPress(VK_NUMPAD4)) {m_fYaw -= fSpeed;bRes = true;}
		if(KeyPress(VK_NUMPAD9)) {m_fPitch += fSpeed;bRes = true;}
		if(KeyPress(VK_NUMPAD3)) {m_fPitch -= fSpeed;bRes = true;}
		if(KeyPress(VK_NUMPAD8)) {vVel.z += fSpeed;bRes = true;}
		if(KeyPress(VK_NUMPAD2)) {vVel.z -= fSpeed;bRes = true;}
		if(KeyPress(VK_INSERT))  {vVel.x += fSpeed;bRes = true;}
		if(KeyPress(VK_DELETE))  {vVel.x -= fSpeed;bRes = true;}
		if(bRes)
		{
			D3DXVec3TransformNormal(&vVel, &vVel, &mTemp);
			m_vLoc += vVel;

			D3DXQUATERNION qR;
			D3DXQuaternionRotationYawPitchRoll( &qR, m_fYaw, m_fPitch, 0.0f );
			D3DXMatrixAffineTransformation( &mTemp, 1.0f, NULL, &qR, &m_vLoc );
			D3DXMatrixInverse(&(m_ViewMatrix[0]), NULL, &mTemp);
			SetTransform(D3DTS_VIEW, &(m_ViewMatrix[0]));
		}

	}

	if(KeyPress('W', true))
	{
		m_bWireFrame = !m_bWireFrame;
	}
*/
}
// ----------------------------------------------------------------------------

bool CHOSurf::SetDefaultRenderStates(void)
{
    SetRenderState(D3DRS_NORMALIZENORMALS, (DWORD)TRUE);
    SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_CCW);
    if(m_pSrcDevice->GetCaps()->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)
	{
		SetSamplerState( 0, SAMP_MINFILTER, TEXF_LINEAR );
		SetSamplerState( 1, SAMP_MINFILTER, TEXF_LINEAR );
	}
    
	if(m_pSrcDevice->GetCaps()->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR)
	{
		SetSamplerState( 0, SAMP_MAGFILTER, TEXF_LINEAR );
		SetSamplerState( 1, SAMP_MAGFILTER, TEXF_LINEAR );
	}
	
	return true;
}

// ----------------------------------------------------------------------------

bool CHOSurf::SetDefaultMatrices(void)
{   
	D3DXMATRIX	Matrix;
    HRESULT     hr;
	float 		fAspect = 1.33f;
	float		fFOV = pi/4.0f;
    
	D3DXMatrixRotationY(&m_RotMatrix, -m_fAngle);
    D3DXMatrixIdentity(&m_PosMatrix);

    D3DXMatrixPerspectiveFovLH(&m_ProjMatrix, fFOV, fAspect, 1.0f, 1000.0f);
    if (!SetTransform(D3DTS_PROJECTION,&m_ProjMatrix))
        return false;

    D3DXMatrixIdentity(&Matrix);
    if (!SetTransform(D3DTS_WORLD, &Matrix))
        return false;

	// Setup the view matrices
	D3DXVECTOR3 v = D3DXVECTOR3(0.0f, 0.0f, -3.0f);
    D3DXMatrixLookAtLH(&(m_ViewMatrix[m_nViewMatrix++]), &m_vLoc, &m_vAt, &m_vUp);
    v = D3DXVECTOR3(3.0f, 0.0f, -1.0f);
    D3DXMatrixLookAtLH(&(m_ViewMatrix[m_nViewMatrix++]), &v, &m_vAt, &m_vUp);
    v = D3DXVECTOR3(-3.0f, 0.0f, -1.0f);
	D3DXMatrixLookAtLH(&(m_ViewMatrix[m_nViewMatrix++]), &v, &m_vAt, &m_vUp);
    v = D3DXVECTOR3(0.0f, 3.0f, -1.0f);
    D3DXMatrixLookAtLH(&(m_ViewMatrix[m_nViewMatrix++]), &v, &m_vAt, &m_vUp);
    v = D3DXVECTOR3(0.0f, -3.0f, -1.0f);
	D3DXMatrixLookAtLH(&(m_ViewMatrix[m_nViewMatrix++]), &v, &m_vAt, &m_vUp);

    float fDist = 7.0;
    v = D3DXVECTOR3(0.0f, 0.0f, fDist);
	D3DXMatrixLookAtLH(&(m_TPViewMatrix[m_nTPViewMatrix++]), &v, &m_vAt, &m_vUp);
	v = D3DXVECTOR3(fDist, 0.0f, 0.0f);
	D3DXMatrixLookAtLH(&(m_TPViewMatrix[m_nTPViewMatrix++]), &v, &m_vAt, &m_vUp);
	v = D3DXVECTOR3(-fDist, 0.0f, 0.0f);
	D3DXMatrixLookAtLH(&(m_TPViewMatrix[m_nTPViewMatrix++]), &v, &m_vAt, &m_vUp);
	D3DXVECTOR3 vUp = D3DXVECTOR3(1.0f, 0.f, 0.f);
	v = D3DXVECTOR3(0.0f, -fDist, 0.0f);
	D3DXMatrixLookAtLH(&(m_TPViewMatrix[m_nTPViewMatrix++]), &v, &m_vAt, &vUp);
	v = D3DXVECTOR3(0.0f, fDist, 0.0f);
	D3DXMatrixLookAtLH(&(m_TPViewMatrix[m_nTPViewMatrix++]), &v, &m_vAt, &vUp);				
	
	if (!SetTransform(D3DTS_VIEW, &(m_ViewMatrix[0])))
		return false;

    // Set the texture transform
    D3DXMatrixRotationZ(&(m_TexTrans[0]), 0);
    D3DXMatrixRotationZ(&(m_TexTrans[1]), pi);
	D3DXMatrixRotationZ(&(m_TexTrans[2]), 0);

    return true;
}

// ----------------------------------------------------------------------------

bool CHOSurf::SetDefaultMaterials(void)
{   
	memset(&m_HOMat, 0 , sizeof(MATERIAL));
	m_HOMat.diffuse.r = 1.0f;
	m_HOMat.diffuse.g = 1.0f;
	m_HOMat.diffuse.b = 1.0f;
	m_HOMat.diffuse.a = 1.0f;
	m_HOMat.ambient.r = 0.f;
	m_HOMat.ambient.g = 0.f;   
	m_HOMat.ambient.b = 0.f; 
    m_HOMat.specular.r = 1.0f;
	m_HOMat.specular.g = 1.0f;   
	m_HOMat.specular.b = 1.0f;
	m_HOMat.power = 10.0f;
	
    memset(&m_PointMat, 0 , sizeof(MATERIAL));
	m_PointMat.diffuse.r = 1.0f;
	m_PointMat.diffuse.g = 1.0f;
	m_PointMat.diffuse.b = 1.0f;
	m_PointMat.diffuse.a = 1.0f;	 
	m_PointMat.ambient.r = 0.0f;
	m_PointMat.ambient.g = 1.0f;   
	m_PointMat.ambient.b = 0.0f; 
	m_PointMat.power = 10.0f;

    memset(&m_TeapotMat, 0 , sizeof(MATERIAL));
	m_TeapotMat.diffuse.r = 1.0f;
	m_TeapotMat.diffuse.g = 1.0f;
	m_TeapotMat.diffuse.b = 1.0f;
	m_TeapotMat.diffuse.a = 1.0f;	 
	m_TeapotMat.ambient.r = 0.0f;
	m_TeapotMat.ambient.g = 0.0f;   
	m_TeapotMat.ambient.b = 0.5f; 
	m_TeapotMat.power = 10.0f;
    
	return true;
}


// ----------------------------------------------------------------------------

bool CHOSurf::SetDefaultLights(void)
{
    D3DLIGHT9 LightData;
    DWORD dwPosLightsSupported = 1;
    
	// Set the total light count
	m_nLightCount = 2;

    // Create the CLights
    m_pLights = new LIGHT[m_nLightCount];
	memset(m_pLights, 0, sizeof(LIGHT) * m_nLightCount);
    
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

// ----------------------------------------------------------------------------
		 
void CHOSurf::CommandLineHelp(void)
{
    //WriteCommandLineHelp("$y-STARTWITH <int>\t$wStart execution at a specific stateblock");
}

// ----------------------------------------------------------------------------

UINT CHOSurf::TestInitialize(void)
{
	// Do Version / Cap checking
	if(m_bD3DXTessellation && m_dwVersion < 0x900)
	{
		WriteToLog(_T("TestInitialize() - This test requires DX9.\n"));
		SetTestRange(1,1);
		return D3DTESTINIT_SKIPALL;
	}
	
    if( m_dwVersion < 0x800)
	{
		WriteToLog(_T("TestInitialize() - This test requires DX8.\n"));
		SetTestRange(1,1);
		return D3DTESTINIT_SKIPALL;
	}

	if(!m_bD3DXTessellation)
	{
		// Check for DX8 DDI support
		if(m_pSrcDevice->GetDDI() < 8.f)
		{
			WriteToLog(_T("TestInitialize() - This test requires a DX8 DDI...exiting.\n"));
			SetTestRange(1,1);
			return D3DTESTINIT_SKIPALL;
		}
		
		// Check for HW VP
		if(!m_pSrcDevice->IsHWVP())
		{
			WriteToLog(_T("TestInitialize() - HO surfaces require HW vertex processing.\n"));
			WriteToLog(_T("TestInitialize() - Selected device uses SW vertex processing...exiting.\n"));
			SetTestRange(1,1);
			return D3DTESTINIT_SKIPALL;
		}

		// Check for HO caps
		if(!CheckCaps())
		{
			WriteToLog(_T("TestInitialize() - CheckCaps() failed.  Skipping...\n"));
			SetTestRange(1,1);
			return D3DTESTINIT_SKIPALL;
		}
	}

   	// Check for vertex shader version > 1
	if(!m_bD3DXTessellation && 
		D3DSHADER_VERSION_MAJOR(m_pSrcDevice->GetCaps()->dwVertexShaderVersion) >= 1)
		m_bVS = true;
	else 
		m_bVS = false;

   	m_nSurfCount = 0;
    m_nSurfHandles = 0;
    m_nLastSurf = -1;
    m_fFrameCount = 0.0f;
    m_fRange = 2.0f * pi;
	m_fStep = m_fRange / 32;
    m_nFramesPerSurface = m_nFramesPerRot * m_nViewMatrix;

	// Load the geometry
	if(!LoadGeometry() || !LoadTextures())
		return D3DTESTINIT_ABORT;
	
	// Setup the vertex shader
    if(!SetupVertexShader())
		 return D3DTESTINIT_ABORT;

	// Set the test range
	SetTestRange(1, m_bInspect ? 0xFFFFFFFF : m_nFramesPerSurface * m_nSurfCount);
    
    return D3DTESTINIT_RUN;
}

// --------------------------------------------------------------------------------------

bool CHOSurf::ClearFrame(void)
{
	DebugOut.Write(4, _T("CD3DTest::ClearFrame\n"));
    return Clear(0, NULL, FMWK_DEFAULT, 0xFF000000, 1.f, 0);
}

// ----------------------------------------------------------------------------

bool CHOSurf::SetupVertexShader()
{
	HRESULT hr;

	DWORD dwDeviceFlag = m_bD3DXTessellation ? CD3D_REF : CD3D_ALL;

	if(m_bVS)
	{
		LPD3DXBUFFER pShader = NULL;
		if(m_dwVersion < 0x900)
		{
			DWORD decl0_8[] =
			{
				D3DVSD_STREAM(0),
				D3DVSD_REG( 8, D3DVSDT_FLOAT3),
				D3DVSD_REG( 6, D3DVSDT_FLOAT3),
				D3DVSD_REG( 11, D3DVSDT_FLOAT2),
				D3DVSD_STREAM_TESS(),
				D3DVSD_TESSNORMAL( 8, 9 ),
				D3DVSD_TESSUV( 10 ),
				D3DVSD_STREAM(1),
				D3DVSD_REG(12, D3DVSDT_D3DCOLOR),
				D3DVSD_END()
			};

			TCHAR *szShader = _T("vs.1.0\n"
								"dp4 oPos.x, v8, c0\n"
								"dp4 oPos.y, v8, c1\n"
								"dp4 oPos.z, v8, c2\n"
								"dp4 oPos.w, v8, c3\n"
								"mul oT1.xy, c4.xy, v10.xy\n"
								"mov oT0, v11\n"
								"mov oD0, v12\n");

			hr = D3DXAssembleShader8(szShader, _tcslen(szShader), 0, NULL, &pShader, NULL);
			if (FAILED(hr))
			{
				WriteToLog(_T("CHOSurf::SetupVertexShader -- D3DXAssembleShader() failed, hr=%s\n"),
							m_pD3D->HResultToString(hr));
				return false;
			}

			if(!CreateVertexShader(decl0_8, (DWORD*)pShader->GetBufferPointer(), &(m_pShaders8[m_nShaders]), 0))
			{
				WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexShader() failed.  hr=%s\n"),
							m_pD3D->HResultToString(GetLastError()));
				return false;
			}
		}
		else
		{
			static VERTEXELEMENT decl0[] = 
			{
				{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 1},
				{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_CROSSUV, DECLUSAGE_NORMAL, 0},
				{0, 0, (DECLTYPE)D3DDECLTYPE_UNUSED, DECLMETHOD_UV, DECLUSAGE_TEXCOORD, 3},
				{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 2},
				{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 4},
				{1, 0, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 5},
				DECL_END()
			};
			m_pRawDecls[m_nShaders] = decl0;

			// NOTE: I don't believe the following is used anywhere:
			static VERTEXELEMENT decl0_out[] = 
			{
				{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 1},
				{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL, 0},
				{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 3},
				{0, 32, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 2},
				{0, 44, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 4},
				{1, 0, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 5},
				DECL_END()
			};
			m_pRawDecls_Out[m_nShaders] = decl0_out;

			if(!CreateVertexDeclaration(decl0, &(m_pDecls[m_nShaders]), dwDeviceFlag))
			{
				WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexDeclaration() failed\n"),
							m_pD3D->HResultToString(GetLastError()));
				return false;
			}

			if(!CreateVertexDeclaration(decl0_out, &(m_pDecls_Out[m_nShaders])))
			{
				WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexDeclaration() failed\n"),
							m_pD3D->HResultToString(GetLastError()));
				return false;
			}


			TCHAR *szShader = _T("vs_1_1\n"

								"dcl_texcoord1 v8\n"
								"dcl_texcoord3 v10\n"
								"dcl_texcoord4 v11\n"
								"dcl_texcoord5 v12\n"

								"dp4 oPos.x, v8, c0\n"
								"dp4 oPos.y, v8, c1\n"
								"dp4 oPos.z, v8, c2\n"
								"dp4 oPos.w, v8, c3\n"
								"mul oT1.xy, c4.xy, v10.xy\n"
								"mov oT0, v11\n"
								"mov oD0, v12\n");

		
			hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
			if (FAILED(hr))
			{
				WriteToLog(_T("CHOSurf::SetupVertexShader -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
							m_pD3D->HResultToString(hr));
				return false;
			}

			if(!CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &(m_pShaders[m_nShaders])))
			{
				WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexShader() failed.  hr=%s\n"),
							m_pD3D->HResultToString(GetLastError()));
				return false;
			}
		}		// end if version 9 or greater

		if(pShader)
			pShader->Release();
	}

	// Even if the above shader isn't defined, we must keep the numbering consistent, then fill in the missing shader (by copying another) later:
	m_nShaders++;
	
	if(m_dwVersion < 0x900)
	{
		DWORD decl1_8[] =
		{
			D3DVSD_STREAM(0),
			D3DVSD_REG( D3DVSDE_POSITION,  D3DVSDT_FLOAT3),
			D3DVSD_REG( D3DVSDE_BLENDWEIGHT,   D3DVSDT_FLOAT3),
			D3DVSD_REG( D3DVSDE_TEXCOORD2, D3DVSDT_FLOAT2),
			D3DVSD_STREAM_TESS(),
			D3DVSD_TESSNORMAL( D3DVSDE_POSITION, D3DVSDE_NORMAL ),
			D3DVSD_TESSUV( D3DVSDE_TEXCOORD0 ),
			D3DVSD_TESSUV( D3DVSDE_TEXCOORD1 ),
			D3DVSD_END()
		};

		if(!CreateVertexShader(decl1_8, NULL, &(m_pShaders8[m_nShaders]), 0))
		{
			WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexShader() failed.  hr=%s\n"),
						m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}
	else
	{
		static VERTEXELEMENT decl1[] = 
		{
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
			{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_BLENDWEIGHT, 0},
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_CROSSUV, DECLUSAGE_NORMAL, 0},
			{0, 0, D3DDECLTYPE_UNUSED, DECLMETHOD_UV, DECLUSAGE_TEXCOORD, 0},
			{0, 0, D3DDECLTYPE_UNUSED, DECLMETHOD_UV, DECLUSAGE_TEXCOORD, 1},
			{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 2},
			DECL_END()
		};
		m_pRawDecls[m_nShaders] = decl1;

		static VERTEXELEMENT decl1_out[] = 
		{
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
			{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_BLENDWEIGHT, 0},
			{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL, 0},
			{0, 36, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
			{0, 44, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 1},
			{0, 52, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 2},
			DECL_END()
		};
		m_pRawDecls_Out[m_nShaders] = decl1_out;

		if(!CreateVertexDeclaration(decl1, &(m_pDecls[m_nShaders]), dwDeviceFlag))
		{
			WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexDeclaration() failed.  hr=%s\n"),
						m_pD3D->HResultToString(GetLastError()));
			return false;
		}

		if(!CreateVertexDeclaration(decl1_out, &(m_pDecls_Out[m_nShaders])))
		{
			WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexDeclaration() failed.  hr=%s\n"),
						m_pD3D->HResultToString(GetLastError()));
			return false;
		}

		if(!CreateVertexShader(NULL, &(m_pShaders[m_nShaders])))
		{
			WriteToLog(_T("CStateBeast::Init8 -- CreateVertexShader() failed.  hr=%s\n"),
						m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}

	m_nShaders++;

	if(m_dwVersion < 0x900)
	{
		DWORD decl2_8[] =
		{
			D3DVSD_STREAM(0),
			D3DVSD_REG( D3DVSDE_POSITION,    D3DVSDT_FLOAT3),
			D3DVSD_REG( D3DVSDE_BLENDWEIGHT, D3DVSDT_FLOAT3),
			D3DVSD_REG( D3DVSDE_TEXCOORD0,   D3DVSDT_FLOAT2),
			D3DVSD_STREAM_TESS(),
			D3DVSD_TESSNORMAL( D3DVSDE_POSITION, D3DVSDE_NORMAL ),
			D3DVSD_TESSUV(D3DVSDE_TEXCOORD1),
			D3DVSD_END()
		};

		if(!CreateVertexShader(decl2_8, NULL, &(m_pShaders8[m_nShaders]), 0))
		{
			WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexShader() failed.  hr=%s\n"),
						m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}
	else
	{
		static VERTEXELEMENT decl2[] = 
		{
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
			{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_BLENDWEIGHT, 0},
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_CROSSUV, DECLUSAGE_NORMAL, 0},
			{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
			{0, 0, D3DDECLTYPE_UNUSED, DECLMETHOD_UV, DECLUSAGE_TEXCOORD, 1},
			DECL_END()
		};
		static VERTEXELEMENT decl2_out[] = 
		{
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
			{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_BLENDWEIGHT, 0},
			{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL, 0},
			{0, 36, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
			{0, 44, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 1},
			DECL_END()
		};

		/*
		static VERTEXELEMENT decl2[] = 
		{
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_CROSSUV, DECLUSAGE_NORMAL, 0},
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_UV, DECLUSAGE_TEXCOORD, 0},
			DECL_END()
		};
		static VERTEXELEMENT decl2_out[] = 
		{
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
			{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL, 0},
			//{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
			DECL_END()
		};
		*/
		m_pRawDecls[m_nShaders] = decl2;
		m_pRawDecls_Out[m_nShaders] = decl2_out;

		if(!CreateVertexDeclaration(decl2, &(m_pDecls[m_nShaders]), dwDeviceFlag))
		{
			WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexDeclaration() failed.  hr=%s\n"),
						m_pD3D->HResultToString(GetLastError()));
			return false;
		}

		if(!CreateVertexDeclaration(decl2_out, &(m_pDecls_Out[m_nShaders])))
		{
			WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexDeclaration() failed.  hr=%s\n"),
						m_pD3D->HResultToString(GetLastError()));
			return false;
		}

		if(!CreateVertexShader(NULL, &(m_pShaders[m_nShaders])))
		{
			WriteToLog(_T("CStateBeast::Init8 -- CreateVertexShader() failed.  hr=%s\n"),
						m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}

	m_nShaders++;

/*	Should be no reason to do this, as this case SHOULD be skipped:
	if(!m_bVS)		// Then first shader was skipped, fill it in by copying one of the other shaders (how about the second one)
	{
		m_pDecls[0] = m_pDecls[1];
		m_pDecls_Out[0] = m_pDecls_Out[1];
		m_pShaders[0] = m_pShaders[1];

		// Add to their reference count, so they don't get released too many times:
		m_pDecls[0]->AddRef();
		m_pDecls_Out[0]->AddRef();
		m_pShaders[0]->AddRef();
	}
*/
	return true;
}

// ----------------------------------------------------------------------------

bool CHOSurf::SetupBuffers(DWORD dwFVF, int nVertices, int nStride, void *pVertices, int nIndices, 
						   WORD *pIndices, CnVertexBuffer **ppVB, CnIndexBuffer **ppIB, CnVertexBuffer **ppColor)
{
    PBYTE pBuffer = NULL;
	HRESULT hr = S_OK;
    
	// Setup the vertex buffer
	if(!CreateVertexBuffer(GetVertexSize(dwFVF) * nVertices, m_dwUsage, dwFVF, D3DPOOL_SYSTEMMEM, ppVB))
	{
		WriteToLog(_T("SetupBuffers() - CreateVertexBuffer failed.\n"));
		return false;
	}

	if(!(*ppVB)->Lock(0,0,&pBuffer,0))
	{
		WriteToLog(_T("SetupBuffers() - couldn't lock VB. hr=%s\n"), m_pD3D->HResultToString(GetLastError()));
		(*ppVB)->Release();
		return false;
	}
    
	PBYTE pPos = (PBYTE)pBuffer;
	PBYTE pVer = (PBYTE)pVertices;
	int nSize = GetVertexSize(dwFVF);
    for(int i=0; i<nVertices; i++)
	{
		memcpy(pPos, pVer, nSize);
		pPos += nSize;
        pVer += nStride;
	}
	
    (*ppVB)->Unlock();
    

	// Setup the index buffer
    if(!CreateIndexBuffer(nIndices * sizeof(WORD), m_dwUsage, FMT_INDEX16, D3DPOOL_SYSTEMMEM, ppIB))
	{
		WriteToLog(_T("SetupBuffers() - CreateIndexBuffer failed. hr=%s(%x)\n"));
		(*ppVB)->Release();
		return false;
	}

	if(!(*ppIB)->Lock(0,0,&pBuffer,0))
	{
		WriteToLog(_T("SetupBuffers() - couldn't lock IB. hr=%s\n"), m_pD3D->HResultToString(GetLastError()));
		(*ppVB)->Release();
        (*ppIB)->Release();
		return false;
	}
	
    memcpy(pBuffer, pIndices, nIndices*sizeof(WORD));
    (*ppIB)->Unlock();

	// Create the color stream
	nSize = GetVertexSize(D3DFVF_DIFFUSE);
	if(!CreateVertexBuffer(nVertices * nSize, m_dwUsage, D3DFVF_DIFFUSE, D3DPOOL_SYSTEMMEM, ppColor))
	{
		WriteToLog(_T("SetupBuffers() - CreateVertexBuffer failed. hr=%s\n"), m_pD3D->HResultToString(GetLastError()));
        (*ppVB)->Release();
        (*ppIB)->Release();
		return false;
	}

	if(!(*ppColor)->Lock(0,0,&pBuffer,0))
	{
		WriteToLog(_T("SetupBuffers() - couldn't lock VB. hr=%s\n"), m_pD3D->HResultToString(GetLastError()));
        (*ppVB)->Release();
        (*ppIB)->Release();
		(*ppColor)->Release();
		return false;
	}
    
	D3DCOLOR* pColor = (D3DCOLOR*)pBuffer;
	
    for(int i=0; i<nVertices; i++)
	{
		DWORD dwVal = i * (0xff/nVertices);
		*pColor = D3DCOLOR_XRGB(dwVal, 0xff - dwVal, 0);
		pColor++;
	}
	
    (*ppColor)->Unlock();
	
	return true;
}

// ----------------------------------------------------------------------------

HOSURFACE* CHOSurf::CreateHOSurf(LPD3DXMESH pShape, HOTEST *pTest, DWORD dwFVF)
{
	// Create the structure
	HOSURFACE *pSurf = new HOSURFACE;
	if(!pSurf)
	{
		WriteToLog(_T("CreatePrim() - Out of memory.\n"));
		return NULL;
	}
	
	// clear the memory
	memset(pSurf, 0, sizeof(HOSURFACE));

	// set the FVF
	pSurf->dwFVF = dwFVF;

	BYTE *pVertices, *pIndices;
	int nVertices = pShape->GetNumVertices();
	int nIndices  = pShape->GetNumFaces() * 3;
	if(FAILED(pShape->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVertices)) ||
	   FAILED(pShape->LockIndexBuffer(D3DLOCK_READONLY, (void**)&pIndices)))
	{
		WriteToLog(_T("CHOSurf::CreateHOSurf() - failed to lock VB/IB.\n"));
		if(pSurf) delete pSurf;
		return NULL;
	}

	// Create the buffers
	if(!SetupBuffers(pSurf->dwFVF, nVertices, GetVertexSize(pSurf->dwFVF), (D3DVERTEX*)pVertices, nIndices, (WORD*)pIndices, 
					 &(pSurf->pVB), &(pSurf->pIB), &(pSurf->pColor)))
	{
		delete pSurf;
		return NULL;
	}

	pShape->UnlockVertexBuffer();
    pShape->UnlockIndexBuffer();

    // Set the data
	pSurf->uVertices 	= nVertices;
	pSurf->uIndices  	= nIndices;

	pSurf->pTest = new HOTEST;

	if(NULL == pSurf->pTest)
	{
		WriteToLog(_T("CHOSurf::CreateHOSurf() - Out of memory.\n"));
		delete pSurf;
		return NULL;
	}
	memcpy(pSurf->pTest, pTest, sizeof(HOTEST));

	
	return pSurf;
}

// ----------------------------------------------------------------------------

HOSURFACE* CHOSurf::CreateHOSurf(CTestShapes *pShape, HOTEST *pTest, DWORD dwFVF)
{
	// Create the structure
	HOSURFACE *pSurf = new HOSURFACE;
	if(!pSurf)
	{
		WriteToLog(_T("CreatePrim() - Out of memory.\n"));
		return NULL;
	}
	
	// clear the memory
	memset(pSurf, 0, sizeof(HOSURFACE));

	// Set the FVF
	pSurf->dwFVF = dwFVF;

	// Create the buffers
	if(!SetupBuffers(pSurf->dwFVF, pShape->m_nVertices, GetVertexSize(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1), pShape->m_pVertices, pShape->m_nIndices, pShape->m_pIndices, 
					 &(pSurf->pVB), &(pSurf->pIB), &(pSurf->pColor)))
	{
		delete pSurf;
		return NULL;
	}

    // Set the data
	pSurf->uVertices 	= pShape->m_nVertices;
	pSurf->uIndices  	= pShape->m_nIndices;
	pSurf->pTest = new HOTEST;

	if(NULL == pSurf->pTest)
	{
		WriteToLog(_T("CHOSurf::CreateHOSurf() - Out of memory.\n"));
		delete pSurf;
		return NULL;
	}
	memcpy(pSurf->pTest, pTest, sizeof(HOTEST));

	return pSurf;
}

// ----------------------------------------------------------------------------

HOSURFACE* CHOSurf::CreateHOSurf(TRIPATCH *pPatch, HOTEST *pTest, DWORD dwFVF)
{
	// Create the structure
	HOSURFACE *pSurf = new HOSURFACE;
	if(!pSurf)
	{
		WriteToLog(_T("CreatePrim() - Out of memory.\n"));
		return NULL;
	}
    
	// clear the memory
	memset(pSurf, 0, sizeof(HOSURFACE));

	// Set the FVF
	pSurf->dwFVF = dwFVF; 

	// Create the buffers
	if(!SetupBuffers(pSurf->dwFVF, pPatch->nVertices, GetVertexSize(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1), pPatch->pVertices, pPatch->nIndices, pPatch->pIndices, 
					 &(pSurf->pVB), &(pSurf->pIB), &(pSurf->pColor)))
	{
		delete pSurf;
		return NULL;
	}

    // Set the data
	pSurf->uVertices 	= pPatch->nVertices;
	pSurf->uIndices  	= pPatch->nIndices;
	pSurf->pTest = new HOTEST;

	if(NULL == pSurf->pTest)
	{
		WriteToLog(_T("CHOSurf::CreateHOSurf() - Out of memory.\n"));
		delete pSurf;
		return NULL;
	}
	memcpy(pSurf->pTest, pTest, sizeof(HOTEST));

	return pSurf;
}

// ----------------------------------------------------------------------------

bool CHOSurf::LoadTextures()
{
	HRESULT hr;
	const int nTextures = 2;
	m_nTextureCount = nTextures;

	m_pTextures = new CnTexture*[nTextures];
	if(!m_pTextures)
	{
		WriteToLog(_T("CHOSurf::LoadTextures() - Out of Memory.\n"));
		return false;
	}
	memset(m_pTextures, 0, sizeof(CTexture*) * nTextures);

    const TCHAR   ptcsTextures[nTextures][14] = {
			_T("tex0.BMP"),
			_T("tex1.BMP")};

	for(int i = 0; i < nTextures; i++)
	{
		if(!CreateTexture(256, 256, 1, 0, *GetCurrentTextureFormat(), FMWK_DEFAULT, &(m_pTextures[i])))
		{
			WriteToLog(_T("CHOSurf::LoadTextures() - CreateTexture failed. hr=%s.\n"), m_pD3D->HResultToString(GetLastError()));
			return false;
		}

		if(!m_pTextures[i]->LoadTexture(1, ptcsTextures[i]))
		{
			WriteToLog(_T("CHOSurf::LoadTextures() - LoadTexture() failed. hr=%s.\n"), m_pD3D->HResultToString(GetLastError()));
			return false;
		}
    }
	return true;
}

// ----------------------------------------------------------------------------

bool CHOSurf::DrawPoints(HOSURFACE *pSurf)
{
	float fTess = 0.f;

	SetMaterial(&m_PointMat);
	SetTexture(0,NULL);
	SetRenderState(D3DRS_SPECULARENABLE , (DWORD) FALSE);
	SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_POINT);
	if(m_dwVersion < 0x900)
	{
		// D3DRS_PATCHSEGMENTS = 164		TODO: Put this in framework header somewhere
		SetRenderState((D3DRENDERSTATETYPE)164, (DWORD)*((unsigned*)&fTess));
	}
	SetFVF(pSurf->dwFVF);
	if(m_dwVersion >= 0x900)
	{
		SetVertexShader((CnVertexShader*)NULL);
	}

	return DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,(DWORD)pSurf->uVertices, 
								 0, (DWORD)pSurf->uIndices / 3, 0, CD3D_BOTH);

}

// ----------------------------------------------------------------------------

bool CHOSurf::DeleteSurfHandles(bool bAll)
{
	bool bRes = true;
	
	for(int i=0; i<m_nSurfHandles; i++)
	{
		// Delete every one except the first
		if(bAll || (i>1))
		{
			if(m_bD3DXTessellation)
				bRes &= !FAILED(m_pRefDevice->DeletePatch(i+1));
			else
				bRes &= DeletePatch(i+1);
		}
	}

	m_nSurfHandles = 0;

	return bRes;
}

// ----------------------------------------------------------------------------

bool CHOSurf::PreExecuteTest(UINT uTestNumber)
{
    // By Default: Skip block if VS not supported
	// Note: if you override SetupVertexShader, this code probably does not apply, so override this function too!
    if(!m_bVS && m_pSurfs[m_nCurrentSurf]->pTest->nShader == 0)
	{
        if(m_pSurfs[m_nCurrentSurf]->pTest->PatchType == UTAHTEAPOT)
			SkipTestRange(uTestNumber, uTestNumber+m_nTPViewMatrix-1);
		else
            SkipTestRange(uTestNumber, uTestNumber+m_nViewMatrix-1);

		return false;	// stop current test
	}

	return true;		// continue with test
}

// ----------------------------------------------------------------------------

bool CHOSurf::ExecuteTest(UINT uTestNumber)
{
	HRESULT hr;

	// For use on calls that should only be done on ref during d3dx tessellation:
	DWORD dwDeviceFlag = m_bD3DXTessellation ? CD3D_REF : CD3D_ALL;

	// Determine the current surface 
	m_nCurrentSurf = m_bInspect ? (m_nPickTest-1) / m_nFramesPerSurface : 
								  (uTestNumber-1) / m_nFramesPerSurface;

	// Begin the test case & spew some info
	TCHAR tcsName[MAX_PATH], tcsLog[MAX_PATH];
    GetLogTags(tcsName, tcsLog);
    BeginTestCase(tcsName, uTestNumber);
	WriteToLog(2, "%s\n", tcsLog);

	// Test-specific hook to skip unsupported tests:
	if(!PreExecuteTest(uTestNumber))
	{
		return false;
	}

	// Skip if quintic aren't supported
	if(!m_bQuintic && m_pSurfs[m_nCurrentSurf]->pTest->HODegree == D3DDEGREE_QUINTIC)
	{
		SkipTestRange(uTestNumber, uTestNumber);
		return false;
	}

	BeginScene();

	// Delete existing handles
	if(m_nCurrentSurf != m_nLastSurf && !DeleteSurfHandles(false))
	{
		Fail();
		WriteToLog(_T("ExecuteTest() - DeleteSurfHandles() failed.\n"));
	}

	// Set the streams

	SetStreamSource(0, m_pSurfs[m_nCurrentSurf]->pVB, GetVertexSize(m_pSurfs[m_nCurrentSurf]->dwFVF));
	
	if(m_bVS)
	{
		SetStreamSource(1,m_pSurfs[m_nCurrentSurf]->pColor, GetVertexSize(D3DFVF_DIFFUSE));
	}
	
	SetIndices(m_pSurfs[m_nCurrentSurf]->pIB, 0);

	// Rotate if appropriate
	if(m_bRotate)
	{
		D3DXMatrixMultiply(&m_PosMatrix, &m_PosMatrix, &m_RotMatrix);
		SetTransform(D3DTS_WORLD, &m_PosMatrix);
	}

	// Check for keyboard input
	CheckInput();

	// If we're not inspecting, rotate the view
    if(!m_bInspect)
	{
		D3DXMATRIX *pMat;
        if(m_pSurfs[m_nCurrentSurf]->pTest->PatchType == UTAHTEAPOT)
		{
            pMat = &(m_TPViewMatrix[(uTestNumber-1) % m_nViewMatrix]);
			SetTransform(D3DTS_VIEW, pMat);
        }
		else
		{
			pMat = &(m_ViewMatrix[(uTestNumber-1) % m_nViewMatrix]);
			SetTransform(D3DTS_VIEW, pMat);
		}

        D3DXMATRIX mat;
		D3DXMatrixMultiply( &mat, pMat, &m_ProjMatrix);
		D3DXMatrixTranspose( &mat, &mat );
		SetVertexShaderConstant(0, &mat, 4);
	}

	// Draw the points
    //DrawPoints(m_pSurfs[m_nCurrentSurf]);

	// Setup the render states
	if(m_bSpecular && m_pSurfs[m_nCurrentSurf]->pTest->bSpecular)
		SetRenderState(D3DRS_SPECULARENABLE, (DWORD) TRUE);
	if(m_bWireFrame)
		SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_WIREFRAME);
	else
		SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_SOLID);
		
	SetMaterial(&m_HOMat);

	// Set the textures
	//if(m_nCurrentSurf != m_nLastSurf)
	{
		SetupTextures(m_pSurfs[m_nCurrentSurf]->pTest->dwTexture);
	}

	/*
	// DEBUG: verify vertex declarations are what I passed at beginning:
	VERTEXELEMENT pDecls[2][17];
	UINT uDeclCount = 17;			// just for GetDeclaration
	for(UINT i=0; i<2; i++)
	{
		CVertexDeclaration* pcdecl;
		m_pDecls[m_pSurfs[m_nCurrentSurf]->pTest->nShader]->GetVDecl(&pcdecl, i);
		if(pcdecl)
		{
			pcdecl->GetDeclaration(pDecls[i], &uDeclCount);
			RELEASE(pcdecl);
		}
	}
	*/

	// Set the vertex shader
	if(m_dwVersion < 0x900)
	{
		SetVertexShader(m_pShaders8[m_pSurfs[m_nCurrentSurf]->pTest->nShader]);
	}
	else
	{
		SetVertexShader(m_pShaders[m_pSurfs[m_nCurrentSurf]->pTest->nShader]);

		if(!SetVertexDeclaration(m_pDecls[m_pSurfs[m_nCurrentSurf]->pTest->nShader], dwDeviceFlag))
		{
			Fail();
			WriteToLog(_T("CHOSurf::ExecuteTest() - SetVertexDeclaration failed.\n"));
		}
		if(m_bD3DXTessellation)
		{
			if(!SetVertexDeclaration(m_pDecls_Out[m_pSurfs[m_nCurrentSurf]->pTest->nShader], CD3D_SRC))
			{
				Fail();
				WriteToLog(_T("CHOSurf::ExecuteTest() - SetVertexDeclaration failed.\n"));
			}
		}
	}

	// Draw the surface
	if(!DrawSurface(m_pSurfs[m_nCurrentSurf]))
	{
		Fail();
		WriteToLog(_T("ExecuteTest() - DrawSurface() failed.\n"));
	}

	EndScene();

    return true;
}

// ----------------------------------------------------------------------------

bool CHOSurf::SetupTextures(DWORD dwTex)
{
	// Set the textures
	if(m_bTexture && dwTex != TEX_NONE)
	{
		SetTextureStageState((DWORD)0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
		SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
		SetTextureStageState((DWORD)0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		SetTextureStageState((DWORD)0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
		SetTexture(0, m_pTextures[0]);
        
		if(m_pSrcDevice->GetCaps()->wMaxSimultaneousTextures > 1 && 
		   m_pSrcDevice->GetCaps()->dwTextureOpCaps & D3DTEXOPCAPS_ADD)
		{
			SetTextureStageState((DWORD)1, D3DTSS_TEXCOORDINDEX, dwTex == TEX_COORD0 ? (DWORD)0 : (DWORD)1);
			SetTextureStageState((DWORD)1, D3DTSS_COLOROP, (DWORD)D3DTOP_ADD);
			SetTextureStageState((DWORD)1, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
			SetTextureStageState((DWORD)1, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT);
			SetTexture(1, m_pTextures[1]);
		}
		
	}
	else
	{
		SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_DISABLE);
		SetTexture(0, NULL);
	}
	return true;

}

// ----------------------------------------------------------------------------

bool CHOSurf::TestTerminate(void)
{
	ReleaseTest();
	return CD3DTest::TestTerminate();
}

// ----------------------------------------------------------------------------

void CHOSurf::GetLogTags(TCHAR *pcsName, TCHAR *pcsLine)
{
	_stprintf(pcsName, _T("%s%s, %s, %s, %s"),
			    m_nCurrentSurf == m_nLastSurf ? _T("Cached ") : _T(""),
				m_pSurfs[m_nCurrentSurf]->tcsSurf,
				g_sDegree[(int)m_pSurfs[m_nCurrentSurf]->pTest->HODegree], 
				g_sBasis[(int)m_pSurfs[m_nCurrentSurf]->pTest->HOBasis],
		        m_pSurfs[m_nCurrentSurf]->tcsTess);
	_stprintf(pcsLine, _T("%s, %s, %s, %s"),
              m_pSurfs[m_nCurrentSurf]->tcsShader,
              m_pSurfs[m_nCurrentSurf]->pTest->HOPStyle == D3DPATCHEDGE_DISCRETE ? _T("Discrete") : _T("Continuous"),
			  m_pSurfs[m_nCurrentSurf]->pTest->bSpecular ? _T("On") : _T("Off"), 
			  m_pSurfs[m_nCurrentSurf]->pTest->dwTexture == TEX_NONE ? _T("Off") : (m_pSurfs[m_nCurrentSurf]->pTest->dwTexture == TEX_COORD0 ? _T("On(1 texcoord)") : _T("On(2 texcoords)")));
}

// ----------------------------------------------------------------------------

void CHOSurf::UpdateStatus(void)
{
	CD3DTest::UpdateStatus();

    WriteStatus(_T("$wBASIS, DEGREE"), _T("$a%s, %s"), g_sBasis[(int)m_pSurfs[m_nCurrentSurf]->pTest->HOBasis],
													  g_sDegree[(int)m_pSurfs[m_nCurrentSurf]->pTest->HODegree]);
    WriteStatus(_T("$wSurface Type"), _T("$a%s%s"), 
				m_nCurrentSurf == m_nLastSurf ? _T("Cached ") : _T(""),
				m_pSurfs[m_nCurrentSurf]->tcsSurf);
	WriteStatus(_T("$wTesselation"), _T("$a%s"), m_pSurfs[m_nCurrentSurf]->tcsTess);
    WriteStatus(_T("$wVertex Shader"), _T("$a%s"), m_pSurfs[m_nCurrentSurf]->tcsShader);
	WriteStatus(_T("$wPatch Style"), _T("$a%s"), 
				m_pSurfs[m_nCurrentSurf]->pTest->HOPStyle == D3DPATCHEDGE_DISCRETE ? 
				_T("Discrete") : _T("Continuous"));
	WriteStatus(_T("$wSpecular"), _T("$a%s"), m_pSurfs[m_nCurrentSurf]->pTest->bSpecular ? _T("On") : _T("Off"));
    WriteStatus(_T("$wTexture\t"), _T("$a%s"), m_pSurfs[m_nCurrentSurf]->pTest->dwTexture == TEX_NONE ? _T("Off") : (m_pSurfs[m_nCurrentSurf]->pTest->dwTexture == TEX_COORD0 ? _T("On(1 texcoord)") : _T("On(2 texcoords)")));

	// Set this as the last surface
    m_nLastSurf = m_nCurrentSurf;
}

// ----------------------------------------------------------------------------

void CHOSurf::TweakGeometry(HOTEST *pTest, D3DVERTEX *pVertices)
{
	switch(pTest->PatchType)
	{
	case WART:
	case DIMPLE:
		{
			for(int i=0; i<pTest->nHeight; i++)
			for(int j=0; j<pTest->nWidth; j++)
			{
				int nOffset = i * pTest->nHeight + j;
				float fDist = sin((float)i / (float)(pTest->nHeight-1) * pi) * sin((float)j / (float)(pTest->nWidth-1) * pi);
				pVertices[nOffset].z = pTest->PatchType == WART ? -fDist : fDist;
			}
		}
		break;
	case WAVE:
		{
			float fInc = pi / (float)pTest->HODegree;
			float fCosOffset = pi / 2.f;
			for(int i=0; i<pTest->nHeight; i++)
			for(int j=0; j<pTest->nWidth; j++)
			{
				int nOffset = i * pTest->nHeight + j;
				pVertices[nOffset].z += (sin(fInc * i) + cos(fCosOffset + fInc * j)) / 2.f;
			}
		}
		break;
	case FAN:
		{
			//float fInc = pi / (float)pTest->HODegree;
			//float fCosOffset = pi / 2.f;
			for(int i=0; i<pTest->nHeight; i++)
			for(int j=0; j<pTest->nWidth; j++)
			{
				int nOffset = i * pTest->nHeight + j;
				//pVertices[nOffset].z += (sin(fInc * i) + cos(fCosOffset + fInc * j)) / 2.f;
				if(j & 1)
					pVertices[nOffset].z += 2;
			}
		}
		break;
	case DEGEN0:
		{
			for(int i=0; i<pTest->nHeight; i++)
			for(int j=0; j<pTest->nWidth; j++)
			{
                int nOffset = i * pTest->nHeight + j;
				pVertices[nOffset].z = pVertices[0].z;
				pVertices[nOffset].y = pVertices[0].y;
				pVertices[nOffset].x = pVertices[0].x;
			}
		}
		break;
	case PLANAR:
	default:
		break;
	}
}
// ----------------------------------------------------------------------------



bool CHOSurf::DrawD3DXRectPatch(CnVertexBuffer* pNVB, const float* pfEdgeTess, UINT uShaderIndex, const D3DRECTPATCH_INFO* pRectInfo, UINT nDevice)
{
	HRESULT hr;
	bool bRes = true;

    CDevice* 	pDevice = GetDevice(nDevice);
	if(!pDevice)
	{
		WriteToLog(_T("HOSurf::DrawD3DXRectPatch() - Error getting device# %d.\n"), nDevice);
		return false;
	}

	// Output mesh:
	LPD3DXMESH pMesh = 0;
	VERTEXELEMENT pInDecl[MAX_FVF_DECL_SIZE];
	VERTEXELEMENT pOutDecl[MAX_FVF_DECL_SIZE];
	UINT nDeclElements = MAX_FVF_DECL_SIZE;
	CVertexBuffer* pVB = 0;
	CVertexDeclaration* pDecl = 0;
	IDirect3DVertexBuffer9* pTessVB = 0;
	IDirect3DIndexBuffer9* pTessIB = 0;
	CVertexBuffer* vb = 0;
	CIndexBuffer* ib = 0;
	DWORD dwTessFaces, dwTessVertices;

	if(!m_pRawDecls[uShaderIndex] || !m_pRawDecls_Out[uShaderIndex])
	{
		WriteToLog(_T("HOSurf::DrawD3DXRectPatch() - Error: in or out Decl for index %d is NULL.\n"), uShaderIndex);
		goto failed;
	}

	D3DXGenerateOutputDecl((D3DVERTEXELEMENT9*)pOutDecl, (D3DVERTEXELEMENT9*)m_pRawDecls[uShaderIndex]);

	if(!pNVB->GetVertexBuffer(&pVB, nDevice))
	{
		WriteToLog(_T("HOSurf::DrawD3DXRectPatch() - Error getting CVertexBuffer for device# %d.\n"), nDevice);
		goto failed;
	}

	// first create a mesh to tessellate into:
	// getting the size first:
	if(FAILED(hr = D3DXRectPatchSize(pfEdgeTess, &dwTessFaces, &dwTessVertices)))
	{
		WriteToLog(_T("HOSurf::DrawD3DXRectPatch() - Error: D3DXRectPatchSize failed. hr = %s.\n"), m_pD3D->HResultToString(hr));
		goto failed;
	}

	// now create the mesh:
	if(FAILED(hr = m_pSrcDevice->D3DXCreateMesh(dwTessFaces, dwTessVertices, D3DXMESH_SYSTEMMEM, (DWORD*)(m_pRawDecls_Out[uShaderIndex]), &pMesh)))
	{
		WriteToLog(_T("HOSurf::DrawD3DXRectPatch() - Error: D3DXRectPatchSize failed. hr = %s.\n"), m_pD3D->HResultToString(hr));
		goto failed;
	}

	// Tessellate the patch and render the polygons		(may also put NULL in place of pOutDecl)
	//if(FAILED(hr = pDevice->D3DXTessellateRectPatch(pVB, pfEdgeTess, m_pRawDecls[uShaderIndex], m_pRawDecls_Out[uShaderIndex], pRectInfo, &pMesh)))
	if(FAILED(hr = pDevice->D3DXTessellateRectPatch(pVB, pfEdgeTess, m_pRawDecls[uShaderIndex], pRectInfo, pMesh)))
//	if(FAILED(hr = pDevice->D3DXTessellateRectPatch(pVB, pfEdgeTess, pInDecl, NULL, pRectInfo, &pMesh)))
	{
		WriteToLog(_T("HOSurf::DrawD3DXRectPatch() - Error: D3DXTessellateRectPatch failed. hr = %s.\n"), m_pD3D->HResultToString(hr));
		goto failed;
	}

	if(!pMesh)
	{
		WriteToLog(_T("HOSurf::DrawD3DXRectPatch() - Error: pMesh null after D3DXTessellateRectPatch successful.\n"));
		goto failed;
	}

	if(FAILED(hr = pMesh->DrawSubset(0)))
	{
		WriteToLog(_T("HOSurf::DrawD3DXRectPatch() - Error: ID3DXMesh::DrawSubset failed. hr = %s.\n"), m_pD3D->HResultToString(hr));
		goto failed;
	}

/*
	// Get VB/IB from mesh and stick them in their wrapper objects, then draw the triangles...
	if(FAILED(hr = pMesh->GetVertexBuffer(&pTessVB)) || FAILED(hr = pMesh->GetIndexBuffer(&pTessIB)))
	{
		WriteToLog(_T("HOSurf::DrawD3DXTriPatch() - Error: ID3DXMesh::GetVertex/IndexBuffer failed. hr = %s.\n"), m_pD3D->HResultToString(hr));
		goto failed;
	}


	vb = new CVertexBuffer();
	ib = new CIndexBuffer();
	if(!vb || !ib)
	{
		WriteToLog(_T("HOSurf::DrawD3DXRectPatch() - Allocation of C[Vertex/Index]Buffers failed.\n"));
		goto failed;
	}
	if(!vb->Initialize(pTessVB, pDevice) || !ib->Initialize(pTessIB, pDevice))
	{
		WriteToLog(_T("HOSurf::DrawD3DXRectPatch() - Unable to initialize C[Vertex/Index]Buffers.\n"));
		goto failed;
	}

	pDevice->SetStreamSource(0, vb, GetVertexSize(pMesh->GetFVF()));
	pDevice->SetIndices(ib, 0);

	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, pMesh->GetNumVertices(), 0, pMesh->GetNumFaces());
*/

	goto done;

failed:
	bRes = false;

done:
	RELEASE(pMesh);
	RELEASE(pDecl);
	RELEASE(pVB);
	/*RELEASE(pTessVB);
	RELEASE(pTessIB);
	RELEASE(vb);
	RELEASE(ib);*/

	return bRes;
}

bool CHOSurf::DrawD3DXTriPatch(CnVertexBuffer* pNVB, const float* pfEdgeTess, UINT uShaderIndex, const D3DTRIPATCH_INFO* pTriInfo, UINT nDevice)
{
	HRESULT hr;
	bool bRes = true;

    CDevice* 	pDevice = GetDevice(nDevice);
	if(!pDevice)
	{
		WriteToLog(_T("HOSurf::DrawD3DXTriPatch() - Error getting device# %d.\n"), nDevice);
		return false;
	}

	// Output mesh:
	LPD3DXMESH pMesh = 0;
	VERTEXELEMENT pInDecl[MAX_FVF_DECL_SIZE];
	VERTEXELEMENT pOutDecl[MAX_FVF_DECL_SIZE];
	UINT nDeclElements = MAX_FVF_DECL_SIZE;
	CVertexDeclaration* pDecl = 0;
	CVertexBuffer* pVB = 0;
	IDirect3DVertexBuffer9* pTessVB = 0;
	IDirect3DIndexBuffer9* pTessIB = 0;
	DWORD dwTessFaces, dwTessVertices;

	CVertexBuffer* vb = 0;
	CIndexBuffer* ib = 0;

	// Used later for debugging purposes:
	BYTE* pData = NULL;

	if(!m_pRawDecls[uShaderIndex] || !m_pRawDecls_Out[uShaderIndex])
	{
		WriteToLog(_T("HOSurf::DrawD3DXTriPatch() - in or out Decl for index %d is NULL.\n"), uShaderIndex);
		goto failed;
	}

	D3DXGenerateOutputDecl((D3DVERTEXELEMENT9*)pOutDecl, (D3DVERTEXELEMENT9*)m_pRawDecls[uShaderIndex]);

	if(!pNVB->GetVertexBuffer(&pVB, nDevice))
	{
		WriteToLog(_T("HOSurf::DrawD3DXTriPatch() - Error getting CVertexBuffer for device# %d.\n"), nDevice);
		goto failed;
	}

	// first create a mesh to tessellate into:
	// getting the size first:
	if(FAILED(hr = D3DXTriPatchSize(pfEdgeTess, &dwTessFaces, &dwTessVertices)))
	{
		WriteToLog(_T("HOSurf::DrawD3DXRectPatch() - Error: D3DXTriPatchSize failed. hr = %s.\n"), m_pD3D->HResultToString(hr));
		goto failed;
	}

	// now create the mesh:
	if(FAILED(hr = m_pSrcDevice->D3DXCreateMesh(dwTessFaces, dwTessVertices, D3DXMESH_SYSTEMMEM, (DWORD*)(m_pRawDecls_Out[uShaderIndex]), &pMesh)))
	{
		WriteToLog(_T("HOSurf::DrawD3DXRectPatch() - Error: D3DXRectPatchSize failed. hr = %s.\n"), m_pD3D->HResultToString(hr));
		goto failed;
	}

/*	// Should the following prevent future rendering?  I didn't think it should!  But it seems to
	pVB->Lock(0, pTriInfo->NumVertices, (void**)&pData, D3DLOCK_NOSYSLOCK);
	pVB->Unlock();
*/
	// Tessellate the patch and render the polygons
	if(FAILED(hr = pDevice->D3DXTessellateTriPatch(pVB, pfEdgeTess, m_pRawDecls[uShaderIndex], pTriInfo, pMesh)))
	{
		WriteToLog(_T("HOSurf::DrawD3DXTriPatch() - Error: D3DXTessellateTriPatch failed. hr = %s.\n"), m_pD3D->HResultToString(hr));
		goto failed;
	}

	if(!pMesh)
	{
		WriteToLog(_T("HOSurf::DrawD3DXTriPatch() - Error: pMesh null after D3DXTessellateTriPatch successful.\n"));
		goto failed;
	}

	if(FAILED(hr = pMesh->DrawSubset(0)))
	{
		WriteToLog(_T("HOSurf::DrawD3DXTriPatch() - Error: ID3DXMesh::DrawSubset failed. hr = %s.\n"), m_pD3D->HResultToString(hr));
		goto failed;
	}

/*	// Get VB/IB from mesh and stick them in their wrapper objects, then draw the triangles...
	if(FAILED(hr = pMesh->GetVertexBuffer(&pTessVB)) || FAILED(hr = pMesh->GetIndexBuffer(&pTessIB)))
	{
		WriteToLog(_T("HOSurf::DrawD3DXTriPatch() - Error: ID3DXMesh::GetVertex/IndexBuffer failed. hr = %s.\n"), m_pD3D->HResultToString(hr));
		goto failed;
	}

	vb = new CVertexBuffer();
	ib = new CIndexBuffer();
	if(!vb || !ib)
	{
		WriteToLog(_T("HOSurf::DrawD3DXTriPatch() - Allocation of C[Vertex/Index]Buffers failed.\n"));
		goto failed;
	}
	if(!vb->Initialize(pTessVB, pDevice) || !ib->Initialize(pTessIB, pDevice))
	{
		WriteToLog(_T("HOSurf::DrawD3DXTriPatch() - Unable to initialize C[Vertex/Index]Buffers.\n"));
		goto failed;
	}

	pDevice->SetStreamSource(0, vb, GetVertexSize(pMesh->GetFVF()));
	pDevice->SetIndices(ib, 0);

	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, pMesh->GetNumVertices(), 0, pMesh->GetNumFaces());
*/

	goto done;

failed:
	bRes = false;

done:
	RELEASE(pMesh);
	RELEASE(pDecl);
	RELEASE(pVB);
	/*RELEASE(pTessVB);
	RELEASE(pTessIB);
	RELEASE(vb);
	RELEASE(ib);*/

	return bRes;
}


// ----------------------------------------------------------------------------



void CHOSurf::ReleaseTest()
{
	m_nViewMatrix = 0;
	m_nTPViewMatrix = 0;

    // Release the surfaces
	for(int i=0; i<m_nSurfCount; i++)
	{
		if(m_pSurfs[i])
		{
			RELEASE(m_pSurfs[i]->pVB);
			RELEASE(m_pSurfs[i]->pIB);
			RELEASE(m_pSurfs[i]->pColor);
			if(m_pSurfs[i]->pTest) delete m_pSurfs[i]->pTest;
			delete m_pSurfs[i];
			m_pSurfs[i] = NULL;
		}
	}

	// Delete the surface handles
	DeleteSurfHandles(true);

    // Release the shaders and decls:
	for(int i=0; i<m_nShaders; i++)
	{
		if(m_pShaders8[i])
		{
			DeleteVertexShader(m_pShaders8[i]);
		}
		if(m_pShaders[i]) 
		{
			RELEASE(m_pShaders[i]);
		}
		if(m_pDecls[i])
		{
			RELEASE(m_pDecls[i]);
		}
		if(m_pDecls_Out[i])
		{
			RELEASE(m_pDecls_Out[i]);
		}
	}
    m_nShaders = 0;

	// release the lights
	SAFEDELETEA(m_pLights);
	
	// release the textures
	if(m_pTextures)
	{
		for(int i=0; i<m_nTextureCount; i++)
		{
			if(m_pTextures[i]) 
				m_pTextures[i]->Release();
		}
		delete m_pTextures;
		m_pTextures = NULL;
	}   
}