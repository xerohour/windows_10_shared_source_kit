// ============================================================================
//
//  StateBeast.cpp
//
// ============================================================================
#include "StateBeast.h"
#include "rnumber.h" // random-number generator
#include "CBuffers.h"
#include "Types.h"

#ifdef FAST_TEST
// The tests automatically will test state blocks with begin/end in addition to these types
// This is all runtime testing so not interesting
FLAGS g_SBT[] = { {} };
int g_nSBT = 0;
#else

FLAGS g_SBT[] = { { D3DSBT_ALL, _T("D3DSBT_ALL") },
	{ D3DSBT_PIXELSTATE, _T("D3DSBT_PIXEL") },
	{ D3DSBT_VERTEXSTATE, _T("D3DSBT_VERTEX") } };
int g_nSBT = 3;
#endif



// ----------------------------------------------------------------------------

CStateBeast::CStateBeast()
{ 
    m_nViewports  = 2;
    m_dwVSShader = m_dwVSShader2 = m_dwVSShader3 = 0;
    m_dwPSShader = 0;
	m_pVB = NULL;
    m_pIB = NULL;
	m_bCmp = false;
    m_dwFVF = D3DFVF_VERTEX;
    m_nStates = 40;
	m_nTotalBlocks = m_nStates * (g_nSBT + 1);

	// alloc the view data
	m_pVMatrix = new D3DXMATRIX;
	m_pvLoc = new D3DXVECTOR3;
	m_pvUp = new D3DXVECTOR3;
    m_pProjMatrix = new D3DXMATRIX;

	// Set the test name -- different for stress mode
	if(KeySet(_T("stress")))
	{
		SetTestType(TESTTYPE_STRESS);
		m_szTestName =	_T("StateBeast");
        m_szLogName = _T("StateBeast");
		m_szCommandKey=	_T("StateBeast");
	}
	else if(KeySet(_T("cmp")))
	{
		SetTestType(TESTTYPE_FUNC);
		m_bCmp = true;
		m_szTestName = _T("StateCmp");
        m_szLogName = _T("StateCmp");
		m_szCommandKey = _T("StateCmp");
		m_Options.D3DOptions.bRefEqualsSrc = true;
		g_nSBT = 1;  // Disable Pixel/Vertex types - compare not valid
	}
	else
	{
    	m_szCommandKey=	_T("StateBlocks");
        m_szLogName = _T("StateBlocks");
		m_szTestName=	_T("StateBlocks");

		SetTestType(TESTTYPE_CONF);
	}

    m_Options.D3DOptions.fMinDXVersion = 7.f;
}


// ----------------------------------------------------------------------------

CStateBeast::~CStateBeast()
{
	// release the view data
    SAFEDELETE(m_pProjMatrix);
	SAFEDELETE(m_pVMatrix);
    SAFEDELETE(m_pvLoc);
    SAFEDELETE(m_pvUp);

}

// ----------------------------------------------------------------------------

bool CStateBeast::SetDefaultRenderStates(void)
{
	// Get the current screen extents
    int nWidth = m_pCurrentMode->nBackBufferWidth;
    int nHeight = m_pCurrentMode->nBackBufferHeight;

    // Create the viewports
	if(m_nViewports > 0 )
	{
		m_pViewports = new VIEWPORT[m_nViewports];
		if(m_pViewports)
		{
			// Store the default viewport as VP 0
			memset(&m_pViewports[0],0, sizeof(SVIEWPORT));
			m_pViewports[0].Width = nWidth;
			m_pViewports[0].Height = nHeight;
			m_pViewports[0].X = 0;
			m_pViewports[0].Y = 0;
			m_pViewports[0].MinZ = 0.f;
			m_pViewports[0].MaxZ = 1.f;

			// Create some other interesting viewports
			memset(&m_pViewports[1], 0, sizeof(SVIEWPORT));
			m_pViewports[1].Width = nWidth / 2;
			m_pViewports[1].Height = nHeight / 2;
			m_pViewports[1].X = m_pViewports[1].Width / 2;
			m_pViewports[1].Y = m_pViewports[1].Height / 2;
			m_pViewports[1].MinZ = 0.f;
			m_pViewports[1].MaxZ = 1.f;
		}
		else
		{
			WriteToLog(_T("SetDefaultRenderStates() - couldn't allocate viewports.\n"));
			return false;
		}
	}

    return true;
}

// ----------------------------------------------------------------------------

bool CStateBeast::SetDefaultMatrices(void)
{
    D3DXMATRIX           Matrix;
    D3DXVECTOR3         at(0.f, 0.f, 0.f);

    *m_pvLoc = D3DXVECTOR3(0.f,0.f,-2.5f);
    *m_pvUp  = D3DXVECTOR3(0.0f,1.0f,0.0f);

	D3DXMatrixPerspectiveFovLH(m_pProjMatrix, pi/4.0f, 1.0f, 1.0f, 1000.0f);
    if (!SetTransform(D3DTS_PROJECTION,m_pProjMatrix))
        return false;

    D3DXMatrixIdentity(&Matrix);
    if (!SetTransform(D3DTS_WORLD, &Matrix))
        return false;

    D3DXMatrixLookAtLH(m_pVMatrix, m_pvLoc, &at, m_pvUp);
    if(!SetTransform(D3DTS_VIEW, m_pVMatrix))
        return false;

    return true;
}

// ----------------------------------------------------------------------------

HRESULT CStateBeast::LoadTextures(void)
{
	HRESULT hr = S_OK;

    const TCHAR   ptcsTextures[NTEXTURES][14] = {
			_T("checker.BMP"),
            _T("Poko0.BMP"),
            _T("Poko1.BMP"),
            _T("Poko2.BMP"),
            _T("Poko3.BMP"),
            _T("Poko4.BMP"),
            _T("Poko5.BMP"),
            _T("Poko6.BMP"),
            _T("Poko7.BMP")};

    if (m_pCurrentMode->nTextureFormat >= m_uCommonTextureFormats)
    {
        WriteToLog(_T("Requested texture format (%d) out of range (%d).\n"),
                   m_pCurrentMode->nTextureFormat, m_uCommonTextureFormats);
        hr = E_INVALIDARG;
		goto Exit;
    }
    else if (!m_pCommonTextureFormats)
    {
        WriteToLog(_T("No texture formats available.\n"));
        hr = E_INVALIDARG;
		goto Exit;
    }

	for(int i = 0; i < NTEXTURES; i++)
	{
        // Create the texture
        if(!CreateTexture(64, 64, 1, 0, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat], FMWK_DEFAULT, &(m_pTextures[i])) ||
			m_pTextures[i] == NULL)
		{
			WriteToLog(_T("CStateBeast::SetDefaultMaterial() - Create texture failed.\n"));
			hr = GetLastError();
			goto Exit;
		}

		// Set the palette if needed
		if(m_pTextures[i]->IsPalettized())
		{
			if (!m_pTextures[i]->SetPalette(NULL))
			{
				WriteToLog("SetPalette() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
				hr = GetLastError();
				goto Exit;
			}
		}

		// Load an image into the texture
        m_pTextures[i]->LoadTexture(0, ptcsTextures[i], 0.75f);

    }

Exit:
	return hr;
}

// ----------------------------------------------------------------------------

