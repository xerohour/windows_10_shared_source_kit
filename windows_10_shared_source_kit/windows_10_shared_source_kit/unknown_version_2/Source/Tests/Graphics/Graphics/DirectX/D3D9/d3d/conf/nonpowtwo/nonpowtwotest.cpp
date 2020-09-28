#include "NonPowTwoTest.h"
#include "types.h"
#include "D3DX9.h"

//extern variables from testcasestruct.h
STextureInfo   g_TexInfo  [TEXTURE_COUNT];  
CnBaseTexture *g_pTextures[TEXTURE_COUNT];
UINT           g_MaxStageCount;
bool           g_bUseTL;
bool           g_isVShaderCase;
bool           g_isWrapCase;
bool           g_isZeroDiffuse;
bool           g_isHalfDiffuse;

//------------------------------------------------------------------------------

CNonPowTwoTest::CNonPowTwoTest()
{
	ZeroMemory(g_pTextures, sizeof(CnBaseTexture *) * TEXTURE_COUNT);
	ZeroMemory(g_TexInfo,   sizeof(STextureInfo   ) * TEXTURE_COUNT);
	ZeroMemory(m_bUsedFmt,  sizeof(bool          *) * TEXTURE_COUNT);
	m_nFmt = 0;

	m_uRectHeight = 0;
	m_uRectWidth  = 0;

	for (UINT i = 0; i < TEXTURE_COUNT; i++)
	{
		g_TexInfo[i].uMipCount = i % 2;
		g_TexInfo[i].TexType = (TEXTURE_TYPE)(i/2);
	}
}

//------------------------------------------------------------------------------

CNonPowTwoTest::~CNonPowTwoTest()
{
	for(UINT i = 0; i < TEXTURE_COUNT; i++)
        delete [] m_bUsedFmt[i];
}

//------------------------------------------------------------------------------

