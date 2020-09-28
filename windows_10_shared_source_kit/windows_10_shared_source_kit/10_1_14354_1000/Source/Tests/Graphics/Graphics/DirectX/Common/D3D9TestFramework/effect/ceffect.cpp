#include "CEffect.h"

// -------------------------------------------------------------------------------------

CEffect::CEffect()
{
	m_pEffect9 = NULL;
	m_pDevice = NULL;

	pReleaseD3D = NULL;
}

// -------------------------------------------------------------------------------------

CEffect::~CEffect()
{
	if (pReleaseD3D)
		ReleaseD3D();
}

// -------------------------------------------------------------------------------------

ULONG WINAPI CEffect::AddRef (void)
{
    if (m_pDevice)
		m_pDevice->AddRef();
	return CObject::AddRef();
}

// -------------------------------------------------------------------------------------

ULONG WINAPI CEffect::Release (void)
{
    if (NULL != m_pDevice)
	{
		m_pDevice->Release();
		m_pDevice = NULL;
	}
	return CObject::Release();
}

// -------------------------------------------------------------------------------------

void* CEffect::GetInternalPointer (void)
{
	if (m_pEffect9)
		return m_pEffect9;

	return NULL;
}

// -------------------------------------------------------------------------------------


bool CEffect::Init (LPVOID pD3DXEffect, CDevice* pDevice)
{
	bool bRet = true;

	if (NULL == pDevice || NULL == pD3DXEffect)
		return false;

	m_pDevice = pDevice;
	m_pDevice->AddRef();

	if (9.0f <= m_pDevice->GetInterface() && 10.0f > m_pDevice->GetInterface())
	{
                pBegin = &CEffect::Begin9;
                pBeginPass = &CEffect::BeginPass9;
                pCommitChanges = &CEffect::CommitChanges9;
                pEndPass = &CEffect::EndPass9;
                pEnd = &CEffect::End9;
                pSetBool = &CEffect::SetBool9;
                pSetBoolArray = &CEffect::SetBoolArray9;
                pSetFloat = &CEffect::SetFloat9;
                pSetFloatArray = &CEffect::SetFloatArray9;
                pSetInt = &CEffect::SetInt9;
                pSetIntArray = &CEffect::SetIntArray9;
                pSetMatrix = &CEffect::SetMatrix9;
                pSetMatrixArray = &CEffect::SetMatrixArray9;
                pSetMatrixPointerArray = &CEffect::SetMatrixPointerArray9;
                pSetMatrixTranspose = &CEffect::SetMatrixTranspose9;
                pSetMatrixTransposeArray = &CEffect::SetMatrixTransposeArray9;
                pSetMatrixTransposePointerArray = &CEffect::SetMatrixTransposePointerArray9;
                pSetPixelShader = &CEffect::SetPixelShader9;
                //pSetStateManager = &CEffect::SetStateManager9;
                pSetString = &CEffect::SetString9;
                pSetTechnique = &CEffect::SetTechnique9;
                pSetTexture = &CEffect::SetTexture9;
                pSetCubeTexture = &CEffect::SetCubeTexture9;
                pSetVolTexture = &CEffect::SetVolTexture9;
                pSetValue = &CEffect::SetValue9;
                pSetVector = &CEffect::SetVector9;
                pSetVectorArray = &CEffect::SetVectorArray9;
                pSetArrayRange = &CEffect::SetArrayRange9;
                pSetVertexShader = &CEffect::SetVertexShader9;
                pGetAnnotation = &CEffect::GetAnnotation9;
                pGetAnnotationByName = &CEffect::GetAnnotationByName9;
                pGetBool = &CEffect::GetBool9;
                pGetBoolArray = &CEffect::GetBoolArray9;
                pGetCurrentTechnique = &CEffect::GetCurrentTechnique9;
                pGetDesc = &CEffect::GetDesc9;
                pGetDevice = &CEffect::GetDevice9;
                pGetFloat = &CEffect::GetFloat9;
                pGetFloatArray = &CEffect::GetFloatArray9;
                pGetFunction = &CEffect::GetFunction9;
                pGetFunctionByName = &CEffect::GetFunctionByName9;
                pGetFunctionDesc = &CEffect::GetFunctionDesc9;
                pGetInt = &CEffect::GetInt9;
                pGetIntArray = &CEffect::GetIntArray9;
                pGetMatrix = &CEffect::GetMatrix9;
                pGetMatrixArray = &CEffect::GetMatrixArray9;
                pGetMatrixPointerArray = &CEffect::GetMatrixPointerArray9;
                pGetMatrixTranspose = &CEffect::GetMatrixTranspose9;
                pGetMatrixTransposeArray = &CEffect::GetMatrixTransposeArray9;
                pGetMatrixTransposePointerArray = &CEffect::GetMatrixTransposePointerArray9;
                pGetParameter = &CEffect::GetParameter9;
                pGetParameterByName = &CEffect::GetParameterByName9;
                pGetParameterBySemantic = &CEffect::GetParameterBySemantic9;
                pGetParameterDesc = &CEffect::GetParameterDesc9;
                pGetParameterElement = &CEffect::GetParameterElement9;
                pGetPass = &CEffect::GetPass9;
                pGetPassByName = &CEffect::GetPassByName9;
                pGetPassDesc = &CEffect::GetPassDesc9;
                pGetPixelShader = &CEffect::GetPixelShader9;
                pGetPool = &CEffect::GetPool9;
                //pGetStateManager = &CEffect::GetStateManager9;
                pGetString = &CEffect::GetString9;
                pGetTechnique = &CEffect::GetTechnique9;
                pGetTechniqueByName = &CEffect::GetTechniqueByName9;
                pGetTechniqueDesc = &CEffect::GetTechniqueDesc9;
                pGetTexture = &CEffect::GetTexture9;
                pGetCubeTexture = &CEffect::GetCubeTexture9;
                pGetVolTexture = &CEffect::GetVolTexture9;
                pGetValue = &CEffect::GetValue9;
                pGetVector = &CEffect::GetVector9;
                pGetVectorArray = &CEffect::GetVectorArray9;
                pGetVertexShader = &CEffect::GetVertexShader9;
                pApplyParameterBlock = &CEffect::ApplyParameterBlock9;
                pBeginParameterBlock = &CEffect::BeginParameterBlock9;
                pEndParameterBlock = &CEffect::EndParameterBlock9;
                pDeleteParameterBlock = &CEffect::DeleteParameterBlock9;
                pCloneEffect = &CEffect::CloneEffect9;
                pSetRawValue = &CEffect::SetRawValue9;
                pFindNextValidTechnique = &CEffect::FindNextValidTechnique9;
                pIsParameterUsed = &CEffect::IsParameterUsed9;
                pOnLostDevice = &CEffect::OnLostDevice9;
                pOnResetDevice = &CEffect::OnResetDevice9;
                pValidateTechnique = &CEffect::ValidateTechnique9;
                pReleaseD3D = &CEffect::ReleaseD3D9;

                bRet = Init9 (pD3DXEffect);
	}
	else
	{
                pBegin = &CEffect::BeginNotImplemented;
                pBeginPass = &CEffect::BeginPassNotImplemented;
                pCommitChanges = &CEffect::CommitChangesNotImplemented;
                pEndPass = &CEffect::EndPassNotImplemented;
                pEnd = &CEffect::EndNotImplemented;
                pSetBool = &CEffect::SetBoolNotImplemented;
                pSetBoolArray = &CEffect::SetBoolArrayNotImplemented;
                pSetFloat = &CEffect::SetFloatNotImplemented;
                pSetFloatArray = &CEffect::SetFloatArrayNotImplemented;
                pSetInt = &CEffect::SetIntNotImplemented;
                pSetIntArray = &CEffect::SetIntArrayNotImplemented;
                pSetMatrix = &CEffect::SetMatrixNotImplemented;
                pSetMatrixArray = &CEffect::SetMatrixArrayNotImplemented;
                pSetMatrixPointerArray = &CEffect::SetMatrixPointerArrayNotImplemented;
                pSetMatrixTranspose = &CEffect::SetMatrixTransposeNotImplemented;
                pSetMatrixTransposeArray = &CEffect::SetMatrixTransposeArrayNotImplemented;
                pSetMatrixTransposePointerArray = &CEffect::SetMatrixTransposePointerArrayNotImplemented;
                pSetPixelShader = &CEffect::SetPixelShaderNotImplemented;
                //pSetStateManager = &CEffect::SetStateManagerNotImplemented;
                pSetString = &CEffect::SetStringNotImplemented;
                pSetTechnique = &CEffect::SetTechniqueNotImplemented;
                pSetTexture = &CEffect::SetTextureNotImplemented;
                pSetCubeTexture = &CEffect::SetCubeTextureNotImplemented;
                pSetVolTexture = &CEffect::SetVolTextureNotImplemented;
                pSetValue = &CEffect::SetValueNotImplemented;
                pSetVector = &CEffect::SetVectorNotImplemented;
                pSetVectorArray = &CEffect::SetVectorArrayNotImplemented;
                pSetVertexShader = &CEffect::SetVertexShaderNotImplemented;
                pSetArrayRange = &CEffect::SetArrayRangeNotImplemented;
                pGetAnnotation = &CEffect::GetAnnotationNotImplemented;
                pGetAnnotationByName = &CEffect::GetAnnotationByNameNotImplemented;
                pGetBool = &CEffect::GetBoolNotImplemented;
                pGetBoolArray = &CEffect::GetBoolArrayNotImplemented;
                pGetCurrentTechnique = &CEffect::GetCurrentTechniqueNotImplemented;
                pGetDesc = &CEffect::GetDescNotImplemented;
                pGetDevice = &CEffect::GetDeviceNotImplemented;
                pGetFloat = &CEffect::GetFloatNotImplemented;
                pGetFloatArray = &CEffect::GetFloatArrayNotImplemented;
                pGetFunction = &CEffect::GetFunctionNotImplemented;
                pGetFunctionByName = &CEffect::GetFunctionByNameNotImplemented;
                pGetFunctionDesc = &CEffect::GetFunctionDescNotImplemented;
                pGetInt = &CEffect::GetIntNotImplemented;
                pGetIntArray = &CEffect::GetIntArrayNotImplemented;
                pGetMatrix = &CEffect::GetMatrixNotImplemented;
                pGetMatrixArray = &CEffect::GetMatrixArrayNotImplemented;
                pGetMatrixPointerArray = &CEffect::GetMatrixPointerArrayNotImplemented;
                pGetMatrixTranspose = &CEffect::GetMatrixTransposeNotImplemented;
                pGetMatrixTransposeArray = &CEffect::GetMatrixTransposeArrayNotImplemented;
                pGetMatrixTransposePointerArray = &CEffect::GetMatrixTransposePointerArrayNotImplemented;
                pGetParameter = &CEffect::GetParameterNotImplemented;
                pGetParameterByName = &CEffect::GetParameterByNameNotImplemented;
                pGetParameterBySemantic = &CEffect::GetParameterBySemanticNotImplemented;
                pGetParameterDesc = &CEffect::GetParameterDescNotImplemented;
                pGetParameterElement = &CEffect::GetParameterElementNotImplemented;
                pGetPass = &CEffect::GetPassNotImplemented;
                pGetPassByName = &CEffect::GetPassByNameNotImplemented;
                pGetPassDesc = &CEffect::GetPassDescNotImplemented;
                pGetPixelShader = &CEffect::GetPixelShaderNotImplemented;
                pGetPool = &CEffect::GetPoolNotImplemented;
                //pGetStateManager = &CEffect::GetStateManagerNotImplemented;
                pGetString = &CEffect::GetStringNotImplemented;
                pGetTechnique = &CEffect::GetTechniqueNotImplemented;
                pGetTechniqueByName = &CEffect::GetTechniqueByNameNotImplemented;
                pGetTechniqueDesc = &CEffect::GetTechniqueDescNotImplemented;
                pGetTexture = &CEffect::GetTextureNotImplemented;
                pGetCubeTexture = &CEffect::GetCubeTextureNotImplemented;
                pGetVolTexture = &CEffect::GetVolTextureNotImplemented;
                pGetValue = &CEffect::GetValueNotImplemented;
                pGetVector = &CEffect::GetVectorNotImplemented;
                pGetVectorArray = &CEffect::GetVectorArrayNotImplemented;
                pGetVertexShader = &CEffect::GetVertexShaderNotImplemented;
                pApplyParameterBlock = &CEffect::ApplyParameterBlockNotImplemented;
                pBeginParameterBlock = &CEffect::BeginParameterBlockNotImplemented;
                pEndParameterBlock = &CEffect::EndParameterBlockNotImplemented;
                pDeleteParameterBlock = &CEffect::DeleteParameterBlockNotImplemented;
                pCloneEffect = &CEffect::CloneEffectNotImplemented;
                pSetRawValue = &CEffect::SetRawValueNotImplemented;
                pFindNextValidTechnique = &CEffect::FindNextValidTechniqueNotImplemented;
                pIsParameterUsed = &CEffect::IsParameterUsedNotImplemented;
                pOnLostDevice = &CEffect::OnLostDeviceNotImplemented;
                pOnResetDevice = &CEffect::OnResetDeviceNotImplemented;
                pValidateTechnique = &CEffect::ValidateTechniqueNotImplemented;
	}

	return bRet;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::BeginNotImplemented (UINT* pPasses, DWORD Flags)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::BeginPassNotImplemented (UINT iPass)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::CommitChangesNotImplemented (void)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::EndPassNotImplemented (void)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::EndNotImplemented (void)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetBoolNotImplemented (D3DXHANDLE hParameter, BOOL b)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetBoolArrayNotImplemented (D3DXHANDLE hParameter, const BOOL* pB, UINT Count)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetFloatNotImplemented (D3DXHANDLE hParameter, FLOAT f)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetFloatArrayNotImplemented (D3DXHANDLE hParameter, const FLOAT* pf, UINT Count)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetIntNotImplemented (D3DXHANDLE hParameter, int n)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetIntArrayNotImplemented (D3DXHANDLE hParameter, const int* pn, UINT Count)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetMatrixNotImplemented (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetMatrixArrayNotImplemented (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, UINT Count)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetMatrixPointerArrayNotImplemented (D3DXHANDLE hParameter, const D3DXMATRIX** ppMatrix, UINT Count)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetMatrixTransposeNotImplemented (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetMatrixTransposeArrayNotImplemented (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, UINT Count)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetMatrixTransposePointerArrayNotImplemented (D3DXHANDLE hParameter, const D3DXMATRIX** ppMatrix, UINT Count)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetPixelShaderNotImplemented (D3DXHANDLE hParameter, CPShader* pPShader)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------
//change this later if a wrapper for LPD3DXEFFECTSTATEMANAGER is added
/*
HRESULT CEffect::SetStateManagerNotImplemented (LPD3DXEFFECTSTATEMANAGER pManager)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}
*/
// -------------------------------------------------------------------------------------

HRESULT CEffect::SetStringNotImplemented (D3DXHANDLE hParameter, const LPCSTR pString)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetTechniqueNotImplemented (D3DXHANDLE hTechnique)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetTextureNotImplemented (D3DXHANDLE hParameter, CTexture* pTexture)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetCubeTextureNotImplemented (D3DXHANDLE hParameter, CCubeTexture* pCubeTexture)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetVolTextureNotImplemented (D3DXHANDLE hParameter, CVolTexture* pVolTexture)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetValueNotImplemented (D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetVectorNotImplemented (D3DXHANDLE hParameter, const D3DXVECTOR4* pVector)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetVectorArrayNotImplemented (D3DXHANDLE hParameter, const D3DXVECTOR4* pVector, UINT Count)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetVertexShaderNotImplemented (D3DXHANDLE hParameter, CVertexShader* pVertexShader)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetArrayRangeNotImplemented (D3DXHANDLE hParameter, UINT Start, UINT End)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetAnnotationNotImplemented (D3DXHANDLE hObject, UINT Index)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return NULL;
}

// -------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetAnnotationByNameNotImplemented (D3DXHANDLE hObject, LPCSTR pName)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return NULL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetBoolNotImplemented (D3DXHANDLE hParameter, BOOL* pb)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetBoolArrayNotImplemented (D3DXHANDLE hParameter, BOOL* pB, UINT Count)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetCurrentTechniqueNotImplemented (void)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return NULL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetDescNotImplemented (D3DXEFFECT_DESC* pDesc)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetDeviceNotImplemented (CDevice** ppDevice)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetFloatNotImplemented (D3DXHANDLE hParameter, FLOAT* pf)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetFloatArrayNotImplemented (D3DXHANDLE hParameter, FLOAT* pf, UINT Count)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetFunctionNotImplemented (UINT Index)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return NULL;
}

// -------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetFunctionByNameNotImplemented (LPCSTR pName)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return NULL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetFunctionDescNotImplemented (D3DXHANDLE hFunction, D3DXFUNCTION_DESC* pDesc)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetIntNotImplemented (D3DXHANDLE hParameter, int* pn)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetIntArrayNotImplemented (D3DXHANDLE hParameter, int* pn, UINT Count)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetMatrixNotImplemented (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetMatrixArrayNotImplemented (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetMatrixPointerArrayNotImplemented (D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetMatrixTransposeNotImplemented (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetMatrixTransposeArrayNotImplemented (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetMatrixTransposePointerArrayNotImplemented (D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetParameterNotImplemented (D3DXHANDLE hParent, UINT Index)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return NULL;
}

// -------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetParameterByNameNotImplemented (D3DXHANDLE hParent, LPCSTR pName)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return NULL;
}

