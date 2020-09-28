#include "TestCaseStruct.h"
#include "TestVariable.h"
#include "NonPowTwoTest.h"
#include "CD3DX.h"

SVShaderCase g_VShaderCases[] = 
{
	{
		FALSE, FALSE, FALSE,
		0, _T("No TexGen"),	
		0
	},
	{
		FALSE, FALSE, FALSE,
		D3DTSS_TCI_CAMERASPACEPOSITION,	_T("CAMERASPACEPOSITION"),
		D3DVTXPCAPS_TEXGEN
	},
	{
		FALSE, FALSE, FALSE,
		D3DTSS_TCI_SPHEREMAP, _T("SphereMap"),
		(D3DVTXPCAPS_TEXGEN | D3DVTXPCAPS_TEXGEN_SPHEREMAP)
	},
	{
		TRUE, FALSE, FALSE,
		0, _T(""), 
		0
	},
	{
		TRUE, TRUE, FALSE,
		0, _T(""), 0

	},
	{
		FALSE, TRUE, FALSE,
		0, _T("No TexGen"),
		0	
	},
	{
		FALSE, FALSE, TRUE,
		0, _T("No TexGen"),
		0
	},	
	{
		TRUE, TRUE, TRUE,
		0, _T(""),
		0
	},
};


//------------------------------------------------------------------------------

