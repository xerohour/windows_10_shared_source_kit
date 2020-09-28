#include "TestCaseStruct.h"
#include "TestVariable.h"

#ifdef FAST_TEST
const SLODBiasCase g_LODBiasCases[] =
{
    { 0.0f, 1.0f, 2.0f },
    { 6.8f, 7.5f, 8.3f },
};
#else
const SLODBiasCase g_LODBiasCases[] =
{
    { 0.0f, 1.0f, 2.0f },
    { 3.0f, 4.0f, 5.0f },
    { 6.8f, 7.5f, 8.3f },
};
#endif



//------------------------------------------------------------------------------

bool CTestVariableImpl<SLODBiasCase>::InitializeResources(CD3DTest* pTest)
{
	return true;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SLODBiasCase>::CleanupResources()
{
}

//------------------------------------------------------------------------------

UINT CTestVariableImpl<SLODBiasCase>::GetUnfilteredDomainSize() const
{
	return (sizeof(g_LODBiasCases) / (sizeof(g_LODBiasCases[0])));
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SLODBiasCase>::GetVariation(SLODBiasCase* pOut, UINT nIndex) const 
{
	memcpy(pOut,&(g_LODBiasCases[nIndex]), sizeof(SLODBiasCase));
}

//------------------------------------------------------------------------------

bool CTestVariableImpl<SLODBiasCase>::CheckTestRequiredCaps(const UINT uPIndex, const UINT uDIndex, CD3DTest* pTest)
{
	DEVICEDESC* DeviceCaps = pTest->m_pSrcDevice->GetCaps();

	if (DeviceCaps->dwRasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS)
		return true;
	else
		return false;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SLODBiasCase>::ExpendTestStateName(TCHAR *testName)
{
	TCHAR tempName[256];

	strcpy(tempName, _T("Biases: "));

	for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[g_uTexCaseIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[g_uTexCaseIndex].TexIndices[i]] )
			continue;
				
		_stprintf(tempName, _T("%s%.2f, "), tempName, m_CurrentState.LODBias[i]);
	}

	strncat(testName, tempName, NAME_SIZE-strlen(testName));
}

//------------------------------------------------------------------------------

bool CTestVariableImpl<SLODBiasCase>::PutTestIntoState(CD3DTest* pTest)
{
	bool bResult = true;

	for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[g_uTexCaseIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[g_uTexCaseIndex].TexIndices[i]] )
			continue;

		DWORD index = g_TextureCases[g_uTexCaseIndex].StageIndices[i];

		//we don't want to duplicate the default value to avoid warning messages
		if ( 0.f != m_CurrentState.LODBias[i] )
			bResult &= pTest->SetSamplerState(index, SAMP_MIPMAPLODBIAS, *((DWORD*) (&(m_CurrentState.LODBias[i]))));
		bResult &= pTest->SetSamplerState(index, SAMP_MIPFILTER, D3DTEXF_POINT);
	}

	return bResult;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SLODBiasCase>::UpdateTestStatus(CD3DTest* pTest)
{
	for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[g_uTexCaseIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[g_uTexCaseIndex].TexIndices[i]] )
			continue;

		TCHAR szLODBias[120];
		
		_stprintf(szLODBias, _T("$gStage %u: $y%.2f"), g_TextureCases[g_uTexCaseIndex].StageIndices[i], m_CurrentState.LODBias[i]);
		
		pTest->WriteStatus(_T("$gLODBias"), szLODBias);

		//set default 0 bias and disable mip-mapping
		float bias = 0.f;
		//we don't want to duplicate the default value to avoid warning messages
		if ( 0.f != m_CurrentState.LODBias[i] )
			pTest->SetSamplerState(g_TextureCases[g_uTexCaseIndex].StageIndices[i], SAMP_MIPMAPLODBIAS, *((DWORD*) (&(bias))));
		pTest->SetSamplerState(g_TextureCases[g_uTexCaseIndex].StageIndices[i], SAMP_MIPFILTER, D3DTEXF_NONE);
	}
}