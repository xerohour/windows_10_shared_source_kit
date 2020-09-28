#include "TestCaseStruct.h"
#include "TestVariable.h"

// Skip these in fast mode, for testing specifically aimed at blend ops, use alphabld
#ifdef FAST_TEST
const SAlphaBlendCase g_AlphaBlendCases[] =
{
    {
        D3DBLEND_SRCALPHA,
        D3DBLEND_INVSRCALPHA,
        D3DPBLENDCAPS_SRCALPHA,
        D3DPBLENDCAPS_INVSRCALPHA,	
    },
    {
        D3DBLEND_BLENDFACTOR,
        D3DBLEND_SRCALPHASAT,
        D3DPBLENDCAPS_BLENDFACTOR,
        D3DPBLENDCAPS_SRCALPHASAT,	
    },
};
#else
const SAlphaBlendCase g_AlphaBlendCases[] =
{

    {
        D3DBLEND_ONE,
        D3DBLEND_ZERO,
        D3DPBLENDCAPS_ONE,
        D3DPBLENDCAPS_ZERO,
    },
    {
        D3DBLEND_SRCCOLOR,
        D3DBLEND_INVSRCCOLOR,
        D3DPBLENDCAPS_SRCCOLOR,
        D3DPBLENDCAPS_INVSRCCOLOR,
    },
    {
        D3DBLEND_SRCALPHA,
        D3DBLEND_INVSRCALPHA,
        D3DPBLENDCAPS_SRCALPHA,
        D3DPBLENDCAPS_INVSRCALPHA,
    },
    {
        D3DBLEND_BLENDFACTOR,
        D3DBLEND_SRCALPHASAT,
        D3DPBLENDCAPS_BLENDFACTOR,
        D3DPBLENDCAPS_SRCALPHASAT,
    },
    {
        D3DBLEND_BOTHSRCALPHA,
        D3DBLEND_ZERO,
        D3DPBLENDCAPS_BOTHSRCALPHA,
        D3DPBLENDCAPS_ONE,
    },
    {
        D3DBLEND_BOTHINVSRCALPHA,
        D3DBLEND_ONE,
        D3DPBLENDCAPS_BOTHINVSRCALPHA,
        D3DPBLENDCAPS_ZERO,
    },

};
#endif


#define SZBLENDOP_ARRAY_SIZE 16   //D3DBLEND_INVBLENDFACTOR = 15 is the highest index

const TCHAR* g_szBlendOps[SZBLENDOP_ARRAY_SIZE];

#define ADD_OP(x) { g_szBlendOps[x] = _T(#x) + 9; } //9 is length of "D3DBLEND_"

//------------------------------------------------------------------------------