bool CTestVariableImpl<SVShaderCase>::InitializeResources(CD3DTest* pTest)
{
	return true;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SVShaderCase>::CleanupResources()
{
}

//------------------------------------------------------------------------------

UINT CTestVariableImpl<SVShaderCase>::GetUnfilteredDomainSize() const
{
	return (sizeof(g_VShaderCases) / (sizeof(g_VShaderCases[0])));
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SVShaderCase>::GetVariation(SVShaderCase* pOut, const UINT nIndex) const 
{
	memcpy(pOut, &(g_VShaderCases[nIndex]), sizeof(SVShaderCase));
}

//------------------------------------------------------------------------------

bool CTestVariableImpl<SVShaderCase>::CheckTestRequiredCaps(const UINT uPIndex, const UINT uDIndex, CD3DTest* pTest)
{
	DEVICEDESC* DeviceCaps = pTest->m_pSrcDevice->GetCaps();

	if( !g_VShaderCases[uDIndex].bDecl )
	{
		if(g_VShaderCases[uDIndex].dwVPCaps != (DeviceCaps->dwVertexProcessingCaps & g_VShaderCases[uDIndex].dwVPCaps))
		{
			return false;
		}
	}
	
	if( g_VShaderCases[uDIndex].bVShader )
	{
		//require vshader version 1.1
		if(DeviceCaps->dwVertexShaderVersion < D3DVS_VERSION(1, 1))
		{
			return false;
		}
	}

	if( g_VShaderCases[uDIndex].bPShader )
	{
		//require pshader version 1.4
        if(DeviceCaps->dwPixelShaderVersion < D3DPS_VERSION(1, 4))
		{
			return false;
		}
	}
		

	return true;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SVShaderCase>::ExpendTestStateName(TCHAR *testName)
{
	TCHAR tempName[256];

	if(m_CurrentState.bDecl)
	{
		sprintf(tempName, "decl-");
	}
	else
	{
		sprintf(tempName, "fvf-");
	}

	if(m_CurrentState.bVShader)
	{
		strcat(tempName, "vshader, ");
	}
	else
	{
		strcat(tempName, "fixed, ");
	}

	if(m_CurrentState.bPShader)
	{
		strcat(tempName, "pshader, ");
	}
	
	if(!m_CurrentState.bDecl)
	{
		sprintf(tempName, _T("%sTexGen: %s; "), tempName, m_CurrentState.szTexCoordIndex);
	}

	strncat(testName, tempName, NAME_SIZE-strlen(testName));	
}

//------------------------------------------------------------------------------

bool CTestVariableImpl<SVShaderCase>::PutTestIntoState(CD3DTest* pTest)
{
	ID3DXBuffer* pCompiledShader = NULL;
	CnVertexShader*     pVShader = NULL;
	CnPixelShader*      pPShader = NULL;
	DWORD*       pShaderFunction = NULL;

	bool bResult = true;

	if(m_CurrentState.bDecl)
	{
		CnVertexDeclaration *pDecl = NULL;
		
		//create declaration
		VERTEXELEMENT decl[] = 
		{
			{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
			{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
			{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
			{0, 32, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 1},
			{0, 44, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 2},
			DECL_END()
		};

		VERTEXELEMENT declTL[] = 
		{
			{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT4, DECLMETHOD_DEFAULT, DECLUSAGE_POSITIONT, 0},
			{0, 16, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_COLOR,  0},
			{0, 20, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
			{0, 28, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 1},
			{0, 40, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 2},
			DECL_END()
		};

		if(!pTest->CreateVertexDeclaration( g_bUseTL && !m_CurrentState.bVShader ? declTL : decl, &(pDecl)))
		{
			pTest->WriteToLog( "GenerateVertexShader - CreateVertexDeclaration() failed. hr=%s.\n", pTest->m_pD3D->HResultToString(GetLastError()) );
		}

		bResult &= pTest->SetVertexDeclaration(pDecl);
		
		RELEASE(pDecl);
	}

	if ( m_CurrentState.bVShader )
	{
		TCHAR szShader[500];
		TCHAR tempStr[50];

		g_isVShaderCase = true;
		
		if (g_bUseTL)
			pTest->SetVertexShader(FVF);

		/*TCHAR szTempTL2[500] = _T("vs.1.1\n\
		   dcl_position   v0 \n\
		   dcl_color      v1 \n\
		   dcl_texcoord0  v2 \n\
		   dcl_texcoord1  v3 \n\
		   dcl_texcoord2  v4 \n\
		   m4x4 oPos, v0, c0 \n\
		   mov  oD0,  v1     \n");*/

		TCHAR szTemp[500] = _T("vs.1.1\n\
		   dcl_position   v0 \n\
		   dcl_normal     v1 \n\
		   dcl_texcoord0  v2 \n\
		   dcl_texcoord1  v3 \n\
		   dcl_texcoord2  v4 \n\
		   m4x4 oPos, v0, c0 \n\
		   dp3  r0.x, v1, c7 \n\
		   mul  oD0, r0.x, c4 \n");
		   ////add  oD0, r1,  c6 \n" )
		
		/*if (pTest->KeySet("tl2"))
			strcpy(szShader, szTempTL2);
		else*/
			strcpy(szShader, szTemp);

		for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
		{
			UINT uStage   = g_TextureCases[g_uTexCaseIndex].StageIndices[i];
			UINT texIndex = g_TextureCases[g_uTexCaseIndex].TexIndices[i];
			
			if (  -1 ==  texIndex || NULL == g_pTextures[texIndex] )
				continue;

			if ( g_TexInfo[texIndex].TexType != uStage )
				pTest->SetTextureStageState(uStage, D3DTSS_TEXCOORDINDEX, uStage);

			sprintf(tempStr, "mov oT%d, v%d\n", uStage, g_TexInfo[texIndex].TexType+2);
			strcat(szShader, tempStr);
		}

		if(FAILED(D3DXAssembleShader9(szShader, (UINT)_tcslen(szShader), NULL, NULL, D3DXSHADER_DEBUG, &pCompiledShader, NULL)))
		{
			bResult = false;
		}
		else
		{
			pShaderFunction = (DWORD*)pCompiledShader->GetBufferPointer();
			bResult &= pTest->CreateVertexShader(pShaderFunction, &pVShader);
		}

		bResult &= pTest->SetVertexShader(pVShader);
		
		RELEASE(pVShader);
		RELEASE(pCompiledShader);
	}

	if ( m_CurrentState.bPShader )
	{
		TCHAR szPShader[1000];
		TCHAR tempStr[50];
		
		strcpy(szPShader, "ps.1.4 \n"); 
		int prevIndex = -1;

		for (UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
		{
			UINT uStage   = g_TextureCases[g_uTexCaseIndex].StageIndices[i];
			UINT texIndex = g_TextureCases[g_uTexCaseIndex].TexIndices[i];
			
			if (  -1 ==  texIndex || NULL == g_pTextures[texIndex] )
				continue;

			sprintf(tempStr, "texld r%d, t%d\n", uStage, /*g_TexInfo[texIndex].TexType*/uStage);
			strcat(szPShader, tempStr);
		}

		for (UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
		{
			UINT uStage   = g_TextureCases[g_uTexCaseIndex].StageIndices[i];
			UINT texIndex = g_TextureCases[g_uTexCaseIndex].TexIndices[i];
			
			if (  -1 ==  texIndex || NULL == g_pTextures[texIndex] )
				continue;

			if (-1 == prevIndex)
			{
				if ( g_TexInfo[texIndex].Format.d3dfFormat == FMT_A8 )
					sprintf(tempStr, "mov r0, r%d.a\n", uStage);
				else
					sprintf(tempStr, "mov r0, r%d\n", uStage);
				strcat(szPShader, tempStr);
			}
			else
			{
				if ( g_TexInfo[texIndex].Format.d3dfFormat == FMT_A8  && pTest->KeySet("a8"))
					sprintf(tempStr, "add r0, r0, r%d.a\n", uStage);
				else
					
					sprintf(tempStr, "mul r0, r0, r%d\n", uStage);
				strcat(szPShader, tempStr);
			}
			
			prevIndex = uStage;
		}
		   
		if(FAILED(D3DXAssembleShader9(szPShader, _tcslen(szPShader), NULL, NULL, D3DXSHADER_DEBUG, &pCompiledShader, NULL)))
		{
			bResult = false;
		}
		else
		{
			bResult &= pTest->CreatePixelShader((LPDWORD)pCompiledShader->GetBufferPointer(), &pPShader);
			bResult &= pTest->SetPixelShader(pPShader);
				
			RELEASE(pPShader);
			RELEASE(pCompiledShader);
		}

	}

	return bResult;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SVShaderCase>::UpdateTestStatus(CD3DTest* pTest)
{
	TCHAR szPipeLine[80];

	if(m_CurrentState.bDecl)
	{
		sprintf(szPipeLine, "Decl - ");
	}
	else
	{
		sprintf(szPipeLine, "FVF - ");
	}

	if(m_CurrentState.bVShader)
	{
		strcat(szPipeLine, "Vertex Shader");

		//set default vertex shader
		pTest->SetVertexShader((CnVertexShader*)NULL);

		//we don't want to duplicate the default value to avoid warning messages
		for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
		{
			UINT uStage   = g_TextureCases[g_uTexCaseIndex].StageIndices[i];
			UINT texIndex = g_TextureCases[g_uTexCaseIndex].TexIndices[i];
			
			if (  -1 ==  texIndex || NULL == g_pTextures[texIndex] )
				continue;

			if ( g_TexInfo[texIndex].TexType != uStage )
				pTest->SetTextureStageState(uStage, D3DTSS_TEXCOORDINDEX, g_TexInfo[texIndex].TexType);
		}
	}
	else
	{
		strcat(szPipeLine, "Fixed Pipeline");
	}

	if(m_CurrentState.bPShader)
	{
		strcat(szPipeLine, " - Pixel Shader");

		//set default pshader (none)
		pTest->SetPixelShader((DWORD)0);
	}

	pTest->WriteStatus(_T("$gVertex Shader"), _T("$y%s"), szPipeLine);

	if(!m_CurrentState.bDecl)
	{
		pTest->WriteStatus(_T("$gTexGen        "), _T("$y%s"), m_CurrentState.szTexCoordIndex);
	}

	if (g_bUseTL)
		pTest->SetVertexShader(TLFVF);
	else
		pTest->SetVertexShader(FVF);
	g_isVShaderCase = false;
}