bool CStateBeast::SetDefaultMaterials(void)
{
	// Create the default materials
    m_pFrontMat = new MATERIAL;
    memset(m_pFrontMat, 0 , sizeof(MATERIAL));
    m_pFrontMat->diffuse.r = 1.0f;
    m_pFrontMat->diffuse.g = 1.0f;
    m_pFrontMat->diffuse.b = 1.0f;
    m_pFrontMat->diffuse.a = 0.5f;
    m_pFrontMat->ambient.r = 0.1f;
    m_pFrontMat->ambient.g = 0.4f;   
    m_pFrontMat->ambient.b = 0.1f; 
	m_pFrontMat->specular.r = 1.f;
    m_pFrontMat->specular.g = 1.f;
    m_pFrontMat->specular.b = 1.f;
    m_pFrontMat->power = 10.0f;

    m_pFrontMat2 = new MATERIAL;
    memset(m_pFrontMat2, 0 , sizeof(MATERIAL));
    m_pFrontMat2->diffuse.r = 1.0f;
    m_pFrontMat2->diffuse.g = 0.0f;
    m_pFrontMat2->diffuse.b = 0.0f;
    m_pFrontMat2->diffuse.a = 0.5f;
    m_pFrontMat2->ambient.r = 1.0f;
    m_pFrontMat2->ambient.g = 0.0f;   
    m_pFrontMat2->ambient.b = 0.0f; 
	m_pFrontMat->specular.r = 1.f;
    m_pFrontMat->specular.g = 1.f;
    m_pFrontMat->specular.b = 1.f;
    m_pFrontMat2->power = 10.0f;
	
    m_pBackMat = new MATERIAL;
    memset(m_pBackMat, 0 , sizeof(MATERIAL));
    m_pBackMat->diffuse.r = 1.0f;
    m_pBackMat->diffuse.g = 1.0f;
    m_pBackMat->diffuse.b = 1.0f;
    m_pBackMat->diffuse.a = 0.5f;
    m_pBackMat->ambient.r = 1.0f;
    m_pBackMat->ambient.g = 0.0f;   
    m_pBackMat->ambient.b = 0.0f; 
	m_pFrontMat->specular.r = 1.f;
    m_pFrontMat->specular.g = 1.f;
    m_pFrontMat->specular.b = 1.f;
    m_pBackMat->power = 10.0f;

    return true;
}


// ----------------------------------------------------------------------------

bool CStateBeast::SetDefaultLights()
{   
    // Create the CLIGHTS
    for(int i=0; i<NLIGHTS; i++)
    {
		m_pLight[i] = new LIGHT();
		memset(m_pLight[i], 0, sizeof(LIGHT));
	}
    
    // Check what kind of lights are available
    if(m_pSrcDevice->GetCaps()->dwDevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        m_dwPosLightsSupported = (m_pSrcDevice->GetCaps()->dwVertexProcessingCaps & D3DVTXPCAPS_POSITIONALLIGHTS);

        if(!m_dwPosLightsSupported && !(m_pSrcDevice->GetCaps()->dwVertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS))
        {
            WriteToLog("(FAIL) T&L Device does not support at least Directional Lights.  Test cannot continue.\n");
            return false;
        }
    }
	else
		m_dwPosLightsSupported = TRUE;


    // Light0
    m_pLight[0]->dltType             = m_dwPosLightsSupported ? D3DLIGHT_POINT : D3DLIGHT_DIRECTIONAL;
    m_pLight[0]->dcvDiffuse.r        = 1.0f;
    m_pLight[0]->dcvDiffuse.g        = 1.0f;
    m_pLight[0]->dcvDiffuse.b        = 1.0f;
    m_pLight[0]->dcvAmbient.r        = 1.0f;
    m_pLight[0]->dcvAmbient.g        = 1.0f;
    m_pLight[0]->dcvAmbient.b        = 1.0f;
    m_pLight[0]->dcvSpecular.r       = 1.0f;
    m_pLight[0]->dcvSpecular.g       = 1.0f;
    m_pLight[0]->dcvSpecular.b       = 1.0f;
    m_pLight[0]->dvPosition       = D3DXVECTOR3(1.0f, 0.0f, -0.5f);
    m_pLight[0]->dvDirection      = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
    m_pLight[0]->dvAttenuation0   = 0.98f;
    m_pLight[0]->dvAttenuation1   = 0.015f;
    m_pLight[0]->dvAttenuation2   = 0.005f;
    m_pLight[0]->dvRange          = 15.0f;
                          
    // Light1
    m_pLight[1]->dltType             = m_dwPosLightsSupported ? D3DLIGHT_SPOT : D3DLIGHT_DIRECTIONAL;
	m_pLight[1]->dcvDiffuse.r        = 0.0f;
    m_pLight[1]->dcvDiffuse.g        = 0.0f;
    m_pLight[1]->dcvDiffuse.b        = 1.0f;
	m_pLight[1]->dcvAmbient.r        = 1.0f;
    m_pLight[1]->dcvAmbient.g        = 1.0f;
    m_pLight[1]->dcvAmbient.b        = 1.0f;
	m_pLight[1]->dcvSpecular.r       = 1.0f;
    m_pLight[1]->dcvSpecular.g       = 1.0f;
    m_pLight[1]->dcvSpecular.b       = 0.0f;
    m_pLight[1]->dvPosition       = D3DXVECTOR3(-1.0f, 0.0f, -1.0f);
    m_pLight[1]->dvDirection      = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
    m_pLight[1]->dvAttenuation0   = 0.98f;
    m_pLight[1]->dvAttenuation1   = 0.015f;
    m_pLight[1]->dvAttenuation2   = 0.005f;
	m_pLight[1]->dvRange             = 15.0f;
    m_pLight[1]->dvTheta             = pi / 2;
    m_pLight[1]->dvPhi               = pi;
    m_pLight[1]->dvFalloff           = 1.0f;

    // Light2
	m_pLight[2]->dltType             = m_dwPosLightsSupported ? D3DLIGHT_SPOT : D3DLIGHT_DIRECTIONAL;
	m_pLight[2]->dcvDiffuse.r        = 1.0f;
    m_pLight[2]->dcvDiffuse.g        = 0.0f;
    m_pLight[2]->dcvDiffuse.b        = 0.0f;
	m_pLight[2]->dcvAmbient.r        = 1.0f;
    m_pLight[2]->dcvAmbient.g        = 1.0f;
    m_pLight[2]->dcvAmbient.b        = 1.0f;
    m_pLight[2]->dcvSpecular.r       = 0.0f;
    m_pLight[2]->dcvSpecular.g       = 0.0f;
    m_pLight[2]->dcvSpecular.b       = 1.0f;
    m_pLight[2]->dvPosition       = D3DXVECTOR3(1.0f, 0.0f, -1.0f);
    m_pLight[2]->dvDirection      = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_pLight[2]->dvAttenuation0   = 0.98f;
    m_pLight[2]->dvAttenuation1   = 0.015f;
    m_pLight[2]->dvAttenuation2   = 0.005f;
	m_pLight[2]->dvRange             = 15.0f;
    m_pLight[2]->dvTheta             = pi / 2;
    m_pLight[2]->dvPhi               = pi;
    m_pLight[2]->dvFalloff           = 1.0f;

	// Light3
	m_pLight[3]->dltType          = m_dwPosLightsSupported ? D3DLIGHT_SPOT : D3DLIGHT_DIRECTIONAL;
	m_pLight[3]->dcvDiffuse.r        = 0.0f;
    m_pLight[3]->dcvDiffuse.g        = 1.0f;
    m_pLight[3]->dcvDiffuse.b        = 0.0f;
	m_pLight[3]->dcvAmbient.r        = 1.0f;
    m_pLight[3]->dcvAmbient.g        = 1.0f;
    m_pLight[3]->dcvAmbient.b        = 1.0f;
    m_pLight[3]->dcvSpecular.r       = 1.0f;
    m_pLight[3]->dcvSpecular.g       = 1.0f;
    m_pLight[3]->dcvSpecular.b       = 1.0f;
    m_pLight[3]->dvPosition          = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	m_pLight[3]->dvDirection         = D3DXVECTOR3(0.0f,0.0f,1.0f);
	m_pLight[3]->dvAttenuation0   = 0.98f;
    m_pLight[3]->dvAttenuation1   = 0.015f;
    m_pLight[3]->dvAttenuation2   = 0.005f;
    m_pLight[3]->dvRange          = 15.0f;
	m_pLight[3]->dvTheta             = pi / 4;
    m_pLight[3]->dvPhi               = pi / 3;
    m_pLight[3]->dvFalloff           = 1.0f;

    return true;
}