// -------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetParameterBySemanticNotImplemented (D3DXHANDLE hParent, LPCSTR pSemantic)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return NULL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetParameterDescNotImplemented (D3DXHANDLE hParameter, D3DXPARAMETER_DESC* pDesc)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetParameterElementNotImplemented (D3DXHANDLE hParent, UINT ElementIndex)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return NULL;
}

// -------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetPassNotImplemented (D3DXHANDLE hTechnique, UINT Index)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return NULL;
}

// -------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetPassByNameNotImplemented (D3DXHANDLE hTechnique, LPCSTR pName)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return NULL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetPassDescNotImplemented (D3DXHANDLE hPass, D3DXPASS_DESC* pDesc)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetPixelShaderNotImplemented (D3DXHANDLE hParameter, CPShader** ppPShader)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetPoolNotImplemented (LPD3DXEFFECTPOOL* ppPool)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------
//change this later if a wrapper for LPD3DXEFFECTSTATEMANAGER is added
/*
HRESULT CEffect::GetStateManagerNotImplemented (LPD3DXEFFECTSTATEMANAGER* ppManager)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}
*/
// -------------------------------------------------------------------------------------

HRESULT CEffect::GetStringNotImplemented (D3DXHANDLE hParameter, LPCSTR* ppString)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetTechniqueNotImplemented (UINT Index)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return NULL;
}