//need to overload this function to be able to set a random seed for each test mode and choose texture formats
//before calling ConstructTestCaption
bool CNonPowTwoTest::SetupTest()
{
	int start = m_uCurrentMode-1;
	int end   = start + 1;

	//initialize m_bUsedFmt at first call
	if (NULL == m_bUsedFmt[0])
	{
		m_nFmt = m_uCommonTextureFormats;

		if ( m_uCommonTextureFormats == 0 )
			return false;
		
		for (UINT i = 0; i < TEXTURE_COUNT; i++)
		{
			m_bUsedFmt[i] = new bool[m_nFmt];
			//ZeroMemory(m_bUsedFmt[i], sizeof(bool) * m_nFmt);
		}
	}

	//if we run the test in a specific mode then we need to repeat setting random seed 
	//and choosing fmts based on this seed for all previous modes
	//thus we'll keep the behavir consistant for running all modes consiquently or separately
	//we also need to remove all other formats
	if (ReadInteger("mode", &end))
	{
		start = 0;
		bool result = true;

		if (end <= 0 || end > m_uTotalModes)
		{
			WriteToLog("Illigal mode index is used.\n");
			result = false;
		}

		//remove all modes but current one
		for (int i = 1; i < min(end, m_uTotalModes); i++)
		{
			if ( NULL == m_pModeList )
				return false;
			TESTMODE *pTemp = m_pModeList;
			m_pModeList = m_pModeList->pNext;
			SAFEDELETE(pTemp->pDisplay);
			SAFEDELETE(pTemp);
			m_uTotalModes--;	    
		}
		m_pCurrentMode = m_pModeList;
		
    	TESTMODE *pMode = m_pModeList;
		pMode = pMode->pNext;
		while (pMode)
		{
			TESTMODE *pTemp = pMode->pNext;
			SAFEDELETE(pMode->pDisplay);
			SAFEDELETE(pMode);
			pMode = pTemp;
			m_uTotalModes--;	    
		}
		m_pModeList->pNext = NULL;

		if (!result)
			return false; //this happens if specified mode is either <= 0 or > m_uTotalModes
	}
	else
		end = start + 1;

	//need to set it to have consistent level count for textures
	m_pCurrentMode->nTextureFormat = (end - 1) % m_uCommonTextureFormats;

	for (int mode = start; mode < end; mode++)
	{
        //initialize array to falses at the first mode
		//we also need to reinitialize array if we already went through all formats(this will only work for stress test)
		if (0 == mode % m_uCommonTextureFormats)
		{
			for (UINT i = 0; i < TEXTURE_COUNT; i++)
			{
				ZeroMemory(m_bUsedFmt[i], sizeof(bool) * m_nFmt);
			}
		}

		for (UINT i = 0; i < TEXTURE_COUNT; i++)
		{
			g_TexInfo[i].uMipCount = (i + mode % m_uCommonTextureFormats) % 2;
		}

		//randomly choose current formats (can't be repeated)
		//need to do it here to be able to use them in the title of the test group
		srand(mode);
		
		for(UINT i = 0; i < TEXTURE_COUNT; i++)
		{
			int FmtIndex;

			FmtIndex = rand() % m_nFmt;
			
			UINT j = ((i>>1)<<1) + g_TexInfo[i].uMipCount;
			
			while (true)
			{
				if ( !m_bUsedFmt[j][FmtIndex] ) 
				{
					m_bUsedFmt[j][FmtIndex] = true;
					//set fmts for current mode
					if (mode == end - 1)
					{
						switch (i)
						{
							case 0: case 1:  
								if ( m_uCommonTextureFormats != 0 )
								g_TexInfo[i].Format = m_pCommonTextureFormats[FmtIndex%m_uCommonTextureFormats];  
								break;
							case 2: case 3:  
								if ( m_uCommonCubeFormats != 0 )
								g_TexInfo[i].Format = m_pCommonCubeFormats   [FmtIndex%m_uCommonCubeFormats   ];
								break;
							case 4: case 5: 
								if ( m_uCommonVolumeFormats != 0 )
								g_TexInfo[i].Format = m_pCommonVolumeFormats [FmtIndex%m_uCommonVolumeFormats ];
								break;
						}	
					}
					break;
				}

				FmtIndex = (FmtIndex+1)%m_nFmt;
			}
		}
	}

	//need to copy used formats to group title name
	m_szGroupName[0] = '\0';
	CreateGroupName();


	//if ps1.x shaders are supported then we need to set a reg key dvMaxPixelShaderValue to the value of thsicap on HAL
	HKEY hRegKey;
	TCHAR ShaderRange[50];
	if ( m_pSrcDevice->GetCaps()->dwPixelShaderVersion > D3DPS_VERSION(0, 0) )
	{
	    //Now we need to set the ref MaxPixelShaderValue as this defines the range of
		//values that can be used within a pixel shader.
		//This is an un documented reg key
		_stprintf(ShaderRange, "%f", m_pSrcDevice->GetCaps()->dvMaxPixelShaderValue);
				
		if (SUCCEEDED ( RegCreateKeyEx(HKEY_LOCAL_MACHINE,
										_T("SOFTWARE\\Microsoft\\Direct3D\\ReferenceDevice"),
										0,
										_T(""),
										REG_OPTION_NON_VOLATILE,
										KEY_WRITE,
										NULL,
										&hRegKey,
										NULL) ) )
		{
			if ( FAILED ( RegSetValueEx(hRegKey, _T("PixelShader1xMaxValue"), 0, REG_SZ,  
							(CONST BYTE*) ShaderRange, 
							(_tcsclen(ShaderRange)+1)*sizeof(TCHAR) ) ) )
			{
				WriteToLog(_T("SetupTest: Unable to set Dx9 ref rast reg key ***Abort this test run***\n"));
				RegCloseKey(hRegKey);
				//return false;
			}

			RegCloseKey(hRegKey);
		}
		else
		{
			WriteToLog(_T("SetupTest: Unable to set ref rast reg key ***Abort this test run***\n"));
			//return false;
		}
	}

	return CD3DTest::SetupTest();
}

