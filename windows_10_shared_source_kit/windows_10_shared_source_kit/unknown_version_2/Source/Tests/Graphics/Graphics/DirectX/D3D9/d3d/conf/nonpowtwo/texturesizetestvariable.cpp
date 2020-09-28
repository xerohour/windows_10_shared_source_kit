#include "TestCaseStruct.h"
#include "TestVariable.h"
#include "CTextures.h"
#include "CD3DX.h"
						
#ifdef FAST_TEST
STextureCase g_TextureCases[] = { 
    { { 0, -1, -1 }, { 0, 0, 0 } },
    { { 2, 1, -1 }, { 0, 0, 0 } },
    { { -1, 4, 0 }, { 0, 0, 0 } },
    { { 3, -1, 5 }, { 0, 0, 0 } },
    { { 5, 3, 1 }, { 0, 0, 0 } }
    };
#else
STextureCase g_TextureCases[] = { { { 0, -1, -1 }, { 0, 0, 0 } },
    { { 1, 0, -1 }, { 0, 0, 0 } },
    { { 2, 1, -1 }, { 0, 0, 0 } },
    { { -1, 4, 0 }, { 0, 0, 0 } },
    { { -1, -1, 2 }, { 0, 0, 0 } },
    { { -1, 2, 3 }, { 0, 0, 0 } },
    { { 3, -1, 5 }, { 0, 0, 0 } },
    { { -1, -1, 4 }, { 0, 0, 0 } },
    { { 4, 5, -1 }, { 0, 0, 0 } },
    { { 5, 3, 1 }, { 0, 0, 0 } }
    };
#endif



//------------------------------------------------------------------------------