// ----------------------------------------------------------------------------

bool CStateBeast::PreDeviceSetup(bool fReference)
{
	if(!KeySet(_T("stress")))
	{
        //
		// Disable EmulateStateBlocks regkey if needed
		// This must be done before the D3DDevice is initialized
		//
		
		HRESULT hr;
		HKEY hKey = (HKEY) NULL;

		hr = RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Direct3D", &hKey); 
		if(hr == ERROR_SUCCESS)
		{
			hr = RegDeleteValue(hKey, "EmulateStateBlocks");
			
			if(hr != ERROR_SUCCESS && hr != ERROR_FILE_NOT_FOUND)
			{
				WriteToLog("Couldn't delete EmulateStateBlocks regkey. hr=%x\n", hr);
				return false;
			}
	
			RegCloseKey(hKey);
		}
		else
		{
			WriteToLog("Can't open Software\\Microsoft\\Direct3D regkey. D3D installed? hr=%x\n", hr);
			return false;        
		}
	}
	return true;
}

// ----------------------------------------------------------------------------

void CStateBeast::CommandLineHelp(void)
{
    WriteCommandLineHelp("$y-TESTBLOCK <int>\t$wTest a single stateblock");
    WriteCommandLineHelp("$y-STARTWITH <int>\t$wStart execution at a specific stateblock");
    WriteCommandLineHelp("$y-SWAP <int>\t$wSet the interval for swapping states");
    WriteCommandLineHelp("$y-BEASTS <int>\t$wSet the number of beasts");
    WriteCommandLineHelp("$y-!CYCLE\t$wDo not cylce the state blocks");
    WriteCommandLineHelp("$y-SFEMUL\t$wDo not enforce DX7 DDI checking");
    WriteCommandLineHelp("$y-!STATIC\t$wSet objects in motion");           
}

// ----------------------------------------------------------------------------

UINT CStateBeast::TestInitialize(void)
{              
	HRESULT hr = S_OK;

    // Allow software emulation if not supported in HW
    m_bSWEmulation = KeySet(_T("SFEmul")) ? true : false;
    
    // 
    // Validate the DX version, device caps
    //
   
    // Make sure we are on DX7
	if (m_dwVersion < 0x0700)
	{
        SetTestRange(1,1);

        WriteToLog(_T("This test requires Dx7.\n"));
		ReleaseTest();     
        return D3DTESTINIT_SKIPALL;
	}
    
	if(!KeySet(_T("stress")))
	{
		// Make sure that we're on a DX7 DDI
		DWORD dwDevCaps = m_pSrcDevice->GetCaps()->dwDevCaps;
		if (!(dwDevCaps & D3DDEVCAPS_DRAWPRIMITIVES2EX))
		{   
			if(m_bSWEmulation)
			{
				WriteToLog(_T("No DX7 DDI available, running with software emulation.\n"));
			}
			else
			{
				SetTestRange(1,1);

				WriteToLog(_T("\nTestInitialize(ERROR) - This test requires a Dx7 Driver. Exiting.\n"));
				ReleaseTest();
				return D3DTESTINIT_SKIPALL;
			}
		}
	}
	
	// Generate the geometry
	m_FrontPlane.NewShape(CS_MESH, 3, 3);
	m_FrontPlane.Scale(2.f,2.f,2.f);
    m_FrontPlane2.NewShape(CS_MESH, 3, 3);
    m_FrontPlane2.Scale(2.f,2.f,2.f);
    m_FrontPlane2.Translate(0,0,0.1f);
    m_BackPlane.NewShape(CS_MESH, 3, 3);
	m_BackPlane.Scale(2.f,2.f,2.f);
	m_BackPlane.Rotate(0, pi, 0);
	m_BackPlane.Translate(0,0,0.2f);
	if(m_dwVersion > 0x700)
	{
       	// Create the VB/IB
		if(!SetupBuffers(&m_FrontPlane, &m_pVB, &m_pIB) || NULL == m_pVB || NULL == m_pIB)
		{
			WriteToLog(_T("CStateBeast::TestInitialize() - buffer setup failed."));
			return D3DTESTINIT_ABORT;
		}
	}
	// Load the textures
    hr = LoadTextures();
	if(FAILED(hr))
	{
		SetTestRange(1,1);
		return hr == D3DERR_OUTOFVIDEOMEMORY ? D3DTESTINIT_SKIPALL : D3DTESTINIT_ABORT;
	}

	// Set the test range
	SetTestRange( 1, m_nTotalBlocks);


    return D3DTESTINIT_RUN;
}

// ----------------------------------------------------------------------------

void CStateBeast::ClearState()
{
	for(int i=0; i<NLIGHTS; i++)
		LightEnable(i, FALSE);

	ApplyStateBlock(m_OriginalBlock.dwSrcHandle);

    if(m_dwVersion > 0x700)
	{
		SetVertexShader(m_dwFVF);
	}
}

// ----------------------------------------------------------------------------