//------------------------------------------------------------------------------

void CNonPowTwoTest::CreateGroupName()
{
	DWORD dwTexCaps = m_pSrcDevice->GetCaps()->dwTextureCaps;
	UINT len = 0, prevLen = 0;
	//instead of displaying one format for each texture type we want to display two
	TCHAR szCaption[MAX_D3D_STRING], tempStr[MAX_D3D_STRING];
	TCHAR strFmt[3][10] = {"Texture: ", "Cube:", "Volume: "};
    TCHAR newStrFmt[3][50];
	TCHAR szFmts[TEXTURE_COUNT][30];

	ConstructTestCaption(szCaption, ARRAYSIZE(szCaption));
	
	LPTSTR newStr = NULL;

	for(int i = 0; i < TEXTURE_COUNT; i++)
	{
		UINT k = (g_TexInfo[i].uMipCount == 1 ? 0 : 1);
		UINT j = ((i>>1)<<1) + k;
        FmtToString(FORMAT_TEXTURE, &g_TexInfo[i].Format, szFmts[j]);
	}

	//want to insert mode index after the name of the test
	_stprintf(tempStr, _T("%s (Mode %d)"), m_szTestName, m_uCurrentMode);
	prevLen = strlen(tempStr);
	len = min(MAX_D3D_STRING-1-prevLen, strlen(szCaption)-strlen(m_szTestName));
	strncat(tempStr, szCaption + strlen(m_szTestName), len);
	tempStr[len+prevLen] = '\0';
	strcpy(szCaption, tempStr);

	//if we don't use some texture type then don't display their formats
	if ( KeySet("no2d") || NULL == m_pCommonTextureFormats )
		newStrFmt[0][0] = '\0';
	else
		sprintf(newStrFmt[0], "Tex: %s %s", szFmts[0], szFmts[1]);
	
	if ( KeySet("nocube") || !(dwTexCaps & D3DPTEXTURECAPS_CUBEMAP) )
		newStrFmt[1][0] = '\0';
	else
		sprintf(newStrFmt[1], "Cube: %s %s", szFmts[2], szFmts[3]);
	
	if ( KeySet("novolume") || !(dwTexCaps & D3DPTEXTURECAPS_VOLUMEMAP) )
		newStrFmt[2][0] = '\0';
	else
		sprintf(newStrFmt[2], "Vol: %s %s", szFmts[4], szFmts[5]);

	for(int i = 0; i < 3; i++)
	{
		newStr = strstr(szCaption, strFmt[i]);

		if (NULL != newStr)
		{
			int newlen = strlen(newStrFmt[i]);
			
			int oldlen = 0;
			while (newStr[oldlen] != '\0' && newStr[oldlen] != ',')
				oldlen++;

			strcpy(tempStr, newStr);
			strncpy(newStr, newStrFmt[i], newlen);
			strcpy(newStr + newlen, tempStr + oldlen);
		}
		else if (newStrFmt[i][0] != '\0')
		{
			sprintf(szCaption, "%s, %s", szCaption, newStrFmt[i]);
		}
	}

	len = min(NAME_SIZE-1, strlen(szCaption));
	strncpy(m_szGroupName, szCaption, len);
	m_szGroupName[len] = '\0';

}
    
//------------------------------------------------------------------------------

//need to overwrite this function to display formats used
//in order to only create this string once we call the function CreateGroupName inside SetupTest()
//which will use CD3DTest::ConstructTestCaption to create group name. Afterthat we just reuse this name
void CNonPowTwoTest::ConstructTestCaption(LPTSTR szText, int nText)
{
	if (m_szGroupName[0] != '\0')
		strncpy(szText, m_szGroupName, nText);
	else
		CD3DTest::ConstructTestCaption(szText, nText);
}

//------------------------------------------------------------------------------