bool CTestVariableImpl<STextureCase>::InitializeResources(CD3DTest* pTest)
{
	UINT size = sizeof(g_TextureCases) / sizeof(g_TextureCases[0]); //prefast error GetUnfilteredDomainSize();

	UINT MaxSimTextures = pTest->m_pSrcDevice->GetCaps()->wMaxSimultaneousTextures;
	UINT texCount = 0;

	for (UINT i = 0; i < size; i++)
	{
		texCount = 0;

		//based on how many texture stages are supported randomly choose stage indices to use
		switch (g_MaxStageCount)
		{
		//disable all textures but g_TextureCases[i].TexIndices[0] since ony one blend stage is supported
		case 1: 
			for(int j = 1; j < MAX_TEXTURE_COUNT; j++)
				g_TextureCases[i].TexIndices[j] = -1;
			break;
		//set first two textures used to stage 0 and 1, disable the rest of textures
		case 2: 
			for(int j = 0; j < MAX_TEXTURE_COUNT; j++)
			{
				if (g_TextureCases[i].TexIndices[j] != -1)
				{	
					if(texCount >= 2)
						g_TextureCases[i].TexIndices[j] = -1;
					else
						g_TextureCases[i].StageIndices[j] = texCount++;
				}
			}
			texCount = 0;
			break;
		case 3:
			g_TextureCases[i].StageIndices[0] = 0;
			g_TextureCases[i].StageIndices[1] = 1;
			g_TextureCases[i].StageIndices[2] = 2;
			break;
		case 4:
			g_TextureCases[i].StageIndices[0] = 0;
			g_TextureCases[i].StageIndices[1] = 1;
			g_TextureCases[i].StageIndices[2] = 2 + rand() % 2;
			break;
		case 5:
			g_TextureCases[i].StageIndices[0] = 0;
			g_TextureCases[i].StageIndices[1] = 1 + rand() % 2;
			g_TextureCases[i].StageIndices[2] = 3 + rand() % 2;
			break;
		default:
			g_TextureCases[i].StageIndices[0] = 0 + rand() % 2;
			g_TextureCases[i].StageIndices[1] = 2 + rand() % 2;
			g_TextureCases[i].StageIndices[2] = 4 + rand() % 2;
		}

		
		//disable some textures when number of textures used exceeds MaxSimultaneousTextures
		for(int j = 0; j < MAX_TEXTURE_COUNT; j++)
		{
			if (g_TextureCases[i].TexIndices[j] != -1)
			{	
				if(++texCount > MaxSimTextures)
					g_TextureCases[i].TexIndices[j] = -1;
			}
		}
	}	

	return true;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<STextureCase>::CleanupResources()
{
}

//------------------------------------------------------------------------------

UINT CTestVariableImpl<STextureCase>::GetUnfilteredDomainSize() const
{
	return (sizeof(g_TextureCases) / sizeof(g_TextureCases[0]));
}

//------------------------------------------------------------------------------

void CTestVariableImpl<STextureCase>::GetVariation(STextureCase* pOut, const UINT nIndex) const

{
	memcpy(pOut, &(g_TextureCases[nIndex]), sizeof(STextureCase));
}

//------------------------------------------------------------------------------

bool CTestVariableImpl<STextureCase>::CheckTestRequiredCaps(const UINT uPIndex, const UINT uDIndex, CD3DTest* pTest)
{
	DEVICEDESC* DeviceCaps = pTest->m_pSrcDevice->GetCaps();
	
	UINT nTextureCount = 0;//how many textures are used by this test case?

	for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if(g_TextureCases[uDIndex].TexIndices[i] < 0 ) 
			continue;
		
		//disable the stage if its texture in NULL or we alredy have as many textures used as wMaxSimultaneousTextures allows
		if (NULL == g_pTextures[g_TextureCases[uDIndex].TexIndices[i]] || ++nTextureCount > DeviceCaps->wMaxSimultaneousTextures)
		{
			m_pDomain[uDIndex].TexIndices[i] = -1;
			continue;
		}
	}

	if (nTextureCount == 0)
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<STextureCase>::ExpendTestStateName(TCHAR *testName)
{
	TCHAR szPool[3][10] = {"DEF", "MAN", "SYS" };
	TCHAR tempName[256];

	tempName[0] = '\0';

	for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if(m_CurrentState.TexIndices[i] < 0)
		{
			continue;
		}
		
		STextureInfo& TexInfo = g_TexInfo[m_CurrentState.TexIndices[i]];
		TCHAR szFmt[30];
		FmtToString(FORMAT_TEXTURE, &(TexInfo.Format), szFmt);

		switch(TexInfo.TexType)
		{
			case TEXTURE_2D:	
				_stprintf(tempName, _T("%s%u: Tex %s %dx%d %d %s, "), tempName, m_CurrentState.StageIndices[i], szFmt, TexInfo.uWidth, TexInfo.uHeight, TexInfo.uMipCount, szPool[TexInfo.Pool]);
				break;
			case TEXTURE_3D:	
				_stprintf(tempName, _T("%s%u: Vol %s %dx%dx%d %d %s, "), tempName, m_CurrentState.StageIndices[i], szFmt, TexInfo.uWidth, TexInfo.uHeight, TexInfo.uDepth, TexInfo.uMipCount, szPool[TexInfo.Pool]);
				break;
			case TEXTURE_CUBE:	
				_stprintf(tempName, _T("%s%u: Cube %s %d %d %s, "), tempName, m_CurrentState.StageIndices[i], szFmt, TexInfo.uWidth, TexInfo.uMipCount, szPool[TexInfo.Pool]);
				break;
			default:			
				_stprintf(tempName, _T("%s%u: ?, "), tempName, m_CurrentState.StageIndices[i]);
				break;
		}	
	}

	strncat(testName, tempName, NAME_SIZE-strlen(testName));
}

//------------------------------------------------------------------------------