void CStateBeast::PostApply(UINT nBlock)
{
	DWORD dwSBT = m_pBlocks[m_nCurrentBlock].dwCreateType;
	int nState = nBlock / (g_nSBT + 1);
	D3DXMATRIX mat;
	D3DXMatrixMultiply( &mat, (D3DXMATRIX*)m_pVMatrix, (D3DXMATRIX*)m_pProjMatrix );
	D3DXMatrixTranspose( &mat, &mat );
	BOOL bConst[] = { TRUE, FALSE, TRUE, FALSE };
	int  nConst[] = {4,3,2,1};

	switch(nState)
	{
    case 10:
		SetRenderState(D3DRS_AMBIENT,RGBA_MAKE(255,255,255,0));
		SetMaterial(m_pFrontMat2);
        // Render the second plane
		if(!RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
						m_FrontPlane2.m_pVertices, 
						m_FrontPlane2.m_nVertices,
						m_FrontPlane2.m_pIndices, 
						m_FrontPlane2.m_nIndices,
						0))
		{
			WriteToLog(_T("CStateBeast::SceneRefresh -- RenderPrimitive failed, hr=%s\n"),
						m_pD3D->HResultToString(GetLastError()));
			Fail();
		}
		break;
	case 2:
	case 3:
	case 7:
	case 11:
	case 18:
	case 21:
	case 22:
	case 24:
	case 25:
		{
			SetRenderState(D3DRS_AMBIENT,RGBA_MAKE(255,255,255,0));
		}
		break;
	case 26:
		{
			DWORD dwSrc;
			BeginStateBlock();
            SetTexture( 0, NULL);
			EndStateBlock(&dwSrc);
			DeleteStateBlock(dwSrc);
		}
		break;
	case 28:
		{
			SetVertexShaderConstant(0, &mat, 4);
			SetVertexShader(m_dwVSShader);
			SetStreamSource(0, m_pVB, GetVertexSize(m_dwFVF));
			SetIndices(m_pIB, 0);
		}
		break;
	case 29:
		{
			SetVertexShaderConstant(0, &mat, 4);
			D3DXVECTOR4 v(1.f, 1.f, 0.f, 1.f );
			SetVertexShaderConstant(4, &v, 1);
			SetStreamSource(0, m_pVB, GetVertexSize(m_dwFVF));
			SetIndices(m_pIB, 0);
		}
		break;
	case 30:
		{
			SetVertexShaderConstant(0, &mat, 4);
			D3DXVECTOR4 v(1.f, 0.f, 0.f, 1.f );
			SetVertexShaderConstant(4, &v, 1);
			SetVertexShader(m_dwVSShader);
			SetIndices(m_pIB, 0);
			if(dwSBT == D3DSBT_PIXELSTATE || dwSBT == D3DSBT_VERTEXSTATE)
			{
				UINT uStride;
				CVertexBuffer *pVB = NULL;
				m_pSrcDevice->GetStreamSource(0, &pVB, &uStride);
				if(pVB != NULL)
				{
					Fail();
					WriteToLog(_T("ExecuteTest() - SetStreamSource shouldn't be captured by this block.\n"));
				}
				RELEASE(pVB);
				SetStreamSource(0, m_pVB, GetVertexSize(m_dwFVF));
			}
		}
		break;
	case 31:
		{
			SetVertexShaderConstant(0, &mat, 4);
			D3DXVECTOR4 v(1.f, 0.f, 1.f, 1.f);
			SetVertexShaderConstant(4, &v, 1);
			SetVertexShader(m_dwVSShader);
			SetStreamSource(0, m_pVB, GetVertexSize(m_dwFVF));
			if(dwSBT == D3DSBT_PIXELSTATE || dwSBT == D3DSBT_VERTEXSTATE)
			{
				UINT uIndex = 0;
				CIndexBuffer *pIB = NULL;
				m_pSrcDevice->GetIndices(&pIB, &uIndex);
				if(pIB != NULL || uIndex != 0)
				{
					Fail();
					WriteToLog(_T("ExecuteTest() - SetIndices shouldn't be captured by this block.\n"));
				}
				RELEASE(pIB);
				SetIndices(m_pIB, 0);
			}
		}
		break;
	case 32:
		{
			SetPixelShader(m_dwPSShader);
			SetStreamSource(0, m_pVB, GetVertexSize(m_dwFVF));
			SetIndices(m_pIB, 0);
		}
		break;
	case 33:
		{
			D3DXVECTOR4 v(0.f, 1.f, 1.f, 1.f);
			SetPixelShaderConstant(0, &v, 1);
			SetStreamSource(0, m_pVB, GetVertexSize(m_dwFVF));
			SetIndices(m_pIB, 0);
		}
		break;
	case 34:
	case 35:
		{
			SetVertexShaderConstant(0, &mat, 4);
			SetPixelShader(m_dwPSShader3);
			SetVertexShader(m_dwVSShader3);
			SetStreamSource(0, m_pVB, GetVertexSize(m_dwFVF));
			SetIndices(m_pIB, 0);
			if(nState == 34)
				SetPixelShaderConstantB(0, bConst, 1);
			else
				SetPixelShaderConstantI(0, nConst, 1);
		}
		break;
	case 36:
	case 37:
		{
			SetVertexShaderConstant(0, &mat, 4);
			SetVertexShader(m_dwVSShader2);
			SetStreamSource(0, m_pVB, GetVertexSize(m_dwFVF));
			SetIndices(m_pIB, 0);
			if(nState == 36)
				SetVertexShaderConstantB(0, bConst, 1);
			else
				SetVertexShaderConstantI(0, nConst, 1);
		}
		break;
	case 38:
		{
			SetRenderState(D3DRS_AMBIENT,RGBA_MAKE(255,255,255,0));
			SetMaterial(m_pFrontMat2);
			SetVertexShader(m_dwFVF);
			SetStreamSource(0, m_pVB, GetVertexSize(m_dwFVF));
			SetIndices(m_pIB, 0);
		}
		break;
	case 39:
		{
			SetVertexShaderConstant(0, &mat, 4);
			D3DXVECTOR4 v(1.f, 0.f, 1.f, 1.f);
			SetVertexShaderConstant(4, &v, 1);
			SetVertexShader(m_dwVSShader);
			if(dwSBT == D3DSBT_PIXELSTATE || dwSBT == D3DSBT_VERTEXSTATE)
			{
				UINT uIndex = 0;
				CIndexBuffer *pIB = NULL;
				m_pSrcDevice->GetIndices(&pIB, &uIndex);
				if(pIB != NULL || uIndex != 0)
				{
					Fail();
					WriteToLog(_T("ExecuteTest() - SetIndices shouldn't be captured by this block.\n"));
				}
				RELEASE(pIB);
				SetIndices(m_pIB, 0);
				SetStreamSource(0, m_pVB, GetVertexSize(m_dwFVF));
			}
		}
	default:
		break;
	}
}


// ----------------------------------------------------------------------------

bool CStateBeast::ExecuteTest(UINT uTestNumber)
{
	m_nCurrentBlock = uTestNumber-1;

	// Create the stateblock
	CreateSB(uTestNumber);

	// Begin the test case
    BeginTestCase(m_pBlocks[m_nCurrentBlock].tcsDesc, uTestNumber);

	if(!m_pBlocks[m_nCurrentBlock].dwSrcHandle)
	{
		SkipTestRange(uTestNumber, uTestNumber);
		return false;
	}

	// Render the scene
    BeginScene();
    
    // Reset the state
    ClearState();
    
	if(m_bCmp)
	{
		int nState = m_nCurrentBlock / (g_nSBT + 1);
		ApplyStateBlock(m_pBlocks[m_nCurrentBlock].dwSrcHandle);
		SetState(nState, NULL, NULL, CD3D_REF);
	}
	else
	{
		// Apply  the stateblock
		ApplyStateBlock(m_pBlocks[m_nCurrentBlock].dwSrcHandle);
	}

	// Do any post apply setup
	PostApply(m_nCurrentBlock);

	if(m_pBlocks[m_nCurrentBlock].nVersion == 7)
	{
		SetMaterial(m_pFrontMat);
	
		// Render the plane
		if(!RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
							m_FrontPlane.m_pVertices, 
							m_FrontPlane.m_nVertices,
							m_FrontPlane.m_pIndices, 
							m_FrontPlane.m_nIndices,
							NULL))
			{
				WriteToLog(_T("CStateBeast::ExecuteTest -- RenderPrimitive failed, hr=%s\n"),
							m_pD3D->HResultToString(GetLastError()));
				Fail();
			}
	
	
		
		// Render the back plane
		SetMaterial(m_pBackMat);        
		// Render the plane
		if(!RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
							m_BackPlane.m_pVertices, 
							m_BackPlane.m_nVertices,
							m_BackPlane.m_pIndices, 
							m_BackPlane.m_nIndices,
							NULL))
			{
				WriteToLog(_T("CStateBeast::ExecuteTest -- RenderPrimitive failed, hr=%s\n"),
							m_pD3D->HResultToString(GetLastError()));
				Fail();
			}
		
	}
	else
	{
		DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,(DWORD)m_FrontPlane.m_nVertices, 
								 0, (DWORD)m_FrontPlane.m_nIndices / 3);
	}

	EndScene();

    return true;
}

// ----------------------------------------------------------------------------

void CStateBeast::CreateSB(UINT nTest)
{   
	TCHAR tcsBuffer[MAX_PATH];
	memset(tcsBuffer, 0, MAX_PATH * sizeof(TCHAR));
	bool bSuccess;
	int nBlock = nTest -1;
	int nState = nBlock / (g_nSBT +1);
	int nType = nBlock % (g_nSBT +1);
	
	if(m_pBlocks[nBlock].dwSrcHandle != 0)
	{
		DeleteStateBlock(m_pBlocks[nBlock].dwSrcHandle);
		m_pBlocks[nBlock].dwSrcHandle = 0;
	}

	switch(nType)
	{
		case 0:
			{
				// Create the stateblocks with the Begin / End
				BeginStateBlock();
				bSuccess = SetState(nState, tcsBuffer, &(m_pBlocks[nBlock].nVersion));
				EndStateBlock(&(m_pBlocks[nBlock].dwSrcHandle));
				_stprintf(m_pBlocks[nBlock].tcsDesc, _T("Set %s within Begin/End"),tcsBuffer);
				      
				if(!bSuccess)
				{
					DeleteStateBlock(m_pBlocks[nBlock].dwSrcHandle);
					m_pBlocks[nBlock].dwSrcHandle = 0;
					m_pBlocks[nBlock].dwRefHandle = 0;
				}
				m_pBlocks[nBlock].dwCreateType = D3DSBT_CUSTOM;
			}
			break;
		case 1:
		case 2:
		case 3:
			{
				// Create the stateblock
				CreateStateBlock((D3DSTATEBLOCKTYPE)g_SBT[nType-1].dwFlags, &m_pBlocks[nBlock].dwSrcHandle);
				// Set the state
				bSuccess = SetState(nState, tcsBuffer, &(m_pBlocks[nBlock].nVersion));

				_stprintf(m_pBlocks[nBlock].tcsDesc, _T("Capture %s in %s block"),tcsBuffer, g_SBT[nType-1].tcsDesc);
				if(!bSuccess)
				{
					DeleteStateBlock(m_pBlocks[nBlock].dwSrcHandle);
					m_pBlocks[nBlock].dwSrcHandle = 0;
					m_pBlocks[nBlock].dwRefHandle = 0;
				}
				else
				{
					// Capture the new state
					CaptureStateBlock(m_pBlocks[nBlock].dwSrcHandle);
				
					// Reset the state
					ClearState();
				}
				m_pBlocks[nBlock].dwCreateType = g_SBT[nType-1].dwFlags;
			}

		default:
			break;
	}
}

// ----------------------------------------------------------------------------