// -------------------------------------------------------------------------------------

D3DXHANDLE CEffect::GetTechniqueByNameNotImplemented (LPCSTR pName)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return NULL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetTechniqueDescNotImplemented (D3DXHANDLE hTechnique, D3DXTECHNIQUE_DESC* pDesc)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetTextureNotImplemented (D3DXHANDLE hParameter, CTexture** ppTexture)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetCubeTextureNotImplemented (D3DXHANDLE hParameter, CCubeTexture** ppCubeTexture)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetVolTextureNotImplemented (D3DXHANDLE hParameter, CVolTexture** ppVolTexture)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetValueNotImplemented (D3DXHANDLE hParameter, LPVOID pData, UINT Bytes)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetVectorNotImplemented (D3DXHANDLE hParameter, D3DXVECTOR4* pVector)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetVectorArrayNotImplemented (D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::GetVertexShaderNotImplemented (D3DXHANDLE hParameter, CVertexShader** ppVShader)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::ApplyParameterBlockNotImplemented (D3DXHANDLE hParameterBlock)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::BeginParameterBlockNotImplemented (void)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

D3DXHANDLE CEffect::EndParameterBlockNotImplemented (void)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return NULL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::DeleteParameterBlockNotImplemented (D3DXHANDLE hParameterBlock)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::CloneEffectNotImplemented (CDevice* pTargetDevice, CEffect** ppResultEffect)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::SetRawValueNotImplemented (D3DXHANDLE hParameter, LPCVOID pData, UINT ByteOffset, UINT Bytes)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::FindNextValidTechniqueNotImplemented (D3DXHANDLE hTechnique, D3DXHANDLE* pTechnique)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

BOOL CEffect::IsParameterUsedNotImplemented (D3DXHANDLE hParameter,D3DXHANDLE hTechnique)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return false;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::OnLostDeviceNotImplemented (void)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::OnResetDeviceNotImplemented (void)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CEffect::ValidateTechniqueNotImplemented (D3DXHANDLE hTechnique)
{
	DPF(1, _T("CEffect not implemented in this interface version\n"));
	return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------