void CNonPowTwoTest::CommandLineHelp()
{
	WriteCommandLineHelp(_T("$g-Mode:#                $yRun test in specific mode"));

	WriteCommandLineHelp(_T("$g-POW2                  $yForce power of 2 dimensions"));
	
    WriteCommandLineHelp(_T("$g-NoVShader\t$yNo vertex shader testing"));
	WriteCommandLineHelp(_T("$g-NoFilter                $yNo filter testing"));
    WriteCommandLineHelp(_T("$g-NoLODBias\t$yNo LOD bias testing"));
    WriteCommandLineHelp(_T("$g-NoTexOp\t$yNo TextureOp testing"));
    WriteCommandLineHelp(_T("$g-NoAlphaBlend\t$yNo alpha blending testing"));
    WriteCommandLineHelp(_T("$g-NoAddress\t$yNo texture addressing mode testing"));
	WriteCommandLineHelp(_T("$g-NoWrap\t$yNo wraping testing"));
    
    WriteCommandLineHelp(_T("$g-NoVolume\t$yNo volume maps"));
    WriteCommandLineHelp(_T("$g-NoCube\t$yNo cube maps"));
    WriteCommandLineHelp(_T("$g-No2D                    $yNo image map testing"));
}

//------------------------------------------------------------------------------

bool CNonPowTwoTest::CapsCheck()
{
	m_bTexPow2       = false;
	m_bCubePow2      = false;
	m_bVolumePow2    = false;
	m_bNonPow2Cond   = false;

	const DEVICEDESC* pCaps = m_pSrcDevice->GetCaps();

	if(NULL == pCaps)
		return false;

	g_MaxStageCount = pCaps->wMaxTextureBlendStages;

	m_bNonPow2Cond = (D3DPTEXTURECAPS_NONPOW2CONDITIONAL == (pCaps->dwTextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL));
	
	m_bTexPow2 = (D3DPTEXTURECAPS_POW2 == (pCaps->dwTextureCaps & D3DPTEXTURECAPS_POW2));
	
	bool bCubeMap = (D3DPTEXTURECAPS_CUBEMAP == (pCaps->dwTextureCaps & D3DPTEXTURECAPS_CUBEMAP));
	bCubeMap &= !KeySet("nocube");
	m_bCubePow2 = (D3DPTEXTURECAPS_CUBEMAP_POW2 == (pCaps->dwTextureCaps & D3DPTEXTURECAPS_CUBEMAP_POW2));
	
	bool bVolumeMap = (D3DPTEXTURECAPS_VOLUMEMAP == (pCaps->dwTextureCaps & D3DPTEXTURECAPS_VOLUMEMAP));
	bVolumeMap &= !KeySet("novolume");
	m_bVolumePow2 = (D3DPTEXTURECAPS_VOLUMEMAP_POW2 == (pCaps->dwTextureCaps & D3DPTEXTURECAPS_VOLUMEMAP_POW2));

	bool bRunTest = false;

	//can use any sizes on cube maps or volumes if they are supported	
	if ((bCubeMap && !m_bCubePow2) || (bVolumeMap && !m_bVolumePow2) )
		 bRunTest = true;

	//can use any sizes on textures
	if (!m_bTexPow2 && !m_bNonPow2Cond)
		bRunTest = true;

	//can use any sizes for 1-level textures
	else if ( m_bTexPow2 &&  m_bNonPow2Cond)
		bRunTest = true;

	//return true for now but abort in Setup because it's a wrong combination of caps
	//if we just return false here then test will be just skipped instead of aborting
	else if (!m_bTexPow2 &&  m_bNonPow2Cond)
		bRunTest = true;

	//if command line -pow2 is set then we support all sizes so reset all the caps
	if (KeySet("pow2"))
	{
		m_bTexPow2 = true;
		m_bCubePow2 = true;
		m_bVolumePow2 = true;
		m_bNonPow2Cond = false;
		bRunTest = true;
	}

	if( !bRunTest )
	{
		WriteToLog("Necessary caps are not supported.\n");

		//remove all modes but current one
		TESTMODE *pMode = m_pModeList;
    	
		pMode = pMode->pNext;
		while (pMode)
		{
			TESTMODE *pTemp = pMode->pNext;
			SAFEDELETE(pMode->pDisplay);
			SAFEDELETE(pMode);
			pMode = pTemp;
			m_uTotalModes--;
		}

		m_pModeList->pNext = NULL;
        m_pFramework->SuggestExitCode(EXITCODE_SKIP);
	}

	return bRunTest;
}