bool CStateBeast::SetState(int nIndex, TCHAR *szName, int *nVersion, DWORD dwFlags)
{
	CAPS Caps;

	if(NULL != nVersion)
	{
		if(nIndex < 28)
			*nVersion = 7;
		else if (nIndex < 34)
			*nVersion = 8;
		else
			*nVersion = 9;
	}
	if(m_pD3D->GetVersion() >= 9.0)
		m_pSrcDevice->GetDeviceCaps(&Caps);

	switch(nIndex)
	{
	case 0:
        if(szName) _tcscpy(szName, _T("No state(empty)"));
		return true;		  
	case 1:
        if(szName) _tcscpy(szName, _T("RS_AMBIENT"));
		SetRenderState(D3DRS_AMBIENT,RGBA_MAKE(255,255,255,0), dwFlags);
        return true;
	case 2:
		if(szName) _tcscpy(szName, _T("RS_FILLMODE"));
        SetRenderState(D3DRS_FILLMODE, (DWORD) D3DFILL_SOLID, dwFlags);
        return true;
	case 3:
        if(szName) _tcscpy(szName, _T("RS_CULLMODE"));
        SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_CW, dwFlags);
        return true;   
	case 4:
        if(szName) _tcscpy(szName, _T("SetLight, Enable=false"));
		SetLight(0, m_pLight[0], dwFlags);
        LightEnable(0, (DWORD) FALSE, dwFlags);
        SetRenderState(D3DRS_SHADEMODE, (DWORD) D3DSHADE_FLAT, dwFlags);
        return true;
	case 5:
        if(szName) _tcscpy(szName, _T("SetLight, Enable=true, RS_SHADEMODE(GOURAUD)"));
        if(m_pSrcDevice->GetCaps()->dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_COLORGOURAUDRGB)
		{
			SetLight(0, m_pLight[0], dwFlags);
			LightEnable(0, (DWORD) TRUE, dwFlags);
			SetRenderState(D3DRS_SHADEMODE, (DWORD) D3DSHADE_GOURAUD, dwFlags);
			SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_CCW, dwFlags);
			return true;
		}
        break;
	case 6:
        if(szName) _tcscpy(szName, _T("SetTexture"));
        if(m_pSrcDevice->GetCaps()->dwTextureOpCaps & D3DTEXOPCAPS_SELECTARG1)
		{
			SetTexture( 0, m_pTextures[0], dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE, dwFlags);
			return true;
		}
        break;
	case 7:
		if(szName) _tcscpy(szName, _T("COLOROP = MODULATE"));
		if(m_pSrcDevice->GetCaps()->dwTextureOpCaps & D3DTEXOPCAPS_MODULATE)
		{
			SetTexture( 0, m_pTextures[0], dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE, dwFlags);
			return true;
		}
		break;
   	case 8:
		if(szName) _tcscpy(szName, _T("COLOROP = MODULATE, Light"));
		if(m_pSrcDevice->GetCaps()->dwTextureOpCaps & D3DTEXOPCAPS_MODULATE)
		{
			SetLight(0, m_pLight[0], dwFlags);
			LightEnable(0, (DWORD) TRUE, dwFlags);
			SetTexture( 0, m_pTextures[0], dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE, dwFlags);
			return true;
		}
		break;
	case 9:
        if(szName) _tcscpy(szName, _T("MAGFILTER,MINFILTER = TEXF_LINEAR"));
        if((m_pSrcDevice->GetCaps()->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR) &&
           (m_pSrcDevice->GetCaps()->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))
        {
			SetTexture( 0, m_pTextures[0], dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_MAGFILTER, (DWORD)TEXF_LINEAR, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_MINFILTER, (DWORD)TEXF_LINEAR, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE, dwFlags);
			return true;
		}
        break;
	case 10:
        if(szName) _tcscpy(szName, _T("ALPHABLENDENABLE=TRUE"));
		if( (m_pSrcDevice->GetCaps()->dwTextureOpCaps & D3DTEXOPCAPS_MODULATE) && 
            (m_pSrcDevice->GetCaps()->dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA) &&
            (m_pSrcDevice->GetCaps()->dpcTriCaps.dwSrcBlendCaps & D3DPBLENDCAPS_SRCALPHA))
		{
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD) TRUE, dwFlags);
			SetRenderState(D3DRS_SRCBLEND,  (DWORD)D3DBLEND_SRCALPHA, dwFlags);
			SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA, dwFlags);
			SetTexture( 0, m_pTextures[0], dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_MODULATE, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_ALPHAARG2, (DWORD)D3DTA_DIFFUSE, dwFlags);
			return true;
		}
        break;
	case 11:
		{
            DWORD dwColor = 0x0000FF;
            float fStart = -1.0f, fEnd = 1.0f;
			if(szName) _tcscpy(szName, _T("FOGENABLE=true"));
			if( (m_pSrcDevice->GetCaps()->dwTextureOpCaps & D3DTEXOPCAPS_MODULATE) && 
				(m_pSrcDevice->GetCaps()->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGVERTEX) &&
				(m_pSrcDevice->GetCaps()->dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_COLORGOURAUDRGB))
			{
				SetRenderState(D3DRS_SHADEMODE, (DWORD)D3DSHADE_GOURAUD, dwFlags);
				SetRenderState(D3DRS_FOGENABLE,     (DWORD) TRUE, dwFlags);
				SetRenderState(D3DRS_FOGVERTEXMODE, (DWORD) D3DFOG_LINEAR, dwFlags);
				SetRenderState(D3DRS_FOGCOLOR, dwColor, dwFlags);
				SetRenderState(D3DRS_FOGSTART, *(DWORD*)&fStart, dwFlags);
				SetRenderState(D3DRS_FOGEND,   *(DWORD*)&fEnd, dwFlags);
				return true;
			}
			break;        
		}
    case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
    case 18:
		{
			int nTexIndex = 1;
			int nTexStages =  m_pSrcDevice->GetCaps()->wMaxSimultaneousTextures;
			int nRequested =  nIndex-10;
			if(szName) _stprintf(szName, _T("%d Stage multi-texturing"), nRequested);
			if(nRequested <= nTexStages)
			{
				for(int j=0; j< nRequested; j++)
				{
					if(j == 0)
					{
						SetTexture( 0, m_pTextures[nTexIndex], dwFlags);
						SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1, dwFlags);
						SetTextureStageState((DWORD)0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE, dwFlags);
					}
					else
					{
						SetTexture( j, m_pTextures[nTexIndex+j], dwFlags);
						SetTextureStageState((DWORD)j, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE, dwFlags);
						SetTextureStageState((DWORD)j, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE, dwFlags);
						SetTextureStageState((DWORD)j, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT, dwFlags);
						SetTextureStageState((DWORD)j, D3DTSS_TEXCOORDINDEX, (DWORD)0, dwFlags);
					}
				}
				return true;
			}
		}
		break;
	case 19:
		if(szName) _tcscpy(szName, _T("Spotlights"));
		if(m_dwPosLightsSupported && 
		   (m_pSrcDevice->GetCaps()->dwTextureOpCaps & D3DTEXOPCAPS_MODULATE))
		{
			SetLight(0, m_pLight[1], dwFlags);
			SetLight(1, m_pLight[2], dwFlags);
			SetLight(2, m_pLight[3], dwFlags);
			LightEnable(0, (DWORD) TRUE, dwFlags);
			LightEnable(1, (DWORD) TRUE, dwFlags);
			LightEnable(2, (DWORD) TRUE, dwFlags);
			SetTexture( 0, m_pTextures[0], dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE, dwFlags);
			return true;
		}
        break;
	case 20:
		if(szName) _tcscpy(szName, _T("Specular lighting"));
		if((m_pSrcDevice->GetCaps()->dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_SPECULARGOURAUDRGB) &&
           m_dwPosLightsSupported)
		{
			SetLight(0, m_pLight[1], dwFlags);
			SetLight(1, m_pLight[2], dwFlags);
			SetLight(2, m_pLight[3], dwFlags);
			LightEnable(0, (DWORD) TRUE, dwFlags);
			LightEnable(1, (DWORD) TRUE, dwFlags);
			LightEnable(2, (DWORD) TRUE, dwFlags);
			SetRenderState(D3DRS_SPECULARENABLE, (DWORD) TRUE, dwFlags);
			return true;
		}
		break;
	case 21:
        if(szName) _tcscpy(szName, _T("Viewport"));  		
        SetViewport(&m_pViewports[1], dwFlags);
        return true;			
	case 22:
		{
            D3DVALUE p1[4];
			if(szName) _tcscpy(szName, _T("Local UD clip-plane"));
			if(m_pSrcDevice->GetCaps()->wMaxUserClipPlanes > 0)
			{
				memset(p1, 0, sizeof(D3DVALUE) * 4);
				p1[0] = -0.7071067f; 
				p1[2] = -0.7071067f;
				p1[3] =  0.0f;
				SetClipPlane(0, (D3DVALUE*)p1, dwFlags);
				SetRenderState(D3DRS_CLIPPLANEENABLE, (DWORD)D3DCLIPPLANE0, dwFlags);
				return true;
			}
		}
		break;
	case 23:
        if(szName) _tcscpy(szName, _T("Local lights"));  		
		{
			LIGHT LightData;
			ZeroMemory(&LightData,sizeof(LIGHT));
			LightData.dltType             = m_dwPosLightsSupported ? D3DLIGHT_POINT : D3DLIGHT_DIRECTIONAL;
			LightData.dcvDiffuse.r        = 1.0f;
			LightData.dcvDiffuse.g        = 1.0f;
			LightData.dcvDiffuse.b        = 1.0f;
			LightData.dcvAmbient.r        = 1.0f;
			LightData.dcvAmbient.g        = 1.0f;
			LightData.dcvAmbient.b        = 1.0f;
			LightData.dcvSpecular.r       = 1.0f;
			LightData.dcvSpecular.g       = 1.0f;
			LightData.dcvSpecular.b       = 1.0f;
			LightData.dvPosition       = D3DXVECTOR3(1.0f, 0.0f, -0.5f);
			LightData.dvDirection      = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
			LightData.dvAttenuation0   = 0.98f;
			LightData.dvAttenuation1   = 0.015f;
			LightData.dvAttenuation2   = 0.005f;
			LightData.dvRange          = 15.0f;
			SetLight(0, &LightData, dwFlags);
			return true;
		}
		break;
	case 24:
        if(szName) _tcscpy(szName, _T("Local viewport"));
		{
			VIEWPORT vp;
            memset(&vp,0, sizeof(VIEWPORT));
			vp.Width = m_pCurrentMode->nBackBufferWidth/2;
			vp.Height = m_pCurrentMode->nBackBufferHeight/2;
			vp.X = 0;
			vp.Y = 0;
			vp.MinZ = 0;
			vp.MaxZ = 1.f;
            SetViewport(&vp, dwFlags);
			return true;
		}
		break;
  	case 25:
        if(szName) _tcscpy(szName, _T("Local TRANSFORM"));
		{
            D3DXMATRIX mat;
            D3DXMatrixRotationY(&mat, pi / 3.f);
			SetTransform(D3DTS_WORLD, &mat, dwFlags);
			return true;
		}
		break;
	case 26:
        if(szName) _tcscpy(szName, _T("record != set state"));
		if(m_pSrcDevice->GetCaps()->dwTextureOpCaps & D3DTEXOPCAPS_SELECTARG1)
		{
			SetTexture( 0, m_pTextures[0], dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1, dwFlags);
			SetTextureStageState((DWORD)0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE, dwFlags);
			return true;
		}
		break;
	case 27:
        if(szName) _tcscpy(szName, _T("LightEnable with uninitialized light"));
		if(!m_bCmp)
		{
			LightEnable(NLIGHTS-1, TRUE, dwFlags);
			return true;
		}
		break;
	case 28:
		if(szName) _tcscpy(szName, _T("SetVertexShaderConstantF"));
        if(D3DSHADER_VERSION_MAJOR(m_pSrcDevice->GetCaps()->dwVertexShaderVersion) >= 1)
		{
			D3DXVECTOR4 v(0.f, 1.f, 0.f, 1.f);
            SetVertexShaderConstant(4, &v, 1, dwFlags);
			return true;
		}
		break;
	case 29:
		if(szName) _tcscpy(szName, _T("SetVertexShader"));
        if(D3DSHADER_VERSION_MAJOR(m_pSrcDevice->GetCaps()->dwVertexShaderVersion) >= 1)
		{
			SetVertexShader(m_dwVSShader, dwFlags);
			return true;
		}
		break;
	case 30:
		if(szName) _tcscpy(szName, _T("SetStreamSource"));
        if(D3DSHADER_VERSION_MAJOR(m_pSrcDevice->GetCaps()->dwVertexShaderVersion) >= 1)
		{
			SetStreamSource(0, m_pVB, GetVertexSize(m_dwFVF), 0, dwFlags);
			return true;
		}
		break;
	case 31:
		if(szName) _tcscpy(szName, _T("SetIndices"));
        if(D3DSHADER_VERSION_MAJOR(m_pSrcDevice->GetCaps()->dwVertexShaderVersion) >= 1)
		{
			SetIndices(m_pIB, 0, dwFlags);
			return true;
		}
		break;
	case 32:
        if(szName) _tcscpy(szName, _T("SetPixelShaderConstantF"));
		if(D3DSHADER_VERSION_MAJOR(m_pSrcDevice->GetCaps()->dwPixelShaderVersion) >= 1)
		{
			D3DXVECTOR4 v(1.f, 1.f, 1.f, 1.f);
			SetPixelShaderConstant(0, &v, 1, dwFlags);
			return true;
		}
		break;
	case 33:
        if(szName) _tcscpy(szName, _T("SetPixelShader"));
		if(D3DSHADER_VERSION_MAJOR(m_pSrcDevice->GetCaps()->dwPixelShaderVersion) >= 1)
		{
			SetPixelShader(m_dwPSShader, dwFlags);
			return true;
		}
		break;
	case 34:
        if(szName) _tcscpy(szName, _T("SetPixelShaderConstantI"));
		if(D3DSHADER_VERSION_MAJOR(m_pSrcDevice->GetCaps()->dwPixelShaderVersion) >= 3)
		{
			int	nConst[] = { 4, 3, 2, 1 };
			SetPixelShaderConstantI(0, nConst, 1, dwFlags);
			return true;
		}
		break;
	case 35:
        if(szName) _tcscpy(szName, _T("SetPixelShaderConstantB"));
		if(D3DSHADER_VERSION_MAJOR(m_pSrcDevice->GetCaps()->dwPixelShaderVersion) >= 3)
		{
			BOOL bConst[] = { TRUE, FALSE, TRUE, FALSE };
			SetPixelShaderConstantB(0, bConst, 1, dwFlags);
			return true;
		}
		break;
	case 36:
		if(szName) _tcscpy(szName, _T("SetVertexShaderConstantI"));
        if(D3DSHADER_VERSION_MAJOR(m_pSrcDevice->GetCaps()->dwVertexShaderVersion) >= 2)
		{
			int	nConst[] = { 4, 3, 2, 1 };
            SetVertexShaderConstantI(0, nConst, 1, dwFlags);
			return true;
		}
		break;
	case 37:
		if(szName) _tcscpy(szName, _T("SetVertexShaderConstantB"));
        if(D3DSHADER_VERSION_MAJOR(m_pSrcDevice->GetCaps()->dwVertexShaderVersion) >= 2)
		{
			BOOL bConst[] = { TRUE, FALSE, TRUE, FALSE };
            SetVertexShaderConstantB(0, bConst, 1, dwFlags);
			return true;
		}
		break;
	case 38:
        if(szName) _tcscpy(szName, _T("SetScissorRect"));
		if(m_pD3D->GetVersion() >= 9.f && Caps.RasterCaps & D3DPRASTERCAPS_SCISSORTEST)
		{
			RECT r = { 0, 0, m_pCurrentMode->nBackBufferWidth/2, m_pCurrentMode->nBackBufferHeight/2 };
			SetScissorRect(&r);
			SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
			return true;
		}
		break;
	case 39:
		if(szName) _tcscpy(szName, _T("Verify REF count on Indices"));
        if(m_pD3D->GetVersion() >= 9.f && D3DSHADER_VERSION_MAJOR(m_pSrcDevice->GetCaps()->dwVertexShaderVersion) >= 1)
		{
			CnIndexBuffer* pIB = NULL;
			CnVertexBuffer* pVB = NULL;

			if(!SetupBuffers(&m_FrontPlane, &pVB, &pIB))
				return false;
			// Set the temporary buffers
			SetStreamSource(0, pVB, GetVertexSize(m_dwFVF), 0, dwFlags);
			SetIndices(pIB, 0, dwFlags);

			// Release the buffers
			RELEASE(pIB);
			RELEASE(pVB);
			return true;
		}
		break;
	default:
		break;
	}
    return false;    
}

