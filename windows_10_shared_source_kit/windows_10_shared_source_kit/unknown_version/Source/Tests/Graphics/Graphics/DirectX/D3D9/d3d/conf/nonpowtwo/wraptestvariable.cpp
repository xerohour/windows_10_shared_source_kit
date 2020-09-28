#include "TestCaseStruct.h"
#include "TestVariable.h"

#ifdef FAST_TEST
const SWrapCase g_WrapCases[] = { {D3DWRAP_U, D3DWRAP_U,D3DWRAP_U},
								  {D3DWRAP_U | D3DWRAP_V | D3DWRAP_W, D3DWRAP_U | D3DWRAP_V | D3DWRAP_W, D3DWRAP_U | D3DWRAP_V | D3DWRAP_W } };
#else
const SWrapCase g_WrapCases[] = { { D3DWRAP_U, D3DWRAP_U, D3DWRAP_U },
{ D3DWRAP_V, D3DWRAP_V, D3DWRAP_V },
{ D3DWRAP_U | D3DWRAP_V, D3DWRAP_U | D3DWRAP_V, D3DWRAP_U | D3DWRAP_V },
{ D3DWRAP_U | D3DWRAP_V | D3DWRAP_W, D3DWRAP_U | D3DWRAP_V | D3DWRAP_W, D3DWRAP_U | D3DWRAP_V | D3DWRAP_W } };
#endif
#define SZWRAP_ARRAY_SIZE 5   //D3DWRAP_W = 4 in the highest index

const TCHAR* g_szWraps[SZWRAP_ARRAY_SIZE];

#define ADD_WRAP(x) {g_szWraps[x] = _T(#x) + 3;} //3 is length of "D3D"

//------------------------------------------------------------------------------

bool CTestVariableImpl<SWrapCase>::InitializeResources(CD3DTest* pTest)
{
	//initialize array with names of filters
	for (int i = 0; i < SZWRAP_ARRAY_SIZE; i++)
		g_szWraps[i] = "UNKNOWN";

	ADD_WRAP( D3DWRAP_U  );
	ADD_WRAP( D3DWRAP_V  );        
	ADD_WRAP( D3DWRAP_W  );       
	
	return true;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SWrapCase>::CleanupResources()
{
}

//------------------------------------------------------------------------------

UINT CTestVariableImpl<SWrapCase>::GetUnfilteredDomainSize() const
{
	return (sizeof(g_WrapCases) / sizeof(g_WrapCases[0]));
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SWrapCase>::GetVariation(SWrapCase* pOut, const UINT nIndex) const
{
	memcpy(pOut, &(g_WrapCases[nIndex]), sizeof(SWrapCase));
}

//------------------------------------------------------------------------------

bool CTestVariableImpl<SWrapCase>::CheckTestRequiredCaps(const UINT uPIndex, const UINT uDIndex, CD3DTest* pTest)
{
	DEVICEDESC* DeviceCaps = pTest->m_pSrcDevice->GetCaps();

	for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[uPIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[uPIndex].TexIndices[i]] )
			continue;

		//if NonPow2Cond cap is set and we use nonPow2 single level 2D textures then no wrapping is allowed
		if ((DeviceCaps->dwTextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) &&
			TEXTURE_2D == g_TexInfo[g_TextureCases[uPIndex].TexIndices[i]].TexType &&
			1 == g_TexInfo[g_TextureCases[uPIndex].TexIndices[i]].uMipCount &&
			!(pTest->KeySet("POW2")) &&
			0 != g_WrapCases[uDIndex].dwWrapParams[i])
		{
			return false;
		}

		//wrapping with cube maps doesn't make sense
		if (TEXTURE_CUBE == g_TexInfo[g_TextureCases[uPIndex].TexIndices[i]].TexType)
		{
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SWrapCase>::ExpendTestStateName(TCHAR *testName)
{
	TCHAR tempName[256];

	strcpy(tempName, "TexWrap: ");
	
	/*for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[g_uTexCaseIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[g_uTexCaseIndex].TexIndices[i]] )
			continue;*/
		
		if(D3DWRAP_U == (D3DWRAP_U & m_CurrentState.dwWrapParams[0/*i*/]))
		{
			_tcscat(tempName, _T("WRAP_U"));
		}
		if(D3DWRAP_V == (D3DWRAP_V & m_CurrentState.dwWrapParams[0/*i*/]))
		{
			_tcscat(tempName, _T(" | WRAP_V"));
		}
		if(D3DWRAP_W == (D3DWRAP_W & m_CurrentState.dwWrapParams[0/*i*/]))
		{
			_tcscat(tempName, _T(" | WRAP_W"));
		}

		_tcscat(tempName, _T(", "));
	//}

	strncat(testName, tempName, NAME_SIZE-strlen(testName));
}

//------------------------------------------------------------------------------

bool CTestVariableImpl<SWrapCase>::PutTestIntoState(CD3DTest* pTest)
{
	bool bResult = true;

	for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[g_uTexCaseIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[g_uTexCaseIndex].TexIndices[i]] )
			continue;

		DWORD uStage = g_TextureCases[g_uTexCaseIndex].StageIndices[i];

		bResult &= pTest->SetRenderState((D3DRENDERSTATETYPE)(D3DRS_WRAP0 + uStage), m_CurrentState.dwWrapParams[i]);
	}

	g_isWrapCase = true;

	return bResult;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SWrapCase>::UpdateTestStatus(CD3DTest* pTest)
{
	TCHAR szWrap[256];
	
	for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[g_uTexCaseIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[g_uTexCaseIndex].TexIndices[i]] )
			continue;

		DWORD uStage = g_TextureCases[g_uTexCaseIndex].StageIndices[i];

		ZeroMemory(szWrap, sizeof(szWrap));
		_stprintf(szWrap, "$gStage %u: ", uStage);

		if(D3DWRAP_U == (D3DWRAP_U & m_CurrentState.dwWrapParams[i]))
		{
			_tcscat(szWrap, _T("$yD3DWRAP_U "));
		}
		if(D3DWRAP_V == (D3DWRAP_V & m_CurrentState.dwWrapParams[i]))
		{
			_tcscat(szWrap, _T("$y | D3DWRAP_V"));
		}
		if(D3DWRAP_W == (D3DWRAP_W & m_CurrentState.dwWrapParams[i]))
		{
			_tcscat(szWrap, _T("$y | D3DWRAP_W"));
		}
		/*if(D3DWRAPCOORD_3 == (D3DWRAPCOORD_3 & m_CurrentState.dwWrapParams[i]))
		{
			_tcscat(szWrap, _T("|D3DWRAPCOORD_3"));
		}*/

		pTest->WriteStatus(_T("$gTexCoordWrap"), szWrap);

		//disable wrapping
		pTest->SetRenderState((D3DRENDERSTATETYPE)(D3DRS_WRAP0 + uStage), 0);
		g_isWrapCase = false;
	}
}