//------------------------------------------------------------------------------

bool CNonPowTwoTest::TestTerminate()
{
	CTestMatrix* pTestMatrix = GetTestMatrix();
	
	if(NULL != pTestMatrix)
	{
		pTestMatrix->Cleanup();
	}

	Cleanup();

	//need to clean up a reg Key we set in SetupTest()
	HKEY hRegKey;
	if (SUCCEEDED (RegOpenKeyEx(HKEY_LOCAL_MACHINE,								// handle to open key
						_T("SOFTWARE\\Microsoft\\Direct3D\\ReferenceDevice"),	// subkey name
						0,														// reserved
						KEY_ALL_ACCESS,											// security access mask
						&hRegKey) )	)											// handle to open key
	{		
		if ( FAILED  ( RegDeleteValue(hRegKey,				// handle to open key
					_T("PixelShader1xMaxValue") ) ) )		// value name
		{
			WriteToLog(_T("TestTerminate: Unable to restore Dx9 ref rast reg key\n"));
		}
		
		RegCloseKey(hRegKey);
	}

	return true;
}

//------------------------------------------------------------------------------

void CNonPowTwoTest::Cleanup()
{
	CleanupTextures();
}

//------------------------------------------------------------------------------

void CNonPowTwoTest::UpdateStatus()
{
	CD3DTest::UpdateStatus();

	CTestMatrix* pTestMatrix = GetTestMatrix();
	
	if(NULL != pTestMatrix)
		pTestMatrix->UpdateStatus(this);
}

//------------------------------------------------------------------------------