// ----------------------------------------------------------------------------

bool CStateBeast::Setup()
{
	if(m_nTotalBlocks)
	{
		// Allocate the stateblock data
		m_pBlocks = new BLOCKINFO[m_nTotalBlocks];
		if(m_pBlocks == NULL)
		{
			WriteToLog(_T("Setup() - Out of memory."));
			return false;
		}
		memset(m_pBlocks, 0, m_nTotalBlocks * sizeof(BLOCKINFO));
	}

	if(!CD3DTest::Setup())
		return false;

    if(m_dwVersion >= 0x800)
    {
		if(!CreateShaders() || !SetVertexShader(m_dwFVF))
			return false;
	}

    // Create a stateblock to hold all state
    if(!CreateStateBlock(D3DSBT_ALL, &m_OriginalBlock.dwSrcHandle) ||
	   // Capture the current state
	   !CaptureStateBlock(m_OriginalBlock.dwSrcHandle))
		return false;

	return true;
}

// ----------------------------------------------------------------------------

void CStateBeast::Cleanup()
{
    ReleaseTest();
}

// ----------------------------------------------------------------------------

void CStateBeast::ReleaseTest()
{
	if(m_pBlocks)
	{
		for(int i=0; i<m_nTotalBlocks; i++)
		{
			if(m_pBlocks[i].dwSrcHandle)
				DeleteStateBlock(m_pBlocks[i].dwSrcHandle);
		}
       	SAFEDELETEA(m_pBlocks);
	}

	if(m_OriginalBlock.dwSrcHandle)
	{
		DeleteStateBlock(m_OriginalBlock.dwSrcHandle);
		memset(&m_OriginalBlock, 0, sizeof(BLOCKINFO));
	}

	SAFEDELETE(m_pFrontMat);
	SAFEDELETE(m_pFrontMat2);
	SAFEDELETE(m_pBackMat);
	SAFEDELETE(m_pViewports);

	// Delete the app tools
	int i;
	for(i=0; i<NLIGHTS; i++)
    {
        if(m_pLight[i])
        {
            SAFEDELETE(m_pLight[i]);
        }
    }


	if(m_dwVSShader)
	{
		DeleteVertexShader(m_dwVSShader);
		m_dwVSShader = 0;
	}

	if(m_dwVSShader2)
	{
		DeleteVertexShader(m_dwVSShader2);
		m_dwVSShader2 = 0;
	}
	if(m_dwVSShader3)
	{
		DeleteVertexShader(m_dwVSShader3);
		m_dwVSShader3 = 0;
	}

	if(m_dwPSShader)
	{
		DeletePixelShader(m_dwPSShader);
		m_dwPSShader = 0;
	}
	if(m_dwPSShader3)
	{
		DeletePixelShader(m_dwPSShader3);
		m_dwPSShader3 = 0;
	}

	RELEASE(m_pVB);
    RELEASE(m_pIB);
	
	for(i=0; i<8; i++)
		SetTexture(0, NULL);

    for(i=0; i<NTEXTURES; i++)
    {
        if(m_pTextures[i])
        {
            RELEASE(m_pTextures[i]);
            m_pTextures[i] = NULL;
        }
    }

}

// ----------------------------------------------------------------------------

bool CStateBeast::CreateShaders()
{
	HRESULT hr;

    // Create the vertex shader
	if(D3DSHADER_VERSION_MAJOR(m_pSrcDevice->GetCaps()->dwVertexShaderVersion) >= 1)
	{
        DWORD decl[] =
			{	D3DVSD_STREAM(0),
				D3DVSD_REG(0, D3DVSDT_FLOAT3),
				D3DVSD_REG(1, D3DVSDT_FLOAT3),
				D3DVSD_REG(2, D3DVSDT_FLOAT2),
				D3DVSD_END()
			};
		
		char *szShader = "vs.1.1\n"
						 "dp4 oPos.x, v0, c0\n"
						 "dp4 oPos.y, v0, c1\n"
						 "dp4 oPos.z, v0, c2\n"
						 "dp4 oPos.w, v0, c3\n"
						 "mov oD0, c4\n";
        LPD3DXBUFFER pShader;
		hr = D3DXAssembleShader(szShader, strlen(szShader), 0, NULL, &pShader, NULL);
		if (FAILED(hr))
		{
			WriteToLog(_T("CStateBeast::CreateShaders -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
						m_pD3D->HResultToString(hr));
			return false;
		}		
		CreateVertexShader(decl, (DWORD*)pShader->GetBufferPointer(), &m_dwVSShader, NULL);

        if(pShader)
			pShader->Release();
	}

	if(D3DSHADER_VERSION_MAJOR(m_pSrcDevice->GetCaps()->dwPixelShaderVersion) >= 1)
	{
		// Create the pixel shader
		char *szShader = "ps.1.1\n"
						 "add r0, v0, c0\n";
		LPD3DXBUFFER pShader;
		hr = D3DXAssembleShader(szShader, strlen(szShader), 0, NULL, &pShader, NULL);
		if (FAILED(hr))
		{
			WriteToLog(_T("CStateBeast::CreateShaders -- D3DXAssembleShader(pixel) failed, hr=%s\n"),
						m_pD3D->HResultToString(hr));
			return false;
		}
    		
		
		if(!CreatePixelShader((DWORD*)pShader->GetBufferPointer(), &m_dwPSShader))
		{
			WriteToLog(_T("CStateBeast::CreateShaders -- CreatePixelShader failed\n"));
			return false;
		}
            
		if(pShader)
			pShader->Release();
	}

	if(D3DSHADER_VERSION_MAJOR(m_pSrcDevice->GetCaps()->dwVertexShaderVersion) >= 2)
	{
        DWORD decl[] =
			{	D3DVSD_STREAM(0),
				D3DVSD_REG(0, D3DVSDT_FLOAT3),
				D3DVSD_REG(1, D3DVSDT_FLOAT3),
				D3DVSD_REG(2, D3DVSDT_FLOAT2),
				D3DVSD_END()
			};
		
		char *szShader = "vs.2.0\n"
						 "def c12, 1,1,1,1 \n"
						 "def c13, 0,1,0,1 \n"
						 "def c14, 1,0,1,0 \n"
						 "def c15, .1,0,0,0 \n"
						 "dp4 oPos.x, v0, c0\n"
						 "dp4 oPos.y, v0, c1\n"
						 "dp4 oPos.z, v0, c2\n"
						 "dp4 oPos.w, v0, c3\n"
						 "mov r0, c12\n"
						 "rep i0\n"
							"add r0, r0, -c15.x\n"
						 "endrep\n"
						 "if b0\n"
							"mul r0, r0, c13\n"
						 "else\n"
							"mul r0, r0, c14\n"
						 "endif\n"
						 "mov oD0, r0\n";
        LPD3DXBUFFER pShader;
		hr = D3DXAssembleShader(szShader, strlen(szShader), 0, NULL, &pShader, NULL);
		if (FAILED(hr))
		{
			WriteToLog(_T("CStateBeast::CreateShaders -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
						m_pD3D->HResultToString(hr));
			return false;
		}		
		CreateVertexShader(decl, (DWORD*)pShader->GetBufferPointer(), &m_dwVSShader2, NULL);

        if(pShader)
			pShader->Release();
	}

	if(D3DSHADER_VERSION_MAJOR(m_pSrcDevice->GetCaps()->dwVertexShaderVersion) >= 3)
	{
        DWORD decl[] =
			{	D3DVSD_STREAM(0),
				D3DVSD_REG(0, D3DVSDT_FLOAT3),
				D3DVSD_REG(1, D3DVSDT_FLOAT3),
				D3DVSD_REG(2, D3DVSDT_FLOAT2),
				D3DVSD_END()
			};
		
		char *szShader = "vs_3_0\n"
						 "def c4, 1,1,1,1\n"
						 "dcl_position0	v0\n"
						 "dcl_position0	o0\n"
						 "dcl_color0	o1\n"
						 "dp4 o0.x, v0, c0\n"
						 "dp4 o0.y, v0, c1\n"
						 "dp4 o0.z, v0, c2\n"
						 "dp4 o0.w, v0, c3\n"
						 "mov o1, c4\n";
        LPD3DXBUFFER pShader;
		hr = D3DXAssembleShader(szShader, strlen(szShader), 0, NULL, &pShader, NULL);
		if (FAILED(hr))
		{
			WriteToLog(_T("CStateBeast::CreateShaders -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
						m_pD3D->HResultToString(hr));
			return false;
		}		
		CreateVertexShader(decl, (DWORD*)pShader->GetBufferPointer(), &m_dwVSShader3, NULL);

        if(pShader)
			pShader->Release();
	}

	if(D3DSHADER_VERSION_MAJOR(m_pSrcDevice->GetCaps()->dwPixelShaderVersion) >= 3)
	{
		// Create the pixel shader
		char *szShader = "ps_3_0\n"
						 "dcl_color0 v0\n"
						 "def c12, 1,1,1,1 \n"
						 "def c13, 0,1,0,1 \n"
						 "def c14, 1,0,1,0 \n"
						 "def c15, .1,0,0,0 \n"
						 "mov r0, v0\n"
						 "rep i0\n"
							"add r0, r0, -c15.x\n"
						 "endrep\n"
						 "if b0\n"
							"mul r0, r0, c13\n"
						 "else\n"
							"mul r0, r0, c14\n"
						 "endif\n"
						 "mov oC0, r0\n";;
		LPD3DXBUFFER pShader;
		hr = D3DXAssembleShader(szShader, strlen(szShader), 0, NULL, &pShader, NULL);
		if (FAILED(hr))
		{
			WriteToLog(_T("CStateBeast::CreateShaders -- D3DXAssembleShader(pixel) failed, hr=%s\n"),
						m_pD3D->HResultToString(hr));
			return false;
		}
    		
		
		if(!CreatePixelShader((DWORD*)pShader->GetBufferPointer(), &m_dwPSShader3))
		{
			WriteToLog(_T("CStateBeast::CreateShaders -- CreatePixelShader failed\n"));
			return false;
		}
            
		if(pShader)
			pShader->Release();
	}

    return true;
}

// ----------------------------------------------------------------------------

bool CStateBeast::SetupBuffers(CShapes *pShapes, CnVertexBuffer **ppVB, CnIndexBuffer **ppIB)
{
    BYTE* pBuffer = NULL;
	HRESULT hr = S_OK;
    
	// Setup the vertex buffer
	if(!CreateVertexBuffer(GetVertexSize(m_dwFVF) * pShapes->m_nVertices, USAGE_DYNAMIC, m_dwFVF, D3DPOOL_SYSTEMMEM, ppVB))
	{
		WriteToLog(_T("SetupBuffers() - CreateVertexBuffer failed. hr=%s\n"), m_pD3D->HResultToString(GetLastError()));
		return false;
	}

	if(!(*ppVB)->Lock(0, 0, &pBuffer, LOCK_NOSYSLOCK))
	{
		WriteToLog(_T("SetupBuffers() - couldn't lock VB. hr=%s\n"), m_pD3D->HResultToString(GetLastError()));
		(*ppVB)->Release();
		return false;
	}
    
	PBYTE pPos = (PBYTE)pBuffer;
	int nSize = GetVertexSize(m_dwFVF);
    for(int i=0; i<pShapes->m_nVertices; i++)
	{
		memcpy(pPos, &(pShapes->m_pVertices[i]), nSize);
		pPos += nSize;
	}
	
    (*ppVB)->Unlock();
    

	// Setup the index buffer
    if(!CreateIndexBuffer(sizeof(WORD) * pShapes->m_nIndices, USAGE_DYNAMIC , FMT_INDEX16, D3DPOOL_SYSTEMMEM, ppIB) || *ppIB == NULL)
	{
		WriteToLog(_T("SetupBuffers() - CreateIndexBuffer failed. hr=%s\n"), m_pD3D->HResultToString(GetLastError()));
		(*ppVB)->Release();
		return false;
	}

	if(!(*ppIB)->Lock(0, 0, &pBuffer, LOCK_NOSYSLOCK))
	{
		WriteToLog(_T("SetupBuffers() - couldn't lock IB. hr=%s\n"), m_pD3D->HResultToString(GetLastError()));
		(*ppVB)->Release();
        (*ppIB)->Release();
		return false;
	}
	
    memcpy(pBuffer, pShapes->m_pIndices, pShapes->m_nIndices*sizeof(WORD));
    (*ppIB)->Unlock();

	return true;
}
