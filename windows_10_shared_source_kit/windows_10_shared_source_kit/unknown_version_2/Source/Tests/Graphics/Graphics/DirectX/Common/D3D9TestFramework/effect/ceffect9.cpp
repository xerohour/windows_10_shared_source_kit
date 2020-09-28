

#include <d3d9.h>
#include "cd3dx.h"
#include "CEffect.h"

// --------------------------------------------------------------------------------------

bool CEffect::Init9 (LPVOID pEffect)
{
	if (NULL == pEffect)
	{
		DPF(1, _T("CEffect::Init9() - invalid parameters.\n"));
		return false;
	}

	m_pEffect9 = (LPD3DXEFFECT) pEffect;

	D3DXHANDLEVERSION(,ID3DXEffect, m_pEffect9, AddRef());

	return true;
}

// --------------------------------------------------------------------------------------

void CEffect::ReleaseD3D9()
{
	RELEASE (m_pEffect9);
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::Begin9 (UINT* pPasses, DWORD Flags)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, Begin(pPasses,Flags));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::BeginPass9 (UINT iPass)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	if(g_fD3DX >= 9.29f)
		return ((ID3DXEffect93*)m_pEffect9)->BeginPass (iPass);
	else if(g_fD3DX < 9.29f &&  g_fD3DX >= 9.19f)
		return ((ID3DXEffect92*)m_pEffect9)->BeginPass (iPass);
	else if (g_fD3DX < 9.19f && g_fD3DX >= 9.09f)
		return ((ID3DXEffect91*)m_pEffect9)->Pass (iPass);
	else
		return ((ID3DXEffect90 *)m_pEffect9)->Pass (iPass);
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::CommitChanges9 (void)
{
	if (NULL == m_pEffect9)
		return E_FAIL;
	
	if(g_fD3DX >= 9.29f)
		return ((ID3DXEffect93*)m_pEffect9)->CommitChanges();
	else if(g_fD3DX < 9.29f &&  g_fD3DX >= 9.19f)
		return ((ID3DXEffect92*)m_pEffect9)->CommitChanges();
	else
		return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::EndPass9 (void)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	if(g_fD3DX >= 9.29f)
		return ((ID3DXEffect93*)m_pEffect9)->EndPass();
	else if(g_fD3DX < 9.29f &&  g_fD3DX >= 9.19f)
		return ((ID3DXEffect92*)m_pEffect9)->EndPass();
	else
		return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::End9 (void)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, End());
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetBool9 (D3DXHANDLE hParameter, BOOL b)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetBool (hParameter, b));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetBoolArray9 (D3DXHANDLE hParameter, const BOOL* pB, UINT Count)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetBoolArray(hParameter, pB, Count));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetFloat9 (D3DXHANDLE hParameter, FLOAT f)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetFloat(hParameter, f));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetFloatArray9 (D3DXHANDLE hParameter, const FLOAT* pf, UINT Count)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetFloatArray (hParameter, pf, Count));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetInt9 (D3DXHANDLE hParameter, int n)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetInt (hParameter, n));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetIntArray9 (D3DXHANDLE hParameter, const int* pn, UINT Count)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetIntArray (hParameter, pn, Count));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetMatrix9 (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetMatrix (hParameter, pMatrix));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetMatrixArray9 (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, UINT Count)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetMatrixArray (hParameter, pMatrix, Count));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetMatrixPointerArray9 (D3DXHANDLE hParameter, const D3DXMATRIX** ppMatrix, UINT Count)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetMatrixPointerArray (hParameter, ppMatrix, Count));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetMatrixTranspose9 (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetMatrixTranspose (hParameter, pMatrix));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetMatrixTransposeArray9 (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, UINT Count)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetMatrixTransposeArray (hParameter, pMatrix, Count));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetMatrixTransposePointerArray9 (D3DXHANDLE hParameter, const D3DXMATRIX** ppMatrix, UINT Count)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetMatrixTransposePointerArray (hParameter, ppMatrix, Count));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetPixelShader9 (D3DXHANDLE hParameter, CPShader* pPShader)
{
	if (NULL == m_pEffect9 || NULL == pPShader)
		return E_FAIL;

	if(g_fD3DX >= 9.29f)
		return E_NOTIMPL;
	else
	{
		if(g_fD3DX < 9.29f && g_fD3DX >= 9.19f)
			return ((ID3DXEffect92*)m_pEffect9)->SetPixelShader(hParameter, pPShader->GetShader9());
		else if (g_fD3DX < 9.19f && g_fD3DX >= 9.09f)
			return ((ID3DXEffect91*)m_pEffect9)->SetPixelShader(hParameter, pPShader->GetShader9());
		else
			return ((ID3DXEffect90*)m_pEffect9)->SetPixelShader(hParameter, pPShader->GetShader9());
	}
}