bool CNonPowTwoTest::Setup()
{
    int nStage;
    DWORD dwMode;

	//only used for stress test
	if ( m_bSkipAll )
		return true;

    //if illigal combination of flags is set (!pow2 && nonpowtwo) then abort
	if (!m_bTexPow2 && m_bNonPow2Cond)
	{
		//remove all modes but current one
		TESTMODE *pMode = m_pModeList;
    	
		pMode = pMode->pNext;
		while (pMode)
		{
			TESTMODE *pTemp = pMode->pNext;
			SAFEDELETE(pMode->pDisplay);
			SAFEDELETE(pMode);
			pMode = pTemp;
			m_uTotalModes--;	    
		}

		m_pModeList->pNext = NULL;
	
		WriteToLog("Illigal combination of flags is set (!D3DPTEXTURECAPS_POW2 && D3DPTEXTURECAPS_NONPOW2CONDITIONAL)\n");
		return false;
	}

	if (KeySet("notl") || m_Options.TestOptions.Type == TESTTYPE_STRESS)
		g_bUseTL = false;
	else
		g_bUseTL = true;


   	if ( !SetupMatrices() )
	{
		return false;
	}

	if( !InitGeometry() )
	{
		return false;
	}

	if ( !InitializeTextures() )
	{
		return false;
	}

	if ( !SetDefaultStates() )
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------

UINT CNonPowTwoTest::TestInitialize()
{
	g_isVShaderCase = false;
	g_isWrapCase    = false;
	g_isZeroDiffuse = false;
	g_isHalfDiffuse = false;

	//only used for stress test
	if ( m_bSkipAll )
	{
		SetTestRange(1, 1);
		return D3DTESTINIT_SKIPALL;
	}

	CTestMatrix* pTestMatrix = GetTestMatrix();
	if(NULL == pTestMatrix)
	{
		return D3DTESTINIT_ABORT;
	}

	pTestMatrix->Cleanup();

	pTestMatrix->RegisterDeletionCallback(DeletionCallBack);

	if(!AddTestCases(pTestMatrix))
	{
		return D3DTESTINIT_ABORT;
	}

	if(!pTestMatrix->InitializeTestVariables(this))
	{
		WriteToLog(_T("TestInitialize: pTestMatrix->InitializeTestVariables() failed.\n"));
		return D3DTESTINIT_ABORT;
	}

	ClearRangeList();

	SetTestCaseRange(pTestMatrix);
	
	return D3DTESTINIT_RUN;
}

//------------------------------------------------------------------------------

bool CNonPowTwoTest::ExecuteTest(UINT nTestNum)
{
	bool bResult = true;

	CTestMatrix* pTestMatrix = GetTestMatrix();

	if(NULL == pTestMatrix)
	{
		Fail();
		return false;
	}

	TCHAR testName[NAME_SIZE];

	testName[0] = '\0';
	
	bResult &= SetupTestState(testName, pTestMatrix, nTestNum);

	//need to call BeginTestCase after SetupTestState because that's where we initialize test case's name
	BeginTestCase(testName, nTestNum);

	bResult &= BeginScene();

	bResult &= RenderScene();

	bResult &= EndScene();

	return bResult;
}

//------------------------------------------------------------------------------

bool CNonPowTwoTest::SetupMatrices()
{
	D3DXVECTOR3 vEyePos(0.0f, 0.0f, -1.0f);
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);

	float		fFov           = D3DX_PI/3.0f;
	float		fAspectRatio   = 4.0f/3.0f;
	float		fNearClipClane = 0.1f;
	float		fFarClipPlane  = 100.0f;

	D3DXMatrixTranslation(&m_mWorld[0], 0.2f, -0.05f, 0.0f);
	
	D3DXMatrixTranslation(&m_mWorld[1], -0.2f, 0.05f, 0.0f);
	
	D3DXMatrixLookAtLH(&m_mView, &vEyePos, &vLookAt, &vUp);

	D3DXMatrixPerspectiveFovLH(&m_mProj, fFov, fAspectRatio, fNearClipClane, fFarClipPlane);

	if(!SetTransform(D3DTS_VIEW, (D3DMATRIX*)&m_mView))
	{
		WriteToLog(_T("Couldn't set view transformation matrix.\n"));
		return false;
	}

	if(!SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&m_mProj))
	{
		WriteToLog(_T("Couldn't set projection transformation matrix.\n"));
		return false;
	}

	//set up matrices for vertex shaders
	D3DXMATRIX temp;
	for(UINT i = 0; i < 2; i++)
	{
		D3DXMatrixMultiply( &temp, &m_mWorld[i], &m_mView); 
		D3DXMatrixMultiply( &m_mTransform[i], &temp, &m_mProj); 
	}

	//set up light
	int				m_nLightCount;
	D3DXVECTOR3 	m_vLoc, m_vAt;
    DWORD dwPosLightsSupported = 1;
    
	m_vLoc = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_vAt  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	// Light1: D3DLIGHT_DIRECTIONAL (no position, not affected by attenuation or range)
	LIGHT Light;
	
	Light.Type        = D3DLIGHT_DIRECTIONAL;
	Light.Diffuse.r   = 1.0f;
	Light.Diffuse.g   = 1.0f;
	Light.Diffuse.b   = 1.0f;
	Light.Ambient.r   = 1.0f;
	Light.Ambient.g   = 1.0f;
	Light.Ambient.b   = 1.0f;
	Light.Specular.r  = 1.0f;
	Light.Specular.g  = 1.0f;
	Light.Specular.b  = 1.0f;
	Light.Direction   = m_vAt;
	Light.Falloff	  = 1.0f;

	SetLight(0, &Light);
       	
	if (KeySet("nol"))
		LightEnable(0, FALSE);
	else
		LightEnable(0, TRUE);
	
	const DEVICEDESC* pCaps = m_pSrcDevice->GetCaps();
	if(pCaps->dwVertexShaderVersion >= D3DVS_VERSION(1, 1))
	{
		float lightDir[4] = {0.f, 0.0f, -1.0f, 0}; 
		SetVertexShaderConstant( 7, lightDir, 1 );
	}

	return true;
}

