#include "TestCaseStruct.h"
#include "TestVariable.h"

STextureOpCase g_TextureOpCases[] ={{ D3DTOP_MODULATE,                  D3DTEXOPCAPS_MODULATE                  },  // = 4
									{ D3DTOP_SELECTARG1,                D3DTEXOPCAPS_SELECTARG1                },  // = 2,
									{ D3DTOP_SELECTARG2,                D3DTEXOPCAPS_SELECTARG2                },  // = 3,
									{ D3DTOP_MODULATE2X,                D3DTEXOPCAPS_MODULATE2X                },  // = 5,
									{ D3DTOP_MODULATE4X,                D3DTEXOPCAPS_MODULATE4X                },  // = 6,
									{ D3DTOP_ADD,                       D3DTEXOPCAPS_ADD                       },  // = 7,
									{ D3DTOP_ADDSIGNED,                 D3DTEXOPCAPS_ADDSIGNED                 },  // = 8,
									{ D3DTOP_ADDSIGNED2X,               D3DTEXOPCAPS_ADDSIGNED2X               },  // = 9,
									{ D3DTOP_SUBTRACT,                  D3DTEXOPCAPS_SUBTRACT                  },  // = 10,
									{ D3DTOP_ADDSMOOTH,                 D3DTEXOPCAPS_ADDSMOOTH                 },  // = 11,
									//{ D3DTOP_BLENDDIFFUSEALPHA,         D3DTEXOPCAPS_BLENDDIFFUSEALPHA       },  // = 12,
									{ D3DTOP_BLENDTEXTUREALPHA,         D3DTEXOPCAPS_BLENDTEXTUREALPHA         },  // = 13,
									//{ D3DTOP_BLENDFACTORALPHA,          D3DTEXOPCAPS_BLENDFACTORALPHA        },  // = 14,
									//{ D3DTOP_BLENDTEXTUREALPHAPM,       D3DTEXOPCAPS_BLENDTEXTUREALPHAPM     },  // = 15,
									//{ D3DTOP_BLENDCURRENTALPHA,         D3DTEXOPCAPS_BLENDCURRENTALPHA       },  // = 16,
									//{ D3DTOP_PREMODULATE,               D3DTEXOPCAPS_PREMODULATE             },  // = 17,
									{ D3DTOP_MODULATEALPHA_ADDCOLOR,    D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR    },  // = 18,
									{ D3DTOP_MODULATECOLOR_ADDALPHA,    D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA    },  // = 19,
									{ D3DTOP_MODULATEINVALPHA_ADDCOLOR, D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR },  // = 20,
									{ D3DTOP_MODULATEINVCOLOR_ADDALPHA, D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA },  // = 21,
									//{ D3DTOP_BUMPENVMAP,      // = 22,
									//{ D3DTOP_BUMPENVMAPLUMINANCE,      // = 23,
									{ D3DTOP_DOTPRODUCT3,               D3DTEXOPCAPS_DOTPRODUCT3               },  // = 24,
									{ D3DTOP_MULTIPLYADD,               D3DTEXOPCAPS_MULTIPLYADD               },  // = 25,
									{ D3DTOP_LERP,                      D3DTEXOPCAPS_LERP                      },  // = 26,
    							};

#define TEXOP_ARRAY_SIZE ( sizeof(g_TextureOpCases) / (sizeof(g_TextureOpCases[0])) )
#define SZTEXOP_ARRAY_SIZE 27   //D3DTOP_LERP = 26 is the highest index
//STextureOpCase *g_pTextureOpCases = NULL;

const TCHAR* g_szTextureOps[SZTEXOP_ARRAY_SIZE]; 

#define ADD_TEXOP(x) { g_szTextureOps[x] = _T(#x) + 7; } //7 is length of "D3DTOP_"

//------------------------------------------------------------------------------