bool CTestVariableImpl<STextureCase>::PutTestIntoState(CD3DTest* pTest)
{
	int iMaxStageIndex = 0;

	for (UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if(m_CurrentState.TexIndices[i] < 0)
			continue;

		if (NULL == g_pTextures[m_CurrentState.TexIndices[i]])
		{
			return false;
		}
		else
		{
			//set textures
			if (!pTest->SetTexture(m_CurrentState.StageIndices[i], g_pTextures[m_CurrentState.TexIndices[i]]))
			{
				return false;
			};	

			//if one of textures is A8 then turn on alpha blending
			if ( g_TexInfo[m_CurrentState.TexIndices[i]].Format.d3dfFormat == FMT_A8 )
			{
				if (!pTest->SetTextureStageState(m_CurrentState.StageIndices[i], D3DTSS_COLORARG2, (DWORD)D3DTA_TEXTURE) ||
					!pTest->SetTextureStageState(m_CurrentState.StageIndices[i], D3DTSS_COLOROP, (DWORD)D3DTOP_BLENDTEXTUREALPHA)  )
				{
					return false;
				};
			}
			else
			{
				//set texop 
				if (!pTest->SetTextureStageState(m_CurrentState.StageIndices[i], D3DTSS_COLORARG2, (DWORD)D3DTA_TEXTURE) ||
					!pTest->SetTextureStageState(m_CurrentState.StageIndices[i], D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE)  )
				{
					return false;
				};
			}

  		    iMaxStageIndex = m_CurrentState.StageIndices[i];

			//set texcoord index
			//we don't want to duplicate the default value to avoid warning messages
			if ( g_TexInfo[m_CurrentState.TexIndices[i]].TexType != m_CurrentState.StageIndices[i] &&
				!pTest->SetTextureStageState(m_CurrentState.StageIndices[i], D3DTSS_TEXCOORDINDEX, g_TexInfo[m_CurrentState.TexIndices[i]].TexType))
			{
				return false;
			}
		}
	}

	if ( iMaxStageIndex < 7 )
	{
		pTest->SetTextureStageState(iMaxStageIndex + 1, D3DTSS_COLOROP, (DWORD)D3DTOP_DISABLE);
		//pTest->SetTextureStageState(iMaxStageIndex + 1, D3DTSS_ALPHAOP, (DWORD)D3DTOP_DISABLE);
	}

	return true;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<STextureCase>::UpdateTestStatus(CD3DTest* pTest)
{
	TCHAR szPool[3][10] = {"DEF", "MAN", "SYS" };
	int iMaxStageIndex = -1;

	for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if(m_CurrentState.TexIndices[i] < 0)
		{
			continue;
		}
		
		const STextureInfo& TexInfo = g_TexInfo[m_CurrentState.TexIndices[i]];

		TCHAR szTexInfo[120];
		FORMAT Fmt;
		const TCHAR* szTexType;

		_stprintf(szTexInfo, _T("$gStage %u: $y"), m_CurrentState.StageIndices[i]); 
	
		switch(TexInfo.TexType)
		{
			case TEXTURE_2D:
				_stprintf(szTexInfo, _T("%s%s, "), szTexInfo, _T("2D Tex"));
				_stprintf(szTexInfo, _T("%s  w=%d  h=%d  nLevels=%d, pool=%s, "), szTexInfo, TexInfo.uWidth, TexInfo.uHeight, TexInfo.uMipCount, szPool[TexInfo.Pool]);

				break;

			case TEXTURE_3D:
				_stprintf(szTexInfo, _T("%s%s, "), szTexInfo, _T("Volume Tex"));
				_stprintf(szTexInfo, _T("%s  w=%d  h=%d  d=%d  nLev=%d, pool=%s, "), szTexInfo, TexInfo.uWidth, TexInfo.uHeight, TexInfo.uDepth, TexInfo.uMipCount, szPool[TexInfo.Pool]);

				break;

			case TEXTURE_CUBE:
				_stprintf(szTexInfo, _T("%s%s, "), szTexInfo, _T("Cube Map"));
				_stprintf(szTexInfo, _T("%s  w=%d  nLevels=%d, pool=%s, "), szTexInfo, TexInfo.uWidth, TexInfo.uMipCount, szPool[TexInfo.Pool]);

				break;

			default:
				_stprintf(szTexInfo, _T("%s%s, "), szTexInfo, _T("Unknown"));
				break;
		}

		Fmt = TexInfo.Format;
		strcat(szTexInfo, _T("  fmt="));
		FmtToString(FORMAT_TEXTURE, &Fmt, szTexInfo+strlen(szTexInfo));

		pTest->WriteStatus( _T("$gTextureType"), szTexInfo);

		//diasble current texture stage
		pTest->SetTexture(m_CurrentState.StageIndices[i], NULL);

		//set default texop settings
		pTest->SetTextureStageState(m_CurrentState.StageIndices[i], D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
		pTest->SetTextureStageState(m_CurrentState.StageIndices[i], D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT);
		iMaxStageIndex = m_CurrentState.StageIndices[i];

		//we don't want to duplicate the default value to avoid warning messages
		if ( g_TexInfo[m_CurrentState.TexIndices[i]].TexType != m_CurrentState.StageIndices[i] )
			pTest->SetTextureStageState(m_CurrentState.StageIndices[i], D3DTSS_TEXCOORDINDEX, m_CurrentState.StageIndices[i]);
	}

	if ( iMaxStageIndex < 7 )
	{
		pTest->SetTextureStageState(iMaxStageIndex + 1, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
		//pTest->SetTextureStageState(iMaxStageIndex + 1, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);
	}
}