bool CTestVariableImpl<SAlphaBlendCase>::InitializeResources(CD3DTest* pTest)
{
	//initialize array with names of blendops
	for (int i = 0; i < SZBLENDOP_ARRAY_SIZE; i++)
		g_szBlendOps[i] = "UNKNOWN";

	ADD_OP(D3DBLEND_ZERO);
	ADD_OP(D3DBLEND_ONE);
	ADD_OP(D3DBLEND_SRCCOLOR);
	ADD_OP(D3DBLEND_INVSRCCOLOR);
	ADD_OP(D3DBLEND_SRCALPHA);
	ADD_OP(D3DBLEND_INVSRCALPHA);
	ADD_OP(D3DBLEND_DESTALPHA);
	ADD_OP(D3DBLEND_INVDESTALPHA);
	ADD_OP(D3DBLEND_DESTCOLOR);
	ADD_OP(D3DBLEND_INVDESTCOLOR);
	ADD_OP(D3DBLEND_SRCALPHASAT);
	ADD_OP(D3DBLEND_BOTHSRCALPHA);
	ADD_OP(D3DBLEND_BOTHINVSRCALPHA);
	ADD_OP(D3DBLEND_BLENDFACTOR);
	ADD_OP(D3DBLEND_INVBLENDFACTOR);

	return true;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SAlphaBlendCase>::CleanupResources()
{
}

//------------------------------------------------------------------------------

UINT CTestVariableImpl<SAlphaBlendCase>::GetUnfilteredDomainSize() const
{
	return (sizeof(g_AlphaBlendCases) / sizeof(g_AlphaBlendCases[0]));
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SAlphaBlendCase>::GetVariation(SAlphaBlendCase* pOut, const UINT nIndex) const
{
	memcpy(pOut, &(g_AlphaBlendCases[nIndex]), sizeof(SAlphaBlendCase));
}

//------------------------------------------------------------------------------

bool CTestVariableImpl<SAlphaBlendCase>::CheckTestRequiredCaps(const UINT uPIndex, const UINT uDIndex, CD3DTest* pTest)
{
	DEVICEDESC* DeviceCaps = pTest->m_pSrcDevice->GetCaps();

	if(g_AlphaBlendCases[uDIndex].dwDestRequiredCaps != (DeviceCaps->dwDestBlendCaps & g_AlphaBlendCases[uDIndex].dwDestRequiredCaps) ||
	   g_AlphaBlendCases[uDIndex].dwSrcRequiredCaps  != (DeviceCaps->dwSrcBlendCaps  & g_AlphaBlendCases[uDIndex].dwSrcRequiredCaps )  )
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SAlphaBlendCase>::ExpendTestStateName(TCHAR *testName)
{
	TCHAR tempName[256];

	strcpy(tempName, _T("AphaBlend "));

	_stprintf(tempName, _T("%ssrc %s "), tempName, g_szBlendOps[m_CurrentState.SrcBlend]);
	_stprintf(tempName, _T("%sdest %s, "), tempName, g_szBlendOps[m_CurrentState.DestBlend]);
	
	strncat(testName, tempName, NAME_SIZE-strlen(testName));
}

//------------------------------------------------------------------------------

bool CTestVariableImpl<SAlphaBlendCase>::PutTestIntoState(CD3DTest* pTest)
{
	bool bResultVal = true;

	bResultVal &= pTest->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	//we don't want to duplicate the default value to avoid warning messages
	if ( D3DBLEND_ONE != m_CurrentState.SrcBlend )
		bResultVal &= pTest->SetRenderState(D3DRS_SRCBLEND,  m_CurrentState.SrcBlend );
	if ( D3DBLEND_ZERO != m_CurrentState.DestBlend )
		bResultVal &= pTest->SetRenderState(D3DRS_DESTBLEND, m_CurrentState.DestBlend);

	DWORD iMaxStageIndex = -1;
	UINT uStageIndex;
		
	for (UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[g_uTexCaseIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[g_uTexCaseIndex].TexIndices[i]] )
			continue;

		uStageIndex = g_TextureCases[g_uTexCaseIndex].StageIndices[i];
		iMaxStageIndex = uStageIndex;
        		
		pTest->SetTextureStageState(uStageIndex, D3DTSS_ALPHAOP, (DWORD)D3DTOP_MODULATE);
	}

	//we don't want to duplicate the default value to avoid warning messages
	//this state was already set in CTestVariableImpl<STextureCase>::PutTestIntoState()
	if ( iMaxStageIndex < 7 )
		pTest->SetTextureStageState(iMaxStageIndex+1, D3DTSS_ALPHAOP, (DWORD)D3DTOP_DISABLE);
	return bResultVal;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SAlphaBlendCase>::UpdateTestStatus(CD3DTest* pTest)
{
	TCHAR szAlpha[120];

	szAlpha[0] = '\0';

	_stprintf(szAlpha, _T("%s$ysrc %s, "), szAlpha, g_szBlendOps[m_CurrentState.SrcBlend]);
	_stprintf(szAlpha, _T("%sdest %s"), szAlpha, g_szBlendOps[m_CurrentState.DestBlend]);
		
	pTest->WriteStatus(_T("$gAlphaBlend"), _T(szAlpha));

	//disable alpha blending
	pTest->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//we don't want to duplicate the default value to avoid warning messages
	if ( D3DBLEND_ONE != m_CurrentState.SrcBlend )
		pTest->SetRenderState(D3DRS_SRCBLEND,  (DWORD)D3DBLEND_ONE );
	if ( D3DBLEND_ZERO != m_CurrentState.DestBlend )
		pTest->SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_ZERO);

	int iMaxStageIndex = -1;
	UINT uStageIndex;

	for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[g_uTexCaseIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[g_uTexCaseIndex].TexIndices[i]] )
			continue;

		uStageIndex = g_TextureCases[g_uTexCaseIndex].StageIndices[i];
		iMaxStageIndex = uStageIndex;
		
		pTest->SetTextureStageState(uStageIndex, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);
	}

	//we don't want to duplicate the default value to avoid warning messages
	//this state will be set in CTestVariableImpl<STextureCase>::UpdateTestStatus()
	if ( iMaxStageIndex < 7 )
		pTest->SetTextureStageState(iMaxStageIndex + 1, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);
}


