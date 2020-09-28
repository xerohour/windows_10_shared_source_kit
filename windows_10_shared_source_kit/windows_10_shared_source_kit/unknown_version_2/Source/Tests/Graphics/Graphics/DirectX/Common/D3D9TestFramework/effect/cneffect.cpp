// ----------------------------------------------------------------------------
// CnEffect
// ----------------------------------------------------------------------------

//TODO :: WARNING :: BUGBUG (mahaigh @ 10/21/2005) There are a lot of functions 
//in CnEffect that use a 'D3DXHANDLE'.  These handles are unique to each particular
//effect and cannot be used across all effects in the CnEffect.  If this wrapper
//were to do this properly, it would need to take and return a 'D3DXnHANDLE' in these
//cases.  As of right now however I don't think there are any tests that use any
//of the handle based functions, and since this is the legacy framework, there
//shouldn't be any in the future.


#include "CEffect.h"

CnEffect::CnEffect()
{
	m_pEffects = NULL;
	m_uDevices = 0;
}

// ----------------------------------------------------------------------------

CnEffect::~CnEffect()
{
	if (NULL != m_pEffects)
	{
		for (UINT i = 0; i < m_uDevices; i++)
		{
			RELEASE (m_pEffects[i]);
		}
		SAFEDELETEA (m_pEffects);
	}
}

// ----------------------------------------------------------------------------

bool CnEffect::SkipDevice (UINT uDevice, DWORD dwFlags)
{
	DWORD dwMask = 1;

	for (int i = 0; i < uDevice; i++)
		dwMask <<= 1;

	return (dwMask & dwFlags) == 0;
}

// ----------------------------------------------------------------------------