// --------------------------------------------------------------------------------------
//change this later if a wrapper for LPD3DXEFFECTSTATEMANAGER is added
/*
HRESULT CEffect::SetStateManager9 (LPD3DXEFFECTSTATEMANAGER pManager)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	if (g_fD3DX >= 9.1f)
		return ((ID3DXEffect91*)m_pEffect9)->SetStateManager (pManager);
	else
		return m_pEffect9->SetStateManager (pManager);
}
*/
// --------------------------------------------------------------------------------------

HRESULT CEffect::SetString9 (D3DXHANDLE hParameter, const LPCSTR pString)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetString (hParameter, pString));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetTechnique9 (D3DXHANDLE hTechnique)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetTechnique (hTechnique));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetTexture9 (D3DXHANDLE hParameter, CTexture* pTexture)
{
	if (NULL == m_pEffect9 || NULL == pTexture)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetTexture (hParameter, (LPDIRECT3DTEXTURE9)pTexture->GetInternalPointer()));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetCubeTexture9 (D3DXHANDLE hParameter, CCubeTexture* pCubeTexture)
{
	if (NULL == m_pEffect9 || NULL == pCubeTexture)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetTexture (hParameter, (LPDIRECT3DCUBETEXTURE9)pCubeTexture->GetInternalPointer()));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetVolTexture9 (D3DXHANDLE hParameter, CVolTexture* pVolTexture)
{
	if (NULL == m_pEffect9 || NULL == pVolTexture)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetTexture (hParameter, (LPDIRECT3DVOLUMETEXTURE9)pVolTexture->GetInternalPointer()));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetValue9 (D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetValue (hParameter, pData, Bytes));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetVector9 (D3DXHANDLE hParameter, const D3DXVECTOR4* pVector)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetVector (hParameter, pVector));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetVectorArray9 (D3DXHANDLE hParameter, const D3DXVECTOR4* pVector, UINT Count)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, SetVectorArray (hParameter, pVector, Count));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetVertexShader9 (D3DXHANDLE hParameter, CVertexShader* pVertexShader)
{
	if (NULL == m_pEffect9 || NULL == pVertexShader)
		return E_FAIL;

	if(g_fD3DX >= 9.29f)
		return E_NOTIMPL;
	else
	{
		if(g_fD3DX < 9.29f && g_fD3DX >= 9.19f)
			return ((ID3DXEffect92*)m_pEffect9)->SetVertexShader(hParameter, pVertexShader->GetShader9());
		else if (g_fD3DX < 9.19f && g_fD3DX >= 9.09f)
			return ((ID3DXEffect91*)m_pEffect9)->SetVertexShader(hParameter, pVertexShader->GetShader9());
		else
			return ((ID3DXEffect90*)m_pEffect9)->SetVertexShader(hParameter, pVertexShader->GetShader9());
	}
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetArrayRange9 (D3DXHANDLE hParameter, UINT Start, UINT End)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	if(g_fD3DX >= 9.29f)
		return ((ID3DXEffect93*)m_pEffect9)->SetArrayRange(hParameter, Start, End);
	else if(g_fD3DX < 9.29f &&  g_fD3DX >= 9.19f)
		return ((ID3DXEffect92*)m_pEffect9)->SetArrayRange(hParameter, Start, End);
	else
		return S_OK;
}

// --------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetAnnotation9 (D3DXHANDLE hObject, UINT Index)
{
	if (NULL == m_pEffect9)
		return NULL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetAnnotation (hObject, Index));
}

// --------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetAnnotationByName9 (D3DXHANDLE hObject, LPCSTR pName)
{
	if (NULL == m_pEffect9)
		return NULL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetAnnotationByName (hObject, pName));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetBool9 (D3DXHANDLE hParameter, BOOL* pb)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetBool (hParameter, pb));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetBoolArray9 (D3DXHANDLE hParameter, BOOL* pB, UINT Count)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetBoolArray (hParameter, pB, Count));
}

// --------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetCurrentTechnique9 (void)
{
	if (NULL == m_pEffect9)
		return NULL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetCurrentTechnique());
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetDesc9 (D3DXEFFECT_DESC* pDesc)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetDesc (pDesc));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetDevice9 (CDevice** ppDevice)
{
	IDirect3DDevice9 *pDev = NULL;
	HRESULT hr;

    D3DXHANDLEVERSION(hr= ,ID3DXEffect, m_pEffect9, GetDevice (PassPointer (&pDev, ppDevice)));

	if (SUCCEEDED(hr))
	{
		if (pDev != m_pDevice->GetDevicePointer9())
		{
			ppDevice = NULL;
		}

		if (ppDevice)
		{
			*ppDevice = m_pDevice; 
			m_pDevice->AddRef();
		}
		RELEASE(pDev);
		return S_OK; 
	}
	else
	{
		ppDevice = NULL;
		RELEASE(pDev);
		return hr;
	}
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetFloat9 (D3DXHANDLE hParameter, FLOAT* pf)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetFloat (hParameter, pf));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetFloatArray9 (D3DXHANDLE hParameter, FLOAT* pf, UINT Count)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetFloatArray (hParameter, pf, Count));
}

// --------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetFunction9 (UINT Index)
{
	if (NULL == m_pEffect9)
		return NULL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetFunction (Index));
}

// --------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetFunctionByName9 (LPCSTR pName)
{
	if (NULL == m_pEffect9)
		return NULL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetFunctionByName (pName));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetFunctionDesc9 (D3DXHANDLE hFunction, D3DXFUNCTION_DESC* pDesc)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetFunctionDesc (hFunction, pDesc));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetInt9 (D3DXHANDLE hParameter, int* pn)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetInt (hParameter, pn));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetIntArray9 (D3DXHANDLE hParameter, int* pn, UINT Count)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetIntArray (hParameter, pn, Count));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetMatrix9 (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetMatrix (hParameter, pMatrix));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetMatrixArray9 (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetMatrixArray (hParameter, pMatrix, Count));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetMatrixPointerArray9 (D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetMatrixPointerArray (hParameter, ppMatrix, Count));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetMatrixTranspose9 (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetMatrixTranspose (hParameter, pMatrix));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetMatrixTransposeArray9 (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetMatrixTransposeArray (hParameter, pMatrix, Count));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetMatrixTransposePointerArray9 (D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetMatrixTransposePointerArray (hParameter, ppMatrix, Count));
}

// --------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetParameter9 (D3DXHANDLE hParent, UINT Index)
{
	if (NULL == m_pEffect9)
		return NULL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetParameter (hParent, Index));
}

// --------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetParameterByName9 (D3DXHANDLE hParent, LPCSTR pName)
{
	if (NULL == m_pEffect9)
		return NULL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetParameterByName (hParent, pName));
}

// --------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetParameterBySemantic9 (D3DXHANDLE hParent, LPCSTR pSemantic)
{
	if (NULL == m_pEffect9)
		return NULL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetParameterBySemantic (hParent, pSemantic));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetParameterDesc9 (D3DXHANDLE hParameter, D3DXPARAMETER_DESC* pDesc)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetParameterDesc (hParameter, pDesc));
}

// --------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetParameterElement9 (D3DXHANDLE hParent, UINT ElementIndex)
{
	if (NULL == m_pEffect9)
		return NULL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetParameterElement (hParent, ElementIndex));
}

// --------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetPass9 (D3DXHANDLE hTechnique, UINT Index)
{
	if (NULL == m_pEffect9)
		return NULL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetPass (hTechnique, Index));
}

// --------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetPassByName9 (D3DXHANDLE hTechnique, LPCSTR pName)
{
	if (NULL == m_pEffect9)
		return NULL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetPassByName (hTechnique, pName));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetPassDesc9 (D3DXHANDLE hPass, D3DXPASS_DESC* pDesc)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetPassDesc (hPass, pDesc));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetPixelShader9 (D3DXHANDLE hParameter, CPShader** ppPShader)
{
	if (NULL == m_pEffect9 || NULL == ppPShader)
		return E_FAIL;

	LPDIRECT3DPIXELSHADER9 pShade = NULL;
	HRESULT hr;

	//get the pixel shader from the effect
	D3DXHANDLEVERSION(hr= ,ID3DXEffect, m_pEffect9, GetPixelShader(hParameter, &pShade));

	if (SUCCEEDED(hr) && NULL != pShade)
	{
		//if we got the shader, set it into CPShader
		*ppPShader = new CPShader;

		if (NULL == *ppPShader)
			hr = E_FAIL;
		else if (!(*ppPShader)->Init (pShade, m_pDevice))
			hr = E_FAIL;
	}

	//free pointers and return
	RELEASE (pShade);
	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetPool9 (LPD3DXEFFECTPOOL* ppPool)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetPool (ppPool));
}

// --------------------------------------------------------------------------------------
//change this later if a wrapper for LPD3DXEFFECTSTATEMANAGER is added
/*
HRESULT CEffect::GetStateManager9 (LPD3DXEFFECTSTATEMANAGER* ppManager)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	if (g_fD3DX >= 9.1f)
		return ((ID3DXEffect91*)m_pEffect9)->GetStateManager (ppManager);
	else
		return m_pEffect9->GetStateManager (ppManager);
}
*/
// --------------------------------------------------------------------------------------

HRESULT CEffect::GetString9 (D3DXHANDLE hParameter, LPCSTR* ppString)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetString (hParameter, ppString));
}

// --------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetTechnique9 (UINT Index)
{
	if (NULL == m_pEffect9)
		return NULL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetTechnique (Index));
}

// --------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetTechniqueByName9 (LPCSTR pName)
{
	if (NULL == m_pEffect9)
		return NULL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetTechniqueByName (pName));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetTechniqueDesc9 (D3DXHANDLE hTechnique, D3DXTECHNIQUE_DESC* pDesc)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetTechniqueDesc (hTechnique, pDesc));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetTexture9 (D3DXHANDLE hParameter, CTexture** ppTexture)
{
	if (NULL == m_pEffect9 || NULL == ppTexture)
		return E_FAIL;

	LPDIRECT3DBASETEXTURE9 pTex = NULL;
	HRESULT hr;

	//get the texture from the effect
	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetTexture (hParameter, &pTex));

	if (SUCCEEDED(hr) && NULL != pTex)
	{
		//if we got the texture, set it into CTexture
		*ppTexture = new CTexture;

		if (NULL == *ppTexture)
			hr = E_FAIL;
		else if (!(*ppTexture)->Init (pTex, m_pDevice))
			hr = E_FAIL;
	}

	//free pointers and return
	RELEASE (pTex);
	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetCubeTexture9 (D3DXHANDLE hParameter, CCubeTexture** ppCubeTexture)
{
	if (NULL == m_pEffect9 || NULL == ppCubeTexture)
		return E_FAIL;

	LPDIRECT3DBASETEXTURE9 pTex = NULL;
	HRESULT hr;

	//get the texture from the effect
	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetTexture (hParameter, &pTex));

	if (SUCCEEDED(hr) && NULL != pTex)
	{
		//if we got the texture, set it into CCubeTexture
		*ppCubeTexture = new CCubeTexture;

		if (NULL == *ppCubeTexture)
			hr = E_FAIL;
		else if (!(*ppCubeTexture)->Init (pTex, m_pDevice))
			hr = E_FAIL;
	}

	//free pointers and return
	RELEASE (pTex);
	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetVolTexture9 (D3DXHANDLE hParameter, CVolTexture** ppVolTexture)
{
	if (NULL == m_pEffect9 || NULL == ppVolTexture)
		return E_FAIL;

	LPDIRECT3DBASETEXTURE9 pTex = NULL;
	HRESULT hr;

	//get the texture from the effect
	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetTexture (hParameter, &pTex));

	if (SUCCEEDED(hr) && NULL != pTex)
	{
		//if we got the texture, set it into CTexture
		*ppVolTexture = new CVolTexture;

		if (NULL == *ppVolTexture)
			hr = E_FAIL;
		else if (!(*ppVolTexture)->Init (pTex, m_pDevice))
			hr = E_FAIL;
	}

	//free pointers and return
	RELEASE (pTex);
	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetValue9 (D3DXHANDLE hParameter, LPVOID pData, UINT Bytes)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetValue (hParameter, pData, Bytes));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetVector9 (D3DXHANDLE hParameter, D3DXVECTOR4* pVector)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetVector (hParameter, pVector));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetVectorArray9 (D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, GetVectorArray (hParameter, pVector, Count));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::GetVertexShader9 (D3DXHANDLE hParameter, CVertexShader** ppVShader)
{
	if (NULL == m_pEffect9 || NULL == ppVShader)
		return E_FAIL;

	LPDIRECT3DVERTEXSHADER9 pShade = NULL;
	HRESULT hr;

	//get the vertex shader from the effect
	D3DXHANDLEVERSION(hr= ,ID3DXEffect, m_pEffect9, GetVertexShader (hParameter, &pShade));

	if (SUCCEEDED(hr) && NULL != pShade)
	{
		//if we got the shader, set it into CVertexShader
		*ppVShader = new CVertexShader;

		if (NULL == *ppVShader)
			hr = E_FAIL;
		else if (!(*ppVShader)->Init (pShade, m_pDevice))
			hr = E_FAIL;
	}

	//free pointers and return
	RELEASE (pShade);
	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::ApplyParameterBlock9 (D3DXHANDLE hParameterBlock)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	if(g_fD3DX >= 9.29f)
		return ((ID3DXEffect93*)m_pEffect9)->ApplyParameterBlock (hParameterBlock);
	else if(g_fD3DX < 9.29f &&  g_fD3DX >= 9.19f)
		return ((ID3DXEffect92*)m_pEffect9)->ApplyParameterBlock (hParameterBlock);
	else if(g_fD3DX < 9.19f && g_fD3DX >= 9.09f)
		return ((ID3DXEffect91*)m_pEffect9)->ApplyParameterBlock (hParameterBlock);
	else
		return E_NOTIMPL;
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::BeginParameterBlock9 (void)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	if(g_fD3DX >= 9.29f)
		return ((ID3DXEffect93*)m_pEffect9)->BeginParameterBlock();
	else if(g_fD3DX < 9.29f &&  g_fD3DX >= 9.19f)
		return ((ID3DXEffect92*)m_pEffect9)->BeginParameterBlock();
	else if (g_fD3DX < 9.19f && g_fD3DX >= 9.09f)
		return ((ID3DXEffect91*)m_pEffect9)->BeginParameterBlock();
	else
		return E_NOTIMPL;
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::CloneEffect9 (CDevice* pTargetDevice, CEffect** ppResultEffect)
{
	if (NULL == m_pEffect9 || NULL == ppResultEffect || NULL == pTargetDevice)
		return E_FAIL;

	LPD3DXEFFECT pEff = NULL;
	HRESULT hr;

	//clone the effect
	D3DXHANDLEVERSION(hr = ,ID3DXEffect, m_pEffect9, CloneEffect (pTargetDevice->m_pDevice9, &pEff));

	if (SUCCEEDED(hr) && NULL != pEff)
	{
		//if we got the new effect, set it into result CEffect
		*ppResultEffect = new CEffect;

		if (NULL == *ppResultEffect)
			hr = E_FAIL;
		else if (!(*ppResultEffect)->Init (pEff, pTargetDevice))
			hr = E_FAIL;
	}

	//free pointers and return
	RELEASE (pEff);
	return hr;
}

// --------------------------------------------------------------------------------------

D3DXHANDLE CEffect::EndParameterBlock9 (void)
{
	if (NULL == m_pEffect9)
		return NULL;

	if(g_fD3DX >= 9.29f)
		return ((ID3DXEffect93*)m_pEffect9)->EndParameterBlock();
	else if(g_fD3DX < 9.29f &&  g_fD3DX >= 9.19f)
		return ((ID3DXEffect92*)m_pEffect9)->EndParameterBlock();
	else if (g_fD3DX < 9.19f && g_fD3DX >= 9.09f)
		return ((ID3DXEffect91*)m_pEffect9)->EndParameterBlock();
	else
		return NULL;
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::DeleteParameterBlock9 (D3DXHANDLE hParameterBlock)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	if(g_fD3DX >= 9.29f)
		return ((ID3DXEffect93*)m_pEffect9)->DeleteParameterBlock(hParameterBlock);
	else
		return E_NOTIMPL;
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::SetRawValue9 (D3DXHANDLE hParameter, LPCVOID pData, UINT ByteOffset, UINT Bytes)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	if(g_fD3DX >= 9.29f)
		return ((ID3DXEffect93*)m_pEffect9)->SetRawValue(hParameter, pData, ByteOffset, Bytes);
	else
		return E_NOTIMPL;
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::FindNextValidTechnique9 (D3DXHANDLE hTechnique, D3DXHANDLE* pTechnique)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, FindNextValidTechnique (hTechnique, pTechnique));
}

// --------------------------------------------------------------------------------------

BOOL CEffect::IsParameterUsed9 (D3DXHANDLE hParameter, D3DXHANDLE hTechnique)
{
	if (NULL == m_pEffect9)
		return false;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, IsParameterUsed (hParameter, hTechnique));
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::OnLostDevice9 (void)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, OnLostDevice());
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::OnResetDevice9 (void)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, OnResetDevice());
}

// --------------------------------------------------------------------------------------

HRESULT CEffect::ValidateTechnique9 (D3DXHANDLE hTechnique)
{
	if (NULL == m_pEffect9)
		return E_FAIL;

	D3DXHANDLEVERSION(return ,ID3DXEffect, m_pEffect9, ValidateTechnique (hTechnique));
}

// --------------------------------------------------------------------------------------