bool CTestVariableImpl<STextureOpCase>::InitializeResources(CD3DTest* pTest)
{
	//initialize array with names of textureops
	for (int i = 0; i < SZTEXOP_ARRAY_SIZE; i++)
		g_szTextureOps[i] = "UNKNOWN";

	ADD_TEXOP(D3DTOP_DISABLE);
    ADD_TEXOP(D3DTOP_SELECTARG1);
    ADD_TEXOP(D3DTOP_SELECTARG2);
    ADD_TEXOP(D3DTOP_MODULATE);
    ADD_TEXOP(D3DTOP_MODULATE2X);
    ADD_TEXOP(D3DTOP_MODULATE4X);
    ADD_TEXOP(D3DTOP_ADD);
    ADD_TEXOP(D3DTOP_ADDSIGNED);
    ADD_TEXOP(D3DTOP_ADDSIGNED2X);
    ADD_TEXOP(D3DTOP_SUBTRACT);
    ADD_TEXOP(D3DTOP_ADDSMOOTH);
    ADD_TEXOP(D3DTOP_BLENDDIFFUSEALPHA);
    ADD_TEXOP(D3DTOP_BLENDTEXTUREALPHA);
    ADD_TEXOP(D3DTOP_BLENDFACTORALPHA);
    ADD_TEXOP(D3DTOP_BLENDTEXTUREALPHAPM);
    ADD_TEXOP(D3DTOP_BLENDCURRENTALPHA);
    ADD_TEXOP(D3DTOP_PREMODULATE);
    ADD_TEXOP(D3DTOP_MODULATEALPHA_ADDCOLOR);
    ADD_TEXOP(D3DTOP_MODULATECOLOR_ADDALPHA);
    ADD_TEXOP(D3DTOP_MODULATEINVALPHA_ADDCOLOR);
    ADD_TEXOP(D3DTOP_MODULATEINVCOLOR_ADDALPHA);
    //ADD_TEXOP(D3DTOP_BUMPENVMAP);
    //ADD_TEXOP(D3DTOP_BUMPENVMAPLUMINANCE);
    ADD_TEXOP(D3DTOP_DOTPRODUCT3);
    ADD_TEXOP(D3DTOP_MULTIPLYADD);
    ADD_TEXOP(D3DTOP_LERP);

	return true;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<STextureOpCase>::CleanupResources()
{
}

//------------------------------------------------------------------------------

UINT CTestVariableImpl<STextureOpCase>::GetUnfilteredDomainSize() const
{
	return TEXOP_ARRAY_SIZE;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<STextureOpCase>::GetVariation(STextureOpCase* pOut, UINT nIndex) const 
{
	memcpy(pOut,&(g_TextureOpCases[nIndex]), sizeof(STextureOpCase));
}

//------------------------------------------------------------------------------

bool CTestVariableImpl<STextureOpCase>::CheckTestRequiredCaps(const UINT uPIndex, const UINT uDIndex, CD3DTest* pTest)
{
	DEVICEDESC* DeviceCaps = pTest->m_pSrcDevice->GetCaps();

	if (g_TextureOpCases[uDIndex].dwRequiredCaps != (g_TextureOpCases[uDIndex].dwRequiredCaps & DeviceCaps->dwTextureOpCaps) )
		return false;

	if (g_TextureOpCases[uDIndex].TextureOp == D3DTOP_MULTIPLYADD && !(D3DPMISCCAPS_TSSARGTEMP & DeviceCaps->dwPrimitiveMiscCaps))
		return false;

	return true;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<STextureOpCase>::ExpendTestStateName(TCHAR *testName)
{
	TCHAR tempName[256];

	tempName[0] = '\0';

	_stprintf(tempName, _T("%sTexOp %s, "), tempName, g_szTextureOps[m_CurrentState.TextureOp]);

	strncat(testName, tempName, NAME_SIZE-strlen(testName));
}

//------------------------------------------------------------------------------

bool CTestVariableImpl<STextureOpCase>::PutTestIntoState(CD3DTest* pTest)
{
	bool bResult = true;
	MATERIAL mat;

	memset(&mat, 0 , sizeof(MATERIAL));
	mat.diffuse.a = 0.5f;
	mat.ambient.r = 0.0f;
	mat.ambient.g = 0.0f;   
	mat.ambient.b = 0.0f; 
    mat.specular.r = 1.0f;
	mat.specular.g = 1.0f;   
	mat.specular.b = 1.0f;
	mat.power = 10.0f;

	//set up some material based on TexOp
	if ((m_CurrentState.TextureOp <= D3DTOP_ADDSMOOTH &&
		 m_CurrentState.TextureOp >= D3DTOP_ADD       &&
		 m_CurrentState.TextureOp != D3DTOP_SUBTRACT) ||
		 m_CurrentState.TextureOp == D3DTOP_BLENDTEXTUREALPHAPM ||
		 m_CurrentState.TextureOp == D3DTOP_MODULATEALPHA_ADDCOLOR ||
		 m_CurrentState.TextureOp == D3DTOP_MODULATEINVALPHA_ADDCOLOR)
	{
		mat.diffuse.r = 0.0f;
		mat.diffuse.g = 0.0f;
		mat.diffuse.b = 0.0f;
	}
	else if ((DWORD)m_CurrentState.TextureOp == D3DTOP_MULTIPLYADD ||
		     (DWORD)m_CurrentState.TextureOp == D3DTOP_LERP         )
	{
		mat.diffuse.r = 0.5f;
		mat.diffuse.g = 0.5f;
		mat.diffuse.b = 0.5f;
	}
	else
	{
		mat.diffuse.r = 1.0f;
		mat.diffuse.g = 1.0f;
		mat.diffuse.b = 1.0f;
	}
	
	pTest->SetMaterial(&mat);

	DWORD uActTex = 0, iMaxStageIndex = -1;
	UINT uStageIndex;
		
	for (UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[g_uTexCaseIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[g_uTexCaseIndex].TexIndices[i]] )
			continue;

		uStageIndex = g_TextureCases[g_uTexCaseIndex].StageIndices[i];
		iMaxStageIndex = uStageIndex;
        		
		if ((uActTex == 0 && D3DTOP_SELECTARG1 == m_CurrentState.TextureOp) ||
			 D3DTOP_MODULATEALPHA_ADDCOLOR     == m_CurrentState.TextureOp  ||
			 D3DTOP_MODULATECOLOR_ADDALPHA     == m_CurrentState.TextureOp  ||
			 D3DTOP_MODULATEINVALPHA_ADDCOLOR  == m_CurrentState.TextureOp  ||
			 D3DTOP_MODULATEINVCOLOR_ADDALPHA  == m_CurrentState.TextureOp   )
		{
			pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		}
		else if (D3DTOP_PREMODULATE == m_CurrentState.TextureOp)
		{
			if (uActTex == 0)
				pTest->SetTextureStageState(uStageIndex,  D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		}
		else if (D3DTOP_MULTIPLYADD == m_CurrentState.TextureOp)
		{
			if (uActTex == 0 )
			{
				pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG0, (DWORD)D3DTA_TEMP);
			}
			/*else
			{
				pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG0, (DWORD)D3DTA_CURRENT);
			}*/
			//we don't want to duplicate the default value to avoid warning messages
			//this state was already set in CTestVariableImpl<STextureCase>::PutTestIntoState()
			//pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG2, (DWORD)D3DTA_TEXTURE);
		}
		else if (D3DTOP_LERP == m_CurrentState.TextureOp)
		{
			pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG0, (DWORD)D3DTA_DIFFUSE);
			//we don't want to duplicate the default value to avoid warning messages
			if (uStageIndex == 0)
				pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG1, (DWORD)D3DTA_CURRENT);
			//we don't want to duplicate the default value to avoid warning messages
			//this state was already set in CTestVariableImpl<STextureCase>::PutTestIntoState()
			//pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG2, (DWORD)D3DTA_TEXTURE);
		}
		//we don't want to duplicate the default value to avoid warning messages
		//this state was already set in CTestVariableImpl<STextureCase>::PutTestIntoState()
		//else
		//	pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG2, (DWORD)D3DTA_TEXTURE);

		if ((g_TexInfo[g_TextureCases[g_uTexCaseIndex].TexIndices[i]].Format.d3dfFormat != FMT_A8 && D3DTOP_MODULATE != m_CurrentState.TextureOp) ||
			g_TexInfo[g_TextureCases[g_uTexCaseIndex].TexIndices[i]].Format.d3dfFormat == FMT_A8 && D3DTOP_BLENDTEXTUREALPHA != m_CurrentState.TextureOp)
			pTest->SetTextureStageState(uStageIndex, D3DTSS_COLOROP, (DWORD)m_CurrentState.TextureOp);

		uActTex++;
	}

	//we don't want to duplicate the default value to avoid warning messages
	//this state was already set in CTestVariableImpl<STextureCase>::PutTestIntoState()
	//if ( iMaxStageIndex < 7 )
	//	pTest->SetTextureStageState(iMaxStageIndex+1, D3DTSS_COLOROP, (DWORD)D3DTOP_DISABLE);

	return bResult;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<STextureOpCase>::UpdateTestStatus(CD3DTest* pTest)
{
	int iMaxStageIndex = -1, uActTex = 0;
	UINT uStageIndex;

	for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[g_uTexCaseIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[g_uTexCaseIndex].TexIndices[i]] )
			continue;

		uStageIndex = g_TextureCases[g_uTexCaseIndex].StageIndices[i];
		iMaxStageIndex = uStageIndex;

		TCHAR szTexop[120];

		_stprintf(szTexop, _T("$gStage %u: $y%s"), uStageIndex, g_szTextureOps[m_CurrentState.TextureOp]);
		pTest->WriteStatus(_T("$gTextureOp"), szTexop);

		//set default settings
		if ((uActTex == 0 && D3DTOP_SELECTARG1 == m_CurrentState.TextureOp) ||
			 D3DTOP_MODULATEALPHA_ADDCOLOR     == m_CurrentState.TextureOp  ||
			 D3DTOP_MODULATECOLOR_ADDALPHA     == m_CurrentState.TextureOp  ||
			 D3DTOP_MODULATEINVALPHA_ADDCOLOR  == m_CurrentState.TextureOp  ||
			 D3DTOP_MODULATEINVCOLOR_ADDALPHA  == m_CurrentState.TextureOp   )
		{
			if (uStageIndex == 0)
				pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG1, (DWORD)D3DTA_DIFFUSE);
			else
				pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG1, (DWORD)D3DTA_CURRENT);
		}
		else if (D3DTOP_PREMODULATE == m_CurrentState.TextureOp)
		{
			if (uActTex == 0)
			{
				if (uStageIndex == 0)
					pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG1, (DWORD)D3DTA_DIFFUSE);
				else
					pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG1, (DWORD)D3DTA_CURRENT);
			}
		}
		else if (D3DTOP_MULTIPLYADD == m_CurrentState.TextureOp)
		{
			if (uActTex == 0 )
			{
				pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG0, (DWORD)D3DTA_CURRENT);
			}
			//we don't want to duplicate the default value to avoid warning messages
			//this state will be set in CTestVariableImpl<STextureCase>::UpdateTestStatus()
			//pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT);
		}
		else if (D3DTOP_LERP == m_CurrentState.TextureOp)
		{
			pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG0, (DWORD)D3DTA_CURRENT);
			//we don't want to duplicate the default value to avoid warning messages
			//this state will be set in CTestVariableImpl<STextureCase>::UpdateTestStatus()
			//pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT);
			if (uStageIndex == 0)
				pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG1, (DWORD)D3DTA_DIFFUSE);
			//else
			//	pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG1, (DWORD)D3DTA_CURRENT);
		}
		//we don't want to duplicate the default value to avoid warning messages
		//this state will be set in CTestVariableImpl<STextureCase>::UpdateTestStatus()
		//else
		//	pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT);

		if (D3DTOP_SELECTARG1 == m_CurrentState.TextureOp)
			pTest->SetTextureStageState(uStageIndex, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE);

		uActTex++;

		/*if (uStageIndex == 0)
			pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG1, (DWORD)D3DTA_DIFFUSE);
		else
			pTest->SetTextureStageState(uStageIndex, D3DTSS_COLORARG1, (DWORD)D3DTA_CURRENT);
				
		pTest->SetTextureStageState(uStageIndex, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);*/
	}

	//we don't want to duplicate the default value to avoid warning messages
	//this state was already set in CTestVariableImpl<STextureCase>::PutTestIntoState()
	//if ( iMaxStageIndex < 7 )
	//	pTest->SetTextureStageState(iMaxStageIndex + 1, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

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
	
	pTest->SetMaterial(&mat);
}