//------------------------------------------------------------------------------

bool CNonPowTwoTest::SetDefaultStates()
{
	bool bResult = true;
	
	//disable alpha blending
	//bResult &= SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);			//it's default value

	//set up fvf parameters
	if (g_bUseTL)
	{
		bResult &= SetVertexShader(TLFVF);
	}
	else
		bResult &= SetVertexShader(FVF);

	//set default white material
	MATERIAL mat;

	memset(&mat, 0 , sizeof(MATERIAL));
	mat.diffuse.a = 0.5f;
	mat.diffuse.r = 1.0f;
	mat.diffuse.g = 1.0f;
	mat.diffuse.b = 1.0f;
	mat.ambient.r = 0.0f;
	mat.ambient.g = 0.0f;   
	mat.ambient.b = 0.0f; 
    mat.specular.r = 1.0f;
	mat.specular.g = 1.0f;   
	mat.specular.b = 1.0f;
	mat.power = 10.0f;

	bResult &= SetMaterial(&mat);

	const DEVICEDESC* pCaps = m_pSrcDevice->GetCaps();
	if(pCaps->dwVertexShaderVersion >= D3DVS_VERSION(1, 1))
	{
		float diffuse_color[4] = { mat.diffuse.r, mat.diffuse.g, mat.diffuse.b, 0 };
		bResult &= SetVertexShaderConstantF( 4, diffuse_color, 1 );
	}

	for (UINT i = 0; i < g_MaxStageCount; i++)
	{
		//filter settings
		//bResult &= SetSamplerState(i, SAMP_MAGFILTER, D3DTEXF_POINT);				//it's default value
		//bResult &= SetSamplerState(i, SAMP_MINFILTER, D3DTEXF_POINT);				//it's default value
		//bResult &= SetSamplerState(i, SAMP_MIPFILTER, D3DTEXF_NONE);				//it's default value
		SetSamplerState(i, SAMP_MAXANISOTROPY, pCaps->dwMaxAnisotropy/2);

		//texop settings
		if (i == 0)
		{
			bResult &= SetTextureStageState(i, D3DTSS_COLORARG1, (DWORD)D3DTA_DIFFUSE);
			bResult &= SetTextureStageState(i, D3DTSS_ALPHAARG1, (DWORD)D3DTA_DIFFUSE);
		}
		else
		{
			bResult &= SetTextureStageState(i, D3DTSS_COLORARG1, (DWORD)D3DTA_CURRENT);
			bResult &= SetTextureStageState(i, D3DTSS_ALPHAARG1, (DWORD)D3DTA_CURRENT);
		}
				
		bResult &= SetTextureStageState(i, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
		
		if (i != 0) //for 0th stage it's already default value
			bResult &= SetTextureStageState(i, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);

		bResult &= SetTextureStageState(i, D3DTSS_ALPHAARG2, (DWORD)D3DTA_TEXTURE);

		//address mode settings
		bResult &= SetSamplerState(i, SAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		bResult &= SetSamplerState(i, SAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		bResult &= SetSamplerState(i, SAMP_ADDRESSW, D3DTADDRESS_CLAMP);

		//wrap settings
		//bResult &= SetRenderState((D3DRENDERSTATETYPE)(D3DRS_WRAP0 + i), 0);			//it's default value
	}

	return bResult;
}

//------------------------------------------------------------------------------

void DeletionCallBack(CTestVariable* pDelete)
{
	delete pDelete;
}