bool CnEffect::Init (UINT uDevices)
{
	m_uDevices = uDevices;
	m_pEffects = new CEffect*[m_uDevices];

	if (NULL == m_pEffects)
		return false;

	memset(m_pEffects, 0, sizeof(CEffect*) * m_uDevices);
	return true;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetEffect (CEffect* pEffect, UINT n)
{
	//check to make sure effects has been initialized
	if (NULL == m_pEffects)
		return false;

	//check that n is within range
	if (n >= m_uDevices)
		return false;

	//old effect should be released before new one is set
	if (m_pEffects[n] != NULL)
		return false;

	//set new effect
	m_pEffects[n] = pEffect;

	return true;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetEffect (CEffect** ppEffect, UINT n)
{
	if (NULL == m_pEffects || n >= m_uDevices || NULL == ppEffect)
		return false;

	m_pEffects[n]->AddRef();
	*ppEffect = m_pEffects[n];

	return true;
}

// ----------------------------------------------------------------------------

bool CnEffect::CloneEffect (UINT sourceEffect, CDevice* pTargetDevice, CEffect** ppResultEffect)
{
	if (sourceEffect >= m_uDevices)
		return false;

	if (NULL == pTargetDevice || NULL == ppResultEffect || NULL == m_pEffects[sourceEffect])
		return false;

	return SUCCEEDED (m_pEffects[sourceEffect]->CloneEffect (pTargetDevice, ppResultEffect));
}

// ----------------------------------------------------------------------------

bool CnEffect::Begin (UINT *pPasses, DWORD Flags, DWORD dwFlags)
{
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for(UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if (FAILED (m_pEffects[i]->Begin (pPasses, Flags)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::Begin() - Begin failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::BeginPass (UINT Pass, DWORD dwFlags)
{
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->BeginPass (Pass)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::BeginPass() - Pass failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::CommitChanges (DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->CommitChanges()))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::CommitChanges() - End failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::EndPass (DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->EndPass()))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::EndPass() - End failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::End (DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->End()))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::End() - End failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetBool (D3DXHANDLE hParameter, BOOL b, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetBool (hParameter, b)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetBool() - SetBool failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetBoolArray (D3DXHANDLE hParameter, const BOOL* pB, UINT Count, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetBoolArray (hParameter, pB, Count)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetBoolArray() - SetBoolArray failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetFloat (D3DXHANDLE hParameter, FLOAT f, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetFloat (hParameter, f)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetFloat() - SetFloat failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetFloatArray (D3DXHANDLE hParameter, const FLOAT* pf, UINT Count, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetFloatArray (hParameter, pf, Count)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetFloatArray() - SetFloatArray failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetInt (D3DXHANDLE hParameter, int n, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetInt (hParameter, n)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetInt() - SetInt failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetIntArray (D3DXHANDLE hParameter, const int* pn, UINT Count, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetIntArray (hParameter, pn, Count)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetIntArray() - SetIntArray failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetMatrix (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetMatrix (hParameter, pMatrix)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetMatrix() - SetMatrix failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetMatrixArray (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, UINT Count, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetMatrixArray (hParameter, pMatrix, Count)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetMatrixArray() - SetMatrixArray failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetMatrixPointerArray (D3DXHANDLE hParameter, const D3DXMATRIX** ppMatrix, UINT Count, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetMatrixPointerArray (hParameter, ppMatrix, Count)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetMatrixPointerArray() - SetMatrixPointerArray failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetMatrixTranspose (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetMatrixTranspose (hParameter, pMatrix)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetMatrixTranspose() - SetMatrixTranspose failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetMatrixTransposeArray (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, UINT Count, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetMatrixTransposeArray (hParameter, pMatrix, Count)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetMatrixTransposeArray() - SetMatrixTransposeArray failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetMatrixTransposePointerArray (D3DXHANDLE hParameter, const D3DXMATRIX** ppMatrix, UINT Count, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetMatrixTransposePointerArray (hParameter, ppMatrix, Count)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetMatrixTransposePointerArray() - SetMatrixTransposePointerArray failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetPixelShader (D3DXHANDLE hParameter, CnPixelShader* pPShader, DWORD dwFlags)
{	
	bool bRes = true;
	bool bTemp;
	CPShader* pShade = NULL;

	//make sure pointers arn't null
	if (NULL == m_pEffects || NULL == pPShader)
		return false;

	//loop through the devices
	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		//grab the shader
		bTemp = pPShader->GetPShader (&pShade, i);

		if (!bTemp || NULL == pShade)
		{
			bRes = false;
			DPF(1, _T("CnEffect::SetPixelShader() - Failed to get CPShader on Effect #%d.\n"), i);
		}
		else 
		{ 
			//we got a valid shader, so set it into the corresponding effect
			if (FAILED (m_pEffects[i]->SetPixelShader (hParameter, pShade)))
			{
				bRes = false;
				DPF(1, _T("CnEffect::SetPixelShader() - SetPixelShader failed on Effect #%d.\n"), i);
			}
			RELEASE(pShade);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------
//change this later if a wrapper for LPD3DXEFFECTSTATEMANAGER is added
/*
bool CnEffect::SetStateManager (LPD3DXEFFECTSTATEMANAGER pManager, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetStateManager (pManager)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetStateManager() - SetStateManager failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}
*/
// ----------------------------------------------------------------------------

bool CnEffect::SetString (D3DXHANDLE hParameter, const LPCSTR pString, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetString (hParameter, pString)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetString() - SetString failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetTechnique (D3DXHANDLE hTechnique, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetTechnique (hTechnique)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetTechnique() - SetTechnique failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetTechniqueByIndex (UINT index, DWORD dwFlags)
{
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if (FAILED (m_pEffects[i]->SetTechnique (m_pEffects[i]->GetTechnique (index))))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetTechniqueByIndex() - SetTechnique failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetTechniqueByName (LPCSTR pName, DWORD dwFlags)
{
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if (FAILED (m_pEffects[i]->SetTechnique (m_pEffects[i]->GetTechniqueByName (pName))))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetTechniqueByName() - SetTechnique failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetTexture (D3DXHANDLE hParameter, CnTexture* pTexture, DWORD dwFlags)
{	
	bool bRes = true;
	bool bTemp;
	CTexture* pTex = NULL;

	if (NULL == m_pEffects || NULL == pTexture)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		bTemp = pTexture->GetTexture (&pTex, i);

		if (!bTemp || NULL == pTex)
		{
			bRes = false;
			DPF(1, _T("CnEffect::SetTexture() - Failed to get CTexture on Effect #%d.\n"), i);
		}
		else 
		{ 
			if (FAILED (m_pEffects[i]->SetTexture (hParameter, pTex)))
			{
				bRes = false;
				DPF(1, _T("CnEffect::SetTexture() - SetTexture failed on Effect #%d.\n"), i);
			}
			RELEASE(pTex);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetCubeTexture (D3DXHANDLE hParameter, CnCubeTexture* pCubeTexture, DWORD dwFlags)
{	
	bool bRes = true;
	bool bTemp;
	CCubeTexture* pTex = NULL;

	if (NULL == m_pEffects || NULL == pCubeTexture)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		bTemp = pCubeTexture->GetCubeTexture (&pTex, i);

		if (!bTemp || NULL == pTex)
		{
			bRes = false;
			DPF(1, _T("CnEffect::SetCubeTexture() - Failed to get CCubeTexture on Effect #%d.\n"), i);
		}
		else 
		{ 
			if (FAILED (m_pEffects[i]->SetCubeTexture (hParameter, pTex)))
			{
				bRes = false;
				DPF(1, _T("CnEffect::SetCubeTexture() - SetCubeTexture failed on Effect #%d.\n"), i);
			}
			RELEASE(pTex);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetVolTexture (D3DXHANDLE hParameter, CnVolTexture* pVolTexture, DWORD dwFlags)
{	
	bool bRes = true;
	bool bTemp;
	CVolTexture* pTex = NULL;

	if (NULL == m_pEffects || NULL == pVolTexture)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		bTemp = pVolTexture->GetVolumeTexture (&pTex, i);

		if (!bTemp || NULL == pTex)
		{
			bRes = false;
			DPF(1, _T("CnEffect::SetVolTexture() - Failed to get CVolTexture on Effect #%d.\n"), i);
		}
		else 
		{ 
			if (FAILED (m_pEffects[i]->SetVolTexture (hParameter, pTex)))
			{
				bRes = false;
				DPF(1, _T("CnEffect::SetVolTexture() - SetVolTexture failed on Effect #%d.\n"), i);
			}
			RELEASE(pTex);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetValue (D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetValue (hParameter, pData, Bytes)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetValue() - SetValue failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetVector (D3DXHANDLE hParameter, const D3DXVECTOR4* pVector, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetVector (hParameter, pVector)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetVector() - SetVector failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetVectorArray (D3DXHANDLE hParameter, const D3DXVECTOR4* pVector, UINT Count, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetVectorArray (hParameter, pVector, Count)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetVectorArray() - SetVectorArray failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetVertexShader (D3DXHANDLE hParameter, CnVertexShader* pVertexShader, DWORD dwFlags)
{	
	bool bRes = true;
	bool bTemp;
	CVertexShader* pShade = NULL;

	//make sure pointers arn't null
	if (NULL == m_pEffects || NULL == pVertexShader)
		return false;

	//loop through the devices
	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		//grab the shader
		bTemp = pVertexShader->GetVShader (&pShade, i);

		if (!bTemp || NULL == pShade)
		{
			bRes = false;
			DPF(1, _T("CnEffect::SetVertexShader() - Failed to get CVertexShader on Effect #%d.\n"), i);
		}
		else 
		{ 
			//we got a valid shader, so set it into the corresponding effect
			if (FAILED (m_pEffects[i]->SetVertexShader (hParameter, pShade)))
			{
				bRes = false;
				DPF(1, _T("CnEffect::SetVertexShader() - SetVertexShader failed on Effect #%d.\n"), i);
			}
			RELEASE(pShade);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetArrayRange (D3DXHANDLE hParameter, UINT Start, UINT End, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetArrayRange (hParameter, Start, End)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetArrayRange() - SetVectorArray failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

D3DXHANDLE CnEffect::GetAnnotation (D3DXHANDLE hObject, UINT Index, DWORD dwFlags)
{
	if (NULL == m_pEffects)
		return NULL;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return m_pEffects[i]->GetAnnotation (hObject, Index);
	}

	return NULL;
}

// ----------------------------------------------------------------------------

D3DXHANDLE CnEffect::GetAnnotationByName (D3DXHANDLE hObject, LPCSTR pName, DWORD dwFlags)
{
	if (NULL == m_pEffects)
		return NULL;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return m_pEffects[i]->GetAnnotationByName (hObject, pName);
	}

	return NULL;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetBool (D3DXHANDLE hParameter, BOOL* pb, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetBool (hParameter, pb));
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetBoolArray (D3DXHANDLE hParameter, BOOL* pB, UINT Count, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetBoolArray (hParameter, pB, Count));
	}
	return false;
}

// ----------------------------------------------------------------------------

D3DXHANDLE CnEffect::GetCurrentTechnique (DWORD dwFlags)
{
	if (NULL == m_pEffects)
		return NULL;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return m_pEffects[i]->GetCurrentTechnique();
	}

	return NULL;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetDesc (D3DXEFFECT_DESC* pDesc, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetDesc (pDesc));
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetDevice (CDevice** ppDevice, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetDevice (ppDevice));
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetFloat (D3DXHANDLE hParameter, FLOAT* pf, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetFloat (hParameter, pf));
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetFloatArray (D3DXHANDLE hParameter, FLOAT* pf, UINT Count, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetFloatArray (hParameter, pf, Count));
	}
	return false;
}

// ----------------------------------------------------------------------------

D3DXHANDLE CnEffect::GetFunction (UINT Index, DWORD dwFlags)
{
	if (NULL == m_pEffects)
		return NULL;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return m_pEffects[i]->GetFunction (Index);
	}

	return NULL;
}

// ----------------------------------------------------------------------------

D3DXHANDLE CnEffect::GetFunctionByName (LPCSTR pName, DWORD dwFlags)
{
	if (NULL == m_pEffects)
		return NULL;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return m_pEffects[i]->GetFunctionByName (pName);
	}

	return NULL;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetFunctionDesc (D3DXHANDLE hFunction, D3DXFUNCTION_DESC* pDesc, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetFunctionDesc (hFunction, pDesc));
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetInt (D3DXHANDLE hParameter, int* pn, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetInt (hParameter, pn));
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetIntArray (D3DXHANDLE hParameter, int* pn, UINT Count, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetIntArray (hParameter, pn, Count));
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetMatrix (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetMatrix (hParameter, pMatrix));
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetMatrixArray (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetMatrixArray (hParameter, pMatrix, Count));
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetMatrixPointerArray (D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetMatrixPointerArray (hParameter, ppMatrix, Count));
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetMatrixTranspose (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetMatrixTranspose (hParameter, pMatrix));
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetMatrixTransposeArray (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetMatrixTransposeArray (hParameter, pMatrix, Count));
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetMatrixTransposePointerArray (D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetMatrixTransposePointerArray (hParameter, ppMatrix, Count));
	}
	return false;
}

// ----------------------------------------------------------------------------

D3DXHANDLE CnEffect::GetParameter (D3DXHANDLE hParent, UINT Index, DWORD dwFlags)
{
	if (NULL == m_pEffects)
		return NULL;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return m_pEffects[i]->GetParameter (hParent, Index);
	}

	return NULL;
}

// ----------------------------------------------------------------------------

D3DXHANDLE CnEffect::GetParameterByName (D3DXHANDLE hParent, LPCSTR pName, DWORD dwFlags)
{
	if (NULL == m_pEffects)
		return NULL;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return m_pEffects[i]->GetParameterByName (hParent, pName);
	}

	return NULL;
}

// ----------------------------------------------------------------------------

D3DXHANDLE CnEffect::GetParameterBySemantic (D3DXHANDLE hParent, LPCSTR pSemantic, DWORD dwFlags)
{
	if (NULL == m_pEffects)
		return NULL;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return m_pEffects[i]->GetParameterBySemantic (hParent, pSemantic);
	}

	return NULL;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetParameterDesc (D3DXHANDLE hParameter, D3DXPARAMETER_DESC* pDesc, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetParameterDesc (hParameter, pDesc));
	}
	return false;
}

// ----------------------------------------------------------------------------

D3DXHANDLE CnEffect::GetParameterElement (D3DXHANDLE hParent, UINT ElementIndex, DWORD dwFlags)
{
	if (NULL == m_pEffects)
		return NULL;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return m_pEffects[i]->GetParameterElement (hParent, ElementIndex);
	}

	return NULL;
}

// ----------------------------------------------------------------------------

D3DXHANDLE CnEffect::GetPass (D3DXHANDLE hTechnique, UINT Index, DWORD dwFlags)
{
	if (NULL == m_pEffects)
		return NULL;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return m_pEffects[i]->GetPass (hTechnique, Index);
	}

	return NULL;
}

// ----------------------------------------------------------------------------

D3DXHANDLE CnEffect::GetPassByName (D3DXHANDLE hTechnique, LPCSTR pName, DWORD dwFlags)
{
	if (NULL == m_pEffects)
		return NULL;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return m_pEffects[i]->GetPassByName (hTechnique, pName);
	}

	return NULL;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetPassDesc (D3DXHANDLE hPass, D3DXPASS_DESC* pDesc, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetPassDesc (hPass, pDesc));
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetPixelShader (D3DXHANDLE hParameter, CnPixelShader** ppPShader, DWORD dwFlags)
{	
	bool bRes = true;
	HRESULT hr;
	CPShader* pShade = NULL;

	//make sure pointers are valid
	if (NULL == m_pEffects || NULL == ppPShader)
		return false;

	//go through the devices
	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		//grab the pixel shader from each effect
		hr = m_pEffects[i]->GetPixelShader (hParameter, &pShade);

		if (FAILED (hr) || NULL == pShade)
		{
			bRes = false;
			DPF(1, _T("CnEffect::GetPixelShader() - Failed to get CPShader from Effect #%d.\n"), i);
		}
		else 
		{ 
			//got the shader, now set it
			if (!(*ppPShader)->SetPShader (pShade, i))
			{
				bRes = false;
				DPF(1, _T("CnEffect::GetPixelShader() - SetPShader failed on Effect #%d.\n"), i);
			}
			RELEASE(pShade);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetPool (LPD3DXEFFECTPOOL* ppPool, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetPool (ppPool));
	}
	return false;
}

// ----------------------------------------------------------------------------
//change this later if a wrapper for LPD3DXEFFECTSTATEMANAGER is added
/*
bool CnEffect::GetStateManager (LPD3DXEFFECTSTATEMANAGER* ppManager, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetStateManager (ppManager));
	}
	return false;
}
*/
// ----------------------------------------------------------------------------

bool CnEffect::GetString (D3DXHANDLE hParameter, LPCSTR* ppString, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetString (hParameter, ppString));
	}
	return false;
}

// ----------------------------------------------------------------------------

D3DXHANDLE CnEffect::GetTechnique (UINT Index, DWORD dwFlags)
{
	if (NULL == m_pEffects)
		return NULL;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return m_pEffects[i]->GetTechnique (Index);
	}

	return NULL;
}

// ----------------------------------------------------------------------------

D3DXHANDLE CnEffect::GetTechniqueByName (LPCSTR pName, DWORD dwFlags)
{
	if (NULL == m_pEffects)
		return NULL;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return m_pEffects[i]->GetTechniqueByName (pName);
	}

	return NULL;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetTechniqueDesc (D3DXHANDLE hTechnique, D3DXTECHNIQUE_DESC* pDesc, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetTechniqueDesc (hTechnique, pDesc));
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetTexture (D3DXHANDLE hParameter, CnTexture** ppTexture, DWORD dwFlags)
{	
	bool bRes = true;
	HRESULT hr;
	CTexture* pTex = NULL;

	//make sure pointers are valid
	if (NULL == m_pEffects || NULL == ppTexture)
		return false;

	//go through the devices
	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		//grab the texture from each effect
		hr = m_pEffects[i]->GetTexture (hParameter, &pTex);

		if (FAILED (hr) || NULL == pTex)
		{
			bRes = false;
			DPF(1, _T("CnEffect::GetTexture() - Failed to get CTexture from Effect #%d.\n"), i);
		}
		else 
		{ 
			//got the texture, now set it
			if (!(*ppTexture)->SetTexture (pTex, i))
			{
				bRes = false;
				DPF(1, _T("CnEffect::GetTexture() - SetTexture failed on Effect #%d.\n"), i);
			}
			RELEASE(pTex);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetCubeTexture (D3DXHANDLE hParameter, CnCubeTexture** ppCubeTexture, DWORD dwFlags)
{	
	bool bRes = true;
	HRESULT hr;
	CCubeTexture* pTex = NULL;

	//make sure pointers are valid
	if (NULL == m_pEffects || NULL == ppCubeTexture)
		return false;

	//go through the devices
	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		//grab the cube texture from each effect
		hr = m_pEffects[i]->GetCubeTexture (hParameter, &pTex);

		if (FAILED (hr) || NULL == pTex)
		{
			bRes = false;
			DPF(1, _T("CnEffect::GetCubeTexture() - Failed to get CCubeTexture from Effect #%d.\n"), i);
		}
		else 
		{ 
			//got the texture, now set it
			if (!(*ppCubeTexture)->SetCubeTexture (pTex, i))
			{
				bRes = false;
				DPF(1, _T("CnEffect::GetCubeTexture() - SetCubeTexture failed on Effect #%d.\n"), i);
			}
			RELEASE(pTex);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetVolTexture (D3DXHANDLE hParameter, CnVolTexture** ppVolTexture, DWORD dwFlags)
{	
	bool bRes = true;
	HRESULT hr;
	CVolTexture* pTex = NULL;

	//make sure pointers are valid
	if (NULL == m_pEffects || NULL == ppVolTexture)
		return false;

	//go through the devices
	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		//grab the volume texture from each effect
		hr = m_pEffects[i]->GetVolTexture (hParameter, &pTex);

		if (FAILED (hr) || NULL == pTex)
		{
			bRes = false;
			DPF(1, _T("CnEffect::GetVolTexture() - Failed to get CVolTexture from Effect #%d.\n"), i);
		}
		else 
		{ 
			//got the texture, now set it
			if (!(*ppVolTexture)->SetVolumeTexture (pTex, i))
			{
				bRes = false;
				DPF(1, _T("CnEffect::GetVolTexture() - SetVolTexture failed on Effect #%d.\n"), i);
			}
			RELEASE(pTex);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetValue (D3DXHANDLE hParameter, LPVOID pData, UINT Bytes, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetValue (hParameter, pData, Bytes));
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetVector (D3DXHANDLE hParameter, D3DXVECTOR4* pVector, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetVector (hParameter, pVector));
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetVectorArray (D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count, DWORD dwFlags)
{	
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return SUCCEEDED (m_pEffects[i]->GetVectorArray (hParameter, pVector, Count));
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CnEffect::GetVertexShader (D3DXHANDLE hParameter, CnVertexShader** ppVShader, DWORD dwFlags)
{	
	bool bRes = true;
	HRESULT hr;
	CVertexShader* pShade = NULL;

	//make sure pointers are valid
	if (NULL == m_pEffects || NULL == ppVShader)
		return false;

	//go through the devices
	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		//grab the vertex shader from each effect
		hr = m_pEffects[i]->GetVertexShader (hParameter, &pShade);

		if (FAILED (hr) || NULL == pShade)
		{
			bRes = false;
			DPF(1, _T("CnEffect::GetVertexShader() - Failed to get CVertexShader from Effect #%d.\n"), i);
		}
		else 
		{ 
			//got the shader, now set it
			if (!(*ppVShader)->SetVShader (pShade, i))
			{
				bRes = false;
				DPF(1, _T("CnEffect::GetVertexShader() - SetVShader failed on Effect #%d.\n"), i);
			}
			RELEASE(pShade);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::ApplyParameterBlock (D3DXHANDLE hParameterBlock, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		//apply the parameter block to each effect
		if(FAILED (m_pEffects[i]->ApplyParameterBlock (hParameterBlock)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::ApplyParameterBlock() - ApplyParameterBlock failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::BeginParameterBlock (DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		//begin the parameter block on each effect
		if(FAILED (m_pEffects[i]->BeginParameterBlock ()))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::BeginParameterBlock() - BeginParameterBlock failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

D3DXHANDLE CnEffect::EndParameterBlock (DWORD dwFlags)
{	
	D3DXHANDLE hRetVal = NULL;

	if (NULL == m_pEffects)
		return NULL;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		//end the parameter block on each effect but only return one handle
		hRetVal = m_pEffects[i]->EndParameterBlock();
	}
	return hRetVal;
}


// ----------------------------------------------------------------------------

bool CnEffect::DeleteParameterBlock (D3DXHANDLE hParameterBlock, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return NULL;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		//delete the parameter block on each effect
		if(FAILED (m_pEffects[i]->DeleteParameterBlock (hParameterBlock)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::DeleteParameterBlock() - DeleteParameterBlock failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::SetRawValue (D3DXHANDLE hParameter, LPCVOID pData, UINT ByteOffset, UINT Bytes, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return NULL;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		if(FAILED (m_pEffects[i]->SetRawValue (hParameter, pData, ByteOffset, Bytes)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::SetRawValue() - SetRawValue failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::FindNextValidTechnique (D3DXHANDLE hTechnique, D3DXHANDLE* pTechnique, DWORD dwFlags)
{
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		//return with the technique of the first valid effect
		return SUCCEEDED (m_pEffects[i]->FindNextValidTechnique (hTechnique, pTechnique));
	}
	return false;
}

// ----------------------------------------------------------------------------

BOOL CnEffect::IsParameterUsed (D3DXHANDLE hParameter, D3DXHANDLE hTechnique, DWORD dwFlags)
{
	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		return m_pEffects[i]->IsParameterUsed (hParameter, hTechnique);
	}
	return false;
}

// ----------------------------------------------------------------------------

bool CnEffect::OnLostDevice (DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		//call lost device on each effect
		if(FAILED (m_pEffects[i]->OnLostDevice ()))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::OnLostDevice() - OnLostDevice failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::OnResetDevice (DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		//begin the parameter block on each effect
		if(FAILED (m_pEffects[i]->OnResetDevice ()))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::OnResetDevice() - OnResetDevice failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::ValidateTechnique (D3DXHANDLE hTechnique, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		//validate the technique on each effect
		if(FAILED (m_pEffects[i]->ValidateTechnique (hTechnique)))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::ValidateTechnique() - ValidateTechnique failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}

// ----------------------------------------------------------------------------

bool CnEffect::ValidateTechniqueByIndex (UINT index, DWORD dwFlags)
{	
	bool bRes = true;

	if (NULL == m_pEffects)
		return false;

	for (UINT i = 0; i < m_uDevices; i++)
	{
		if (SkipDevice (i, dwFlags))
			continue;

		if (NULL == m_pEffects[i])
			continue;

		//validate the technique on each effect
		if(FAILED (m_pEffects[i]->ValidateTechnique (m_pEffects[i]->GetTechnique(index))))
		{ 
			bRes = false;
			DPF(1, _T("CnEffect::ValidateTechniqueByIndex() - ValidateTechnique failed on Effect #%d.\n"), i);
		}
	}
	return bRes;
}