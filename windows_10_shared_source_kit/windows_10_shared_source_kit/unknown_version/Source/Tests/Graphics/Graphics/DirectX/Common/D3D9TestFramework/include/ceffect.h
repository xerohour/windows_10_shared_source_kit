#ifndef __CEFFECT_H__
#define __CEFFECT_H__

//#include <d3d9.h>
#include <d3dx9.h>
#include <types.h>

#include "Framework.h"
//#include <d3dx9effect.h>
#include "CTextures.h"

class CEffect : public CObject
{
protected:

	friend class CDevice;

	LPD3DXEFFECT	m_pEffect9;
	CDevice*		m_pDevice;

    bool Init9 (LPVOID pEffect);

	LPD3DXEFFECT GetEffectPointer9() { return m_pEffect9; }
	virtual void*	GetInternalPointer (void);

	void ReleaseD3D(void) { (this->*pReleaseD3D)(); }
    void (CEffect::*pReleaseD3D)(void);
    void ReleaseD3D9();

//	bool (CEffect::*pCreateEffectFromFile)(LPCTSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors)



	HRESULT (CEffect::*pBegin)(UINT* pPasses, DWORD Flags);
	HRESULT BeginNotImplemented (UINT* pPasses, DWORD Flags);
	HRESULT Begin9 (UINT* pPasses, DWORD Flags);

	HRESULT (CEffect::*pBeginPass)(UINT Pass);
	HRESULT BeginPassNotImplemented (UINT Pass);
	HRESULT BeginPass9 (UINT Pass);

	HRESULT (CEffect::*pCommitChanges)(void);
	HRESULT CommitChangesNotImplemented (void);
	HRESULT CommitChanges9 (void);

	HRESULT (CEffect::*pEndPass)(void);
	HRESULT EndPassNotImplemented (void);
	HRESULT EndPass9 (void);

	HRESULT (CEffect::*pEnd)(void);
	HRESULT EndNotImplemented (void);
	HRESULT End9 (void);


	HRESULT (CEffect::*pSetBool)(D3DXHANDLE hParameter, BOOL b);
	HRESULT SetBoolNotImplemented (D3DXHANDLE hParameter, BOOL b);
	HRESULT SetBool9 (D3DXHANDLE hParameter, BOOL b);

	HRESULT (CEffect::*pSetBoolArray)(D3DXHANDLE hParameter, const BOOL* pB, UINT Count);
	HRESULT SetBoolArrayNotImplemented (D3DXHANDLE hParameter, const BOOL* pB, UINT Count);
	HRESULT SetBoolArray9 (D3DXHANDLE hParameter, const BOOL* pB, UINT Count);

	HRESULT (CEffect::*pSetFloat)(D3DXHANDLE hParameter, FLOAT f);
	HRESULT SetFloatNotImplemented (D3DXHANDLE hParameter, FLOAT f);
	HRESULT SetFloat9 (D3DXHANDLE hParameter, FLOAT f);

	HRESULT (CEffect::*pSetFloatArray)(D3DXHANDLE hParameter, const FLOAT* pf, UINT Count);
	HRESULT SetFloatArrayNotImplemented (D3DXHANDLE hParameter, const FLOAT* pf, UINT Count);
	HRESULT SetFloatArray9 (D3DXHANDLE hParameter, const FLOAT* pf, UINT Count);

	HRESULT (CEffect::*pSetInt)(D3DXHANDLE hParameter, int n);
	HRESULT SetIntNotImplemented (D3DXHANDLE hParameter, int n);
	HRESULT SetInt9 (D3DXHANDLE hParameter, int n);

	HRESULT (CEffect::*pSetIntArray)(D3DXHANDLE hParameter, const int* pn, UINT Count);
	HRESULT SetIntArrayNotImplemented (D3DXHANDLE hParameter, const int* pn, UINT Count);
	HRESULT SetIntArray9 (D3DXHANDLE hParameter, const int* pn, UINT Count);

	HRESULT (CEffect::*pSetMatrix)(D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix);
	HRESULT SetMatrixNotImplemented (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix);
	HRESULT SetMatrix9 (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix);

	HRESULT (CEffect::*pSetMatrixArray)(D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, UINT Count);
	HRESULT SetMatrixArrayNotImplemented (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, UINT Count);
	HRESULT SetMatrixArray9 (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, UINT Count);

	HRESULT (CEffect::*pSetMatrixPointerArray)(D3DXHANDLE hParameter, const D3DXMATRIX** ppMatrix, UINT Count);		//spec indicates this as* pMatrix, not** ppMatrix
	HRESULT SetMatrixPointerArrayNotImplemented (D3DXHANDLE hParameter, const D3DXMATRIX** ppMatrix, UINT Count);
	HRESULT SetMatrixPointerArray9 (D3DXHANDLE hParameter, const D3DXMATRIX** ppMatrix, UINT Count);

	HRESULT (CEffect::*pSetMatrixTranspose)(D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix);
	HRESULT SetMatrixTransposeNotImplemented (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix);
	HRESULT SetMatrixTranspose9 (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix);

	HRESULT (CEffect::*pSetMatrixTransposeArray)(D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, UINT Count);
	HRESULT SetMatrixTransposeArrayNotImplemented (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, UINT Count);
	HRESULT SetMatrixTransposeArray9 (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, UINT Count);

	HRESULT (CEffect::*pSetMatrixTransposePointerArray)(D3DXHANDLE hParameter, const D3DXMATRIX** ppMatrix, UINT Count);
	HRESULT SetMatrixTransposePointerArrayNotImplemented (D3DXHANDLE hParameter, const D3DXMATRIX** ppMatrix, UINT Count);
	HRESULT SetMatrixTransposePointerArray9 (D3DXHANDLE hParameter, const D3DXMATRIX** ppMatrix, UINT Count);

	HRESULT (CEffect::*pSetPixelShader)(D3DXHANDLE hParameter, CPShader* pPShader);
	HRESULT SetPixelShaderNotImplemented (D3DXHANDLE hParameter, CPShader* pPShader);
	HRESULT SetPixelShader9 (D3DXHANDLE hParameter, CPShader* pPShader);
	
	//change this later if a wrapper for LPD3DXEFFECTSTATEMANAGER is added
//	HRESULT (CEffect::*pSetStateManager)(LPD3DXEFFECTSTATEMANAGER pManager);
//	HRESULT SetStateManagerNotImplemented (LPD3DXEFFECTSTATEMANAGER pManager);
//	HRESULT SetStateManager9 (LPD3DXEFFECTSTATEMANAGER pManager);
	
	HRESULT (CEffect::*pSetString)(D3DXHANDLE hParameter, const LPCSTR pString);
	HRESULT SetStringNotImplemented (D3DXHANDLE hParameter, const LPCSTR pString);
	HRESULT SetString9 (D3DXHANDLE hParameter, const LPCSTR pString);
	
	HRESULT (CEffect::*pSetTechnique)(D3DXHANDLE hTechnique);
	HRESULT SetTechniqueNotImplemented (D3DXHANDLE hTechnique);
	HRESULT SetTechnique9 (D3DXHANDLE hTechnique);
	
	HRESULT (CEffect::*pSetTexture)(D3DXHANDLE hParameter, CTexture* pTexture);
	HRESULT SetTextureNotImplemented (D3DXHANDLE hParameter, CTexture* pTexture);
	HRESULT SetTexture9 (D3DXHANDLE hParameter, CTexture* pTexture);
	
	HRESULT (CEffect::*pSetCubeTexture)(D3DXHANDLE hParameter, CCubeTexture* pCubeTexture);
	HRESULT SetCubeTextureNotImplemented (D3DXHANDLE hParameter, CCubeTexture* pCubeTexture);
	HRESULT SetCubeTexture9 (D3DXHANDLE hParameter, CCubeTexture* pCubeTexture);
	
	HRESULT (CEffect::*pSetVolTexture)(D3DXHANDLE hParameter, CVolTexture* pVolTexture);
	HRESULT SetVolTextureNotImplemented (D3DXHANDLE hParameter, CVolTexture* pVolTexture);
	HRESULT SetVolTexture9 (D3DXHANDLE hParameter, CVolTexture* pVolTexture);

	HRESULT (CEffect::*pSetValue)(D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes);
	HRESULT SetValueNotImplemented (D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes);
	HRESULT SetValue9 (D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes);
	
	HRESULT (CEffect::*pSetVector)(D3DXHANDLE hParameter, const D3DXVECTOR4* pVector);
	HRESULT SetVectorNotImplemented (D3DXHANDLE hParameter, const D3DXVECTOR4* pVector);
	HRESULT SetVector9 (D3DXHANDLE hParameter, const D3DXVECTOR4* pVector);
	
	HRESULT (CEffect::*pSetVectorArray)(D3DXHANDLE hParameter, const D3DXVECTOR4* pVector, UINT Count);
	HRESULT SetVectorArrayNotImplemented (D3DXHANDLE hParameter, const D3DXVECTOR4* pVector, UINT Count);
	HRESULT SetVectorArray9 (D3DXHANDLE hParameter, const D3DXVECTOR4* pVector, UINT Count);
	
	HRESULT (CEffect::*pSetVertexShader)(D3DXHANDLE hParameter, CVertexShader* pVertexShader);
	HRESULT SetVertexShaderNotImplemented (D3DXHANDLE hParameter, CVertexShader* pVertexShader);
	HRESULT SetVertexShader9 (D3DXHANDLE hParameter, CVertexShader* pVertexShader);

	HRESULT (CEffect::*pSetArrayRange)(D3DXHANDLE hParameter, UINT uStart, UINT uEnd);
	HRESULT SetArrayRangeNotImplemented (D3DXHANDLE hParameter, UINT uStart, UINT uEnd);
	HRESULT SetArrayRange9 (D3DXHANDLE hParameter, UINT uStart, UINT uEnd);

	D3DXHANDLE (CEffect::*pGetAnnotation)(D3DXHANDLE hObject, UINT Index);
	D3DXHANDLE GetAnnotationNotImplemented (D3DXHANDLE hObject, UINT Index);
	D3DXHANDLE GetAnnotation9 (D3DXHANDLE hObject, UINT Index);
	
	D3DXHANDLE (CEffect::*pGetAnnotationByName)(D3DXHANDLE hObject, LPCSTR pName);
	D3DXHANDLE GetAnnotationByNameNotImplemented (D3DXHANDLE hObject, LPCSTR pName);
	D3DXHANDLE GetAnnotationByName9 (D3DXHANDLE hObject, LPCSTR pName);
	
	HRESULT (CEffect::*pGetBool)(D3DXHANDLE hParameter, BOOL* pb);
	HRESULT GetBoolNotImplemented (D3DXHANDLE hParameter, BOOL* pb);
	HRESULT GetBool9 (D3DXHANDLE hParameter, BOOL* pb);
	
	HRESULT (CEffect::*pGetBoolArray)(D3DXHANDLE hParameter, BOOL* pB, UINT Count);
	HRESULT GetBoolArrayNotImplemented (D3DXHANDLE hParameter, BOOL* pB, UINT Count);
	HRESULT GetBoolArray9 (D3DXHANDLE hParameter, BOOL* pB, UINT Count);
	
	D3DXHANDLE (CEffect::*pGetCurrentTechnique)(void);
	D3DXHANDLE GetCurrentTechniqueNotImplemented (void);
	D3DXHANDLE GetCurrentTechnique9 (void);
	
	HRESULT (CEffect::*pGetDesc)(D3DXEFFECT_DESC* pDesc);
	HRESULT GetDescNotImplemented (D3DXEFFECT_DESC* pDesc);
	HRESULT GetDesc9 (D3DXEFFECT_DESC* pDesc);
	
	HRESULT (CEffect::*pGetDevice)(CDevice** ppDevice);
	HRESULT GetDeviceNotImplemented (CDevice** ppDevice);
	HRESULT GetDevice9 (CDevice** ppDevice);
	
	HRESULT (CEffect::*pGetFloat)(D3DXHANDLE hParameter, FLOAT* pf);
	HRESULT GetFloatNotImplemented (D3DXHANDLE hParameter, FLOAT* pf);
	HRESULT GetFloat9 (D3DXHANDLE hParameter, FLOAT* pf);
	
	HRESULT (CEffect::*pGetFloatArray)(D3DXHANDLE hParameter, FLOAT* pf, UINT Count);
	HRESULT GetFloatArrayNotImplemented (D3DXHANDLE hParameter, FLOAT* pf, UINT Count);
	HRESULT GetFloatArray9 (D3DXHANDLE hParameter, FLOAT* pf, UINT Count);
	
	D3DXHANDLE (CEffect::*pGetFunction)(UINT Index);
	D3DXHANDLE GetFunctionNotImplemented (UINT Index);
	D3DXHANDLE GetFunction9 (UINT Index);
	
	D3DXHANDLE (CEffect::*pGetFunctionByName)(LPCSTR pName);
	D3DXHANDLE GetFunctionByNameNotImplemented (LPCSTR pName);
	D3DXHANDLE GetFunctionByName9 (LPCSTR pName);
	
	HRESULT (CEffect::*pGetFunctionDesc)(D3DXHANDLE hFunction, D3DXFUNCTION_DESC* pDesc);
	HRESULT GetFunctionDescNotImplemented (D3DXHANDLE hFunction, D3DXFUNCTION_DESC* pDesc);
	HRESULT GetFunctionDesc9 (D3DXHANDLE hFunction, D3DXFUNCTION_DESC* pDesc);
	
	HRESULT (CEffect::*pGetInt)(D3DXHANDLE hParameter, int* pn);
	HRESULT GetIntNotImplemented (D3DXHANDLE hParameter, int* pn);
	HRESULT GetInt9 (D3DXHANDLE hParameter, int* pn);
	
	HRESULT (CEffect::*pGetIntArray)(D3DXHANDLE hParameter, int* pn, UINT Count);
	HRESULT GetIntArrayNotImplemented (D3DXHANDLE hParameter, int* pn, UINT Count);
	HRESULT GetIntArray9 (D3DXHANDLE hParameter, int* pn, UINT Count);
	
	HRESULT (CEffect::*pGetMatrix)(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix);
	HRESULT GetMatrixNotImplemented (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix);
	HRESULT GetMatrix9 (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix);
	
	HRESULT (CEffect::*pGetMatrixArray)(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count);
	HRESULT GetMatrixArrayNotImplemented (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count);
	HRESULT GetMatrixArray9 (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count);
	
	HRESULT (CEffect::*pGetMatrixPointerArray)(D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count);  //spec says CONST D3DXMATRIX* pMatrix, not D3DXMATRIX** ppMatrix
	HRESULT GetMatrixPointerArrayNotImplemented (D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count);
	HRESULT GetMatrixPointerArray9 (D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count);
		
	HRESULT (CEffect::*pGetMatrixTranspose)(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix);
	HRESULT GetMatrixTransposeNotImplemented (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix);
	HRESULT GetMatrixTranspose9 (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix);
		
	HRESULT (CEffect::*pGetMatrixTransposeArray)(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count);
	HRESULT GetMatrixTransposeArrayNotImplemented (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count);
	HRESULT GetMatrixTransposeArray9 (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count);
		
	HRESULT (CEffect::*pGetMatrixTransposePointerArray)(D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count); //spec says CONST D3DXMATRIX** ppMatrix, not D3DXMATRIX** ppMatrix
	HRESULT GetMatrixTransposePointerArrayNotImplemented (D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count);
	HRESULT GetMatrixTransposePointerArray9 (D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count);
		
	D3DXHANDLE (CEffect::*pGetParameter)(D3DXHANDLE hParent, UINT Index);
	D3DXHANDLE GetParameterNotImplemented (D3DXHANDLE hParent, UINT Index);
	D3DXHANDLE GetParameter9 (D3DXHANDLE hParent, UINT Index);
		
	D3DXHANDLE (CEffect::*pGetParameterByName)(D3DXHANDLE hParent, LPCSTR pName);
	D3DXHANDLE GetParameterByNameNotImplemented (D3DXHANDLE hParent, LPCSTR pName);
	D3DXHANDLE GetParameterByName9 (D3DXHANDLE hParent, LPCSTR pName);
		
	D3DXHANDLE (CEffect::*pGetParameterBySemantic)(D3DXHANDLE hParent, LPCSTR pSemantic);
	D3DXHANDLE GetParameterBySemanticNotImplemented (D3DXHANDLE hParent, LPCSTR pSemantic);
	D3DXHANDLE GetParameterBySemantic9 (D3DXHANDLE hParent, LPCSTR pSemantic);
		
	HRESULT (CEffect::*pGetParameterDesc)(D3DXHANDLE hParameter, D3DXPARAMETER_DESC* pDesc);
	HRESULT GetParameterDescNotImplemented (D3DXHANDLE hParameter, D3DXPARAMETER_DESC* pDesc);
	HRESULT GetParameterDesc9 (D3DXHANDLE hParameter, D3DXPARAMETER_DESC* pDesc);
		
	D3DXHANDLE (CEffect::*pGetParameterElement)(D3DXHANDLE hParent, UINT ElementIndex);
	D3DXHANDLE GetParameterElementNotImplemented (D3DXHANDLE hParent, UINT ElementIndex);
	D3DXHANDLE GetParameterElement9 (D3DXHANDLE hParent, UINT ElementIndex);
		
	D3DXHANDLE (CEffect::*pGetPass)(D3DXHANDLE hTechnique, UINT Index);
	D3DXHANDLE GetPassNotImplemented (D3DXHANDLE hTechnique, UINT Index);
	D3DXHANDLE GetPass9 (D3DXHANDLE hTechnique, UINT Index);
		
	D3DXHANDLE (CEffect::*pGetPassByName)(D3DXHANDLE hTechnique, LPCSTR pName);
	D3DXHANDLE GetPassByNameNotImplemented (D3DXHANDLE hTechnique, LPCSTR pName);
	D3DXHANDLE GetPassByName9 (D3DXHANDLE hTechnique, LPCSTR pName);
		
	HRESULT (CEffect::*pGetPassDesc)(D3DXHANDLE hPass, D3DXPASS_DESC* pDesc);
	HRESULT GetPassDescNotImplemented (D3DXHANDLE hPass, D3DXPASS_DESC* pDesc);
	HRESULT GetPassDesc9 (D3DXHANDLE hPass, D3DXPASS_DESC* pDesc);
		
	HRESULT (CEffect::*pGetPixelShader)(D3DXHANDLE hParameter, CPShader** ppPShader);
	HRESULT GetPixelShaderNotImplemented (D3DXHANDLE hParameter, CPShader** ppPShader);
	HRESULT GetPixelShader9 (D3DXHANDLE hParameter, CPShader** ppPShader);
		
	HRESULT (CEffect::*pGetPool)(LPD3DXEFFECTPOOL* ppPool);
	HRESULT GetPoolNotImplemented (LPD3DXEFFECTPOOL* ppPool);
	HRESULT GetPool9 (LPD3DXEFFECTPOOL* ppPool);
		
	//change this later if a wrapper for LPD3DXEFFECTSTATEMANAGER is added
//	HRESULT (CEffect::*pGetStateManager)(LPD3DXEFFECTSTATEMANAGER* ppManager);
//	HRESULT GetStateManagerNotImplemented (LPD3DXEFFECTSTATEMANAGER* ppManager);
//	HRESULT GetStateManager9 (LPD3DXEFFECTSTATEMANAGER* ppManager);
		
	HRESULT (CEffect::*pGetString)(D3DXHANDLE hParameter, LPCSTR* ppString);
	HRESULT GetStringNotImplemented (D3DXHANDLE hParameter, LPCSTR* ppString);
	HRESULT GetString9 (D3DXHANDLE hParameter, LPCSTR* ppString);
		
	D3DXHANDLE (CEffect::*pGetTechnique)(UINT Index);
	D3DXHANDLE GetTechniqueNotImplemented (UINT Index);
	D3DXHANDLE GetTechnique9 (UINT Index);
		
	D3DXHANDLE (CEffect::*pGetTechniqueByName)(LPCSTR pName);
	D3DXHANDLE GetTechniqueByNameNotImplemented (LPCSTR pName);
	D3DXHANDLE GetTechniqueByName9 (LPCSTR pName);
		
	HRESULT (CEffect::*pGetTechniqueDesc)(D3DXHANDLE hTechnique, D3DXTECHNIQUE_DESC* pDesc);
	HRESULT GetTechniqueDescNotImplemented (D3DXHANDLE hTechnique, D3DXTECHNIQUE_DESC* pDesc);
	HRESULT GetTechniqueDesc9 (D3DXHANDLE hTechnique, D3DXTECHNIQUE_DESC* pDesc);
		
	HRESULT (CEffect::*pGetTexture)(D3DXHANDLE hParameter, CTexture** ppTexture);
	HRESULT GetTextureNotImplemented (D3DXHANDLE hParameter, CTexture** ppTexture);
	HRESULT GetTexture9 (D3DXHANDLE hParameter, CTexture** ppTexture);
		
	HRESULT (CEffect::*pGetCubeTexture)(D3DXHANDLE hParameter, CCubeTexture** ppCubeTexture);
	HRESULT GetCubeTextureNotImplemented (D3DXHANDLE hParameter, CCubeTexture** ppCubeTexture);
	HRESULT GetCubeTexture9 (D3DXHANDLE hParameter, CCubeTexture** ppCubeTexture);

	HRESULT (CEffect::*pGetVolTexture)(D3DXHANDLE hParameter, CVolTexture** ppVolTexture);
	HRESULT GetVolTextureNotImplemented (D3DXHANDLE hParameter, CVolTexture** ppVolTexture);
	HRESULT GetVolTexture9 (D3DXHANDLE hParameter, CVolTexture** ppVolTexture);

	HRESULT (CEffect::*pGetValue)(D3DXHANDLE hParameter, LPVOID pData, UINT Bytes);
	HRESULT GetValueNotImplemented (D3DXHANDLE hParameter, LPVOID pData, UINT Bytes);
	HRESULT GetValue9 (D3DXHANDLE hParameter, LPVOID pData, UINT Bytes);
		
	HRESULT (CEffect::*pGetVector)(D3DXHANDLE hParameter, D3DXVECTOR4* pVector);
	HRESULT GetVectorNotImplemented (D3DXHANDLE hParameter, D3DXVECTOR4* pVector);
	HRESULT GetVector9 (D3DXHANDLE hParameter, D3DXVECTOR4* pVector);
		
	HRESULT (CEffect::*pGetVectorArray)(D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count);
	HRESULT GetVectorArrayNotImplemented (D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count);
	HRESULT GetVectorArray9 (D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count);
		
	HRESULT (CEffect::*pGetVertexShader)(D3DXHANDLE hParameter, CVertexShader** ppVShader);
	HRESULT GetVertexShaderNotImplemented (D3DXHANDLE hParameter, CVertexShader** ppVShader);
	HRESULT GetVertexShader9 (D3DXHANDLE hParameter, CVertexShader** ppVShader);
		

	HRESULT (CEffect::*pApplyParameterBlock)(D3DXHANDLE hParameterBlock);
	HRESULT ApplyParameterBlockNotImplemented (D3DXHANDLE hParameterBlock);
	HRESULT ApplyParameterBlock9 (D3DXHANDLE hParameterBlock);

	HRESULT (CEffect::*pBeginParameterBlock)(void);
	HRESULT BeginParameterBlockNotImplemented (void);
	HRESULT BeginParameterBlock9 (void);

	D3DXHANDLE (CEffect::*pEndParameterBlock)(void);
	D3DXHANDLE EndParameterBlockNotImplemented (void);
	D3DXHANDLE EndParameterBlock9 (void);

	HRESULT (CEffect::*pDeleteParameterBlock)(D3DXHANDLE hParameterBlock);
	HRESULT DeleteParameterBlockNotImplemented (D3DXHANDLE hParameterBlock);
	HRESULT DeleteParameterBlock9 (D3DXHANDLE hParameterBlock);

	HRESULT (CEffect::*pCloneEffect)(CDevice* pTargetDevice, CEffect** ppResultEffect);
	HRESULT CloneEffectNotImplemented (CDevice* pTargetDevice, CEffect** ppResultEffect);
	HRESULT CloneEffect9 (CDevice* pTargetDevice, CEffect** ppResultEffect);

	HRESULT (CEffect::*pSetRawValue)(D3DXHANDLE hParameter, LPCVOID pData, UINT ByteOffset, UINT Bytes);
	HRESULT SetRawValueNotImplemented (D3DXHANDLE hParameter, LPCVOID pData, UINT ByteOffset, UINT Bytes);
	HRESULT SetRawValue9 (D3DXHANDLE hParameter, LPCVOID pData, UINT ByteOffset, UINT Bytes);

	HRESULT (CEffect::*pFindNextValidTechnique)(D3DXHANDLE hTechnique, D3DXHANDLE* pTechnique);
	HRESULT FindNextValidTechniqueNotImplemented (D3DXHANDLE hTechnique, D3DXHANDLE* pTechnique);
	HRESULT FindNextValidTechnique9 (D3DXHANDLE hTechnique, D3DXHANDLE* pTechnique);

	BOOL (CEffect::*pIsParameterUsed)(D3DXHANDLE hParameter,D3DXHANDLE hTechnique);
	BOOL IsParameterUsedNotImplemented (D3DXHANDLE hParameter,D3DXHANDLE hTechnique);
	BOOL IsParameterUsed9 (D3DXHANDLE hParameter,D3DXHANDLE hTechnique);

	HRESULT (CEffect::*pOnLostDevice)(void);
	HRESULT OnLostDeviceNotImplemented (void);
	HRESULT OnLostDevice9 (void);

	HRESULT (CEffect::*pOnResetDevice)(void);
	HRESULT OnResetDeviceNotImplemented (void);
	HRESULT OnResetDevice9 (void);

	HRESULT (CEffect::*pValidateTechnique)(D3DXHANDLE hTechnique);
	HRESULT ValidateTechniqueNotImplemented (D3DXHANDLE hTechnique);
	HRESULT ValidateTechnique9 (D3DXHANDLE hTechnique);


public:
	CEffect();
	~CEffect();

	virtual ULONG WINAPI AddRef (void);
	virtual ULONG WINAPI Release (void);

	bool Init (LPVOID pD3DXEffect, CDevice* pDevice);

	//major functions
	HRESULT Begin (UINT* pPasses, DWORD Flags) { return (this->*pBegin)(pPasses, Flags); }
	HRESULT BeginPass (UINT iPass) { return (this->*pBeginPass)(iPass); }
	HRESULT CommitChanges (void) { return (this->*pCommitChanges)(); }
	HRESULT EndPass (void) { return (this->*pEndPass)(); }
	HRESULT End (void) { return (this->*pEnd)(); }

	HRESULT SetBool (D3DXHANDLE hParameter, BOOL b) { return (this->*pSetBool)(hParameter, b); }
	HRESULT SetBoolArray (D3DXHANDLE hParameter, const BOOL* pB, UINT Count) { return (this->*pSetBoolArray)(hParameter, pB, Count); }
	HRESULT SetFloat (D3DXHANDLE hParameter, FLOAT f) { return (this->*pSetFloat)(hParameter, f); }
	HRESULT SetFloatArray (D3DXHANDLE hParameter, const FLOAT* pf, UINT Count) { return (this->*pSetFloatArray)(hParameter, pf, Count); }
	HRESULT SetInt (D3DXHANDLE hParameter, int n) { return (this->*pSetInt)(hParameter, n); }
	HRESULT SetIntArray (D3DXHANDLE hParameter, const int* pn, UINT Count) { return (this->*pSetIntArray)(hParameter, pn, Count); }
	HRESULT SetMatrix (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix) { return (this->*pSetMatrix)(hParameter, pMatrix); }
	HRESULT SetMatrixArray (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, UINT Count) { return (this->*pSetMatrixArray)(hParameter, pMatrix, Count); }
	HRESULT SetMatrixPointerArray (D3DXHANDLE hParameter, const D3DXMATRIX** ppMatrix, UINT Count) { return (this->*pSetMatrixPointerArray)(hParameter, ppMatrix, Count); }
	HRESULT SetMatrixTranspose (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix) { return (this->*pSetMatrixTranspose)(hParameter, pMatrix); }
	HRESULT SetMatrixTransposeArray (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, UINT Count) { return (this->*pSetMatrixTransposeArray)(hParameter, pMatrix, Count); }
	HRESULT SetMatrixTransposePointerArray (D3DXHANDLE hParameter, const D3DXMATRIX** ppMatrix, UINT Count) { return (this->*pSetMatrixTransposePointerArray)(hParameter, ppMatrix, Count); }
	HRESULT SetPixelShader (D3DXHANDLE hParameter, CPShader* pPShader) { return (this->*pSetPixelShader)(hParameter, pPShader); }
//	HRESULT SetStateManager (LPD3DXEFFECTSTATEMANAGER pManager) { return (this->*pSetStateManager)(pManager); }
	HRESULT SetString (D3DXHANDLE hParameter, const LPCSTR pString) { return (this->*pSetString)(hParameter, pString); }
	HRESULT SetTechnique (D3DXHANDLE hTechnique) { return (this->*pSetTechnique)(hTechnique); }
	HRESULT SetTexture (D3DXHANDLE hParameter, CTexture* pTexture) { return (this->*pSetTexture)(hParameter, pTexture); }
	HRESULT SetCubeTexture (D3DXHANDLE hParameter, CCubeTexture* pCubeTexture) { return (this->*pSetCubeTexture)(hParameter, pCubeTexture); }
	HRESULT SetVolTexture (D3DXHANDLE hParameter, CVolTexture* pVolTexture) { return (this->*pSetVolTexture)(hParameter, pVolTexture); }
	HRESULT SetValue (D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes) { return (this->*pSetValue)(hParameter, pData, Bytes); }
	HRESULT SetVector (D3DXHANDLE hParameter, const D3DXVECTOR4* pVector) { return (this->*pSetVector)(hParameter, pVector); }
	HRESULT SetVectorArray (D3DXHANDLE hParameter, const D3DXVECTOR4* pVector, UINT Count) { return (this->*pSetVectorArray)(hParameter, pVector, Count); }
	HRESULT SetVertexShader (D3DXHANDLE hParameter, CVertexShader* pVertexShader) { return (this->*pSetVertexShader)(hParameter, pVertexShader); }
	HRESULT SetArrayRange (D3DXHANDLE hParameter, UINT uStart, UINT uEnd) { return (this->*pSetArrayRange)(hParameter, uStart, uEnd); }
	
	D3DXHANDLE GetAnnotation (D3DXHANDLE hObject, UINT Index) { return (this->*pGetAnnotation)(hObject, Index); }
	D3DXHANDLE GetAnnotationByName (D3DXHANDLE hObject, LPCSTR pName) { return (this->*pGetAnnotationByName)(hObject, pName); }
	HRESULT GetBool (D3DXHANDLE hParameter, BOOL* pb) { return (this->*pGetBool)(hParameter, pb); }
	HRESULT GetBoolArray (D3DXHANDLE hParameter, BOOL* pB, UINT Count) { return (this->*pGetBoolArray)(hParameter, pB, Count); }
	D3DXHANDLE GetCurrentTechnique (void) { return (this->*pGetCurrentTechnique)(); }
	HRESULT GetDesc (D3DXEFFECT_DESC* pDesc) { return (this->*pGetDesc)(pDesc); }
	HRESULT GetDevice (CDevice** ppDevice) { return (this->*pGetDevice)(ppDevice); }
	HRESULT GetFloat (D3DXHANDLE hParameter, FLOAT* pf) { return (this->*pGetFloat)(hParameter, pf); }
	HRESULT GetFloatArray (D3DXHANDLE hParameter, FLOAT* pf, UINT Count) { return (this->*pGetFloatArray)(hParameter, pf, Count); }
	D3DXHANDLE GetFunction (UINT Index) { return (this->*pGetFunction)(Index); }
	D3DXHANDLE GetFunctionByName (LPCSTR pName) { return (this->*pGetFunctionByName)(pName); }
	HRESULT GetFunctionDesc (D3DXHANDLE hFunction, D3DXFUNCTION_DESC* pDesc) { return (this->*pGetFunctionDesc)(hFunction, pDesc); }
	HRESULT GetInt (D3DXHANDLE hParameter, int* pn) { return (this->*pGetInt)(hParameter, pn); }
	HRESULT GetIntArray (D3DXHANDLE hParameter, int* pn, UINT Count) { return (this->*pGetIntArray)(hParameter, pn, Count); }
	HRESULT GetMatrix (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix) { return (this->*pGetMatrix)(hParameter, pMatrix); }
	HRESULT GetMatrixArray (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count) { return (this->*pGetMatrixArray)(hParameter, pMatrix, Count); }
	HRESULT GetMatrixPointerArray (D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count) { return (this->*pGetMatrixPointerArray)(hParameter, ppMatrix, Count); }
	HRESULT GetMatrixTranspose (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix) { return (this->*pGetMatrixTranspose)(hParameter, pMatrix); }
	HRESULT GetMatrixTransposeArray (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count) { return (this->*pGetMatrixTransposeArray)(hParameter, pMatrix, Count); }
	HRESULT GetMatrixTransposePointerArray (D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count) { return (this->*pGetMatrixTransposePointerArray)(hParameter, ppMatrix, Count); }
	D3DXHANDLE GetParameter (D3DXHANDLE hParent, UINT Index) { return (this->*pGetParameter)(hParent, Index); }
	D3DXHANDLE GetParameterByName (D3DXHANDLE hParent, LPCSTR pName) { return (this->*pGetParameterByName)(hParent, pName); }
	D3DXHANDLE GetParameterBySemantic (D3DXHANDLE hParent, LPCSTR pSemantic) { return (this->*pGetParameterBySemantic)(hParent, pSemantic); }
	HRESULT GetParameterDesc (D3DXHANDLE hParameter, D3DXPARAMETER_DESC* pDesc) { return (this->*pGetParameterDesc)(hParameter, pDesc); }
	D3DXHANDLE GetParameterElement (D3DXHANDLE hParent, UINT ElementIndex) { return (this->*pGetParameterElement)(hParent, ElementIndex); }
	D3DXHANDLE GetPass (D3DXHANDLE hTechnique, UINT Index) { return (this->*pGetPass)(hTechnique, Index); }
	D3DXHANDLE GetPassByName (D3DXHANDLE hTechnique, LPCSTR pName) { return (this->*pGetPassByName)(hTechnique, pName); }
	HRESULT GetPassDesc (D3DXHANDLE hPass, D3DXPASS_DESC* pDesc) { return (this->*pGetPassDesc)(hPass, pDesc); }
	HRESULT GetPixelShader (D3DXHANDLE hParameter, CPShader** ppPShader) { return (this->*pGetPixelShader)(hParameter, ppPShader); }
	HRESULT GetPool (LPD3DXEFFECTPOOL* ppPool) { return (this->*pGetPool)(ppPool); }
//	HRESULT GetStateManager (LPD3DXEFFECTSTATEMANAGER* ppManager) { return (this->*pGetStateManager)(ppManager); }
	HRESULT GetString (D3DXHANDLE hParameter, LPCSTR* ppString) { return (this->*pGetString)(hParameter, ppString); }
	D3DXHANDLE GetTechnique (UINT Index) { return (this->*pGetTechnique)(Index); }
	D3DXHANDLE GetTechniqueByName (LPCSTR pName) { return (this->*pGetTechniqueByName)(pName); }
	HRESULT GetTechniqueDesc (D3DXHANDLE hTechnique, D3DXTECHNIQUE_DESC* pDesc) { return (this->*pGetTechniqueDesc)(hTechnique, pDesc); }
	HRESULT GetTexture (D3DXHANDLE hParameter, CTexture** ppTexture) { return (this->*pGetTexture)(hParameter, ppTexture); }
	HRESULT GetCubeTexture (D3DXHANDLE hParameter, CCubeTexture** ppCubeTexture) { return (this->*pGetCubeTexture)(hParameter, ppCubeTexture); }
	HRESULT GetVolTexture (D3DXHANDLE hParameter, CVolTexture** ppVolTexture) { return (this->*pGetVolTexture)(hParameter, ppVolTexture); }
	HRESULT GetValue (D3DXHANDLE hParameter, LPVOID pData, UINT Bytes) { return (this->*pGetValue)(hParameter, pData, Bytes); }
	HRESULT GetVector (D3DXHANDLE hParameter, D3DXVECTOR4* pVector) { return (this->*pGetVector)(hParameter, pVector); }
	HRESULT GetVectorArray (D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count) { return (this->*pGetVectorArray)(hParameter, pVector, Count); }
	HRESULT GetVertexShader (D3DXHANDLE hParameter, CVertexShader** ppVShader) { return (this->*pGetVertexShader)(hParameter, ppVShader); }
		
	HRESULT ApplyParameterBlock (D3DXHANDLE hParameterBlock) { return (this->*pApplyParameterBlock)(hParameterBlock); }
	HRESULT BeginParameterBlock (void) { return (this->*pBeginParameterBlock)(); }
	D3DXHANDLE EndParameterBlock (void) { return (this->*pEndParameterBlock)(); }
	HRESULT DeleteParameterBlock (D3DXHANDLE hParameterBlock) { return (this->*pDeleteParameterBlock)(hParameterBlock); }
	HRESULT CloneEffect (CDevice* pTargetDevice, CEffect** ppResultEffect) { return (this->*pCloneEffect)(pTargetDevice, ppResultEffect); }
	HRESULT SetRawValue (D3DXHANDLE hParameter, LPCVOID pData, UINT ByteOffset, UINT Bytes) { return (this->*pSetRawValue)(hParameter, pData, ByteOffset, Bytes); }
	HRESULT FindNextValidTechnique (D3DXHANDLE hTechnique, D3DXHANDLE* pTechnique) { return (this->*pFindNextValidTechnique)(hTechnique, pTechnique); }
	BOOL IsParameterUsed (D3DXHANDLE hParameter,D3DXHANDLE hTechnique) { return (this->*pIsParameterUsed)(hParameter, hTechnique); }
	HRESULT OnLostDevice (void) { return (this->*pOnLostDevice)(); }
	HRESULT OnResetDevice (void) { return (this->*pOnResetDevice)(); }
	HRESULT ValidateTechnique (D3DXHANDLE hTechnique) { return (this->*pValidateTechnique)(hTechnique); }
};



class CnEffect : public CObject
{
private:

	CEffect**	m_pEffects;
	UINT		m_uDevices;

	bool SkipDevice (UINT uDevice, DWORD dwFlags);

public:
	CnEffect();
	~CnEffect();
	virtual bool Init (UINT uDevices);	
	virtual bool SetEffect (CEffect* pEffect, UINT n);
	virtual bool GetEffect (CEffect** ppEffect, UINT n);
	virtual bool CloneEffect (UINT sourceEffect, CDevice* pTargetDevice, CEffect** ppResultEffect);

	virtual bool Begin (UINT* pPasses, DWORD Flags, DWORD dwFlags = CD3D_ALL);
	virtual bool BeginPass (UINT iPass, DWORD dwFlags = CD3D_ALL);
	virtual bool CommitChanges (DWORD dwFlags = CD3D_ALL);
	virtual bool EndPass (DWORD dwFlags = CD3D_ALL);
	virtual bool End (DWORD dwFlags = CD3D_ALL);

	virtual bool SetBool (D3DXHANDLE hParameter, BOOL b, DWORD dwFlags = CD3D_ALL);
	virtual bool SetBoolArray (D3DXHANDLE hParameter, const BOOL* pB, UINT Count, DWORD dwFlags = CD3D_ALL);
	virtual bool SetFloat (D3DXHANDLE hParameter, FLOAT f, DWORD dwFlags = CD3D_ALL);
	virtual bool SetFloatArray (D3DXHANDLE hParameter, const FLOAT* pf, UINT Count, DWORD dwFlags = CD3D_ALL);
	virtual bool SetInt (D3DXHANDLE hParameter, int n, DWORD dwFlags = CD3D_ALL);
	virtual bool SetIntArray (D3DXHANDLE hParameter, const int* pn, UINT Count, DWORD dwFlags = CD3D_ALL);
	virtual bool SetMatrix (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, DWORD dwFlags = CD3D_ALL);
	virtual bool SetMatrixArray (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, UINT Count, DWORD dwFlags = CD3D_ALL);
	virtual bool SetMatrixPointerArray (D3DXHANDLE hParameter, const D3DXMATRIX** ppMatrix, UINT Count, DWORD dwFlags = CD3D_ALL);
	virtual bool SetMatrixTranspose (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, DWORD dwFlags = CD3D_ALL);
	virtual bool SetMatrixTransposeArray (D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix, UINT Count, DWORD dwFlags = CD3D_ALL);
	virtual bool SetMatrixTransposePointerArray (D3DXHANDLE hParameter, const D3DXMATRIX** ppMatrix, UINT Count, DWORD dwFlags = CD3D_ALL);
	virtual bool SetPixelShader (D3DXHANDLE hParameter, CnPixelShader* pPShader, DWORD dwFlags = CD3D_ALL);
//	virtual bool SetStateManager (LPD3DXEFFECTSTATEMANAGER pManager, DWORD dwFlags = CD3D_ALL);
	virtual bool SetString (D3DXHANDLE hParameter, const LPCSTR pString, DWORD dwFlags = CD3D_ALL);
	virtual bool SetTechnique (D3DXHANDLE hTechnique, DWORD dwFlags = CD3D_ALL);
	virtual bool SetTechniqueByIndex (UINT index, DWORD dwFlags = CD3D_ALL);
	virtual bool SetTechniqueByName (LPCSTR pName, DWORD dwFlags = CD3D_ALL);
	virtual bool SetTexture (D3DXHANDLE hParameter, CnTexture* pTexture, DWORD dwFlags = CD3D_ALL);
	virtual bool SetCubeTexture (D3DXHANDLE hParameter, CnCubeTexture* pCubeTexture, DWORD dwFlags = CD3D_ALL);
	virtual bool SetVolTexture (D3DXHANDLE hParameter, CnVolTexture* pVolTexture, DWORD dwFlags = CD3D_ALL);
	virtual bool SetValue (D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes, DWORD dwFlags = CD3D_ALL);
	virtual bool SetVector (D3DXHANDLE hParameter, const D3DXVECTOR4* pVector, DWORD dwFlags = CD3D_ALL);
	virtual bool SetVectorArray (D3DXHANDLE hParameter, const D3DXVECTOR4* pVector, UINT Count, DWORD dwFlags = CD3D_ALL);
	virtual bool SetVertexShader (D3DXHANDLE hParameter, CnVertexShader* pVertexShader, DWORD dwFlags = CD3D_ALL);
	virtual bool SetArrayRange (D3DXHANDLE hParameter, UINT Start, UINT End, DWORD dwFlags = CD3D_ALL);

	virtual D3DXHANDLE GetAnnotation (D3DXHANDLE hObject, UINT Index, DWORD dwFlags = CD3D_ALL);
	virtual D3DXHANDLE GetAnnotationByName (D3DXHANDLE hObject, LPCSTR pName, DWORD dwFlags = CD3D_ALL);
	virtual bool GetBool (D3DXHANDLE hParameter, BOOL* pb, DWORD dwFlags = CD3D_ALL);
	virtual bool GetBoolArray (D3DXHANDLE hParameter, BOOL* pB, UINT Count, DWORD dwFlags = CD3D_ALL);
	virtual D3DXHANDLE GetCurrentTechnique (DWORD dwFlags = CD3D_ALL);
	virtual bool GetDesc (D3DXEFFECT_DESC* pDesc, DWORD dwFlags = CD3D_ALL);
	virtual bool GetDevice (CDevice** ppDevice, DWORD dwFlags = CD3D_ALL);
	virtual bool GetFloat (D3DXHANDLE hParameter, FLOAT* pf, DWORD dwFlags = CD3D_ALL);
	virtual bool GetFloatArray (D3DXHANDLE hParameter, FLOAT* pf, UINT Count, DWORD dwFlags = CD3D_ALL);
	virtual D3DXHANDLE GetFunction (UINT Index, DWORD dwFlags = CD3D_ALL);
	virtual D3DXHANDLE GetFunctionByName (LPCSTR pName, DWORD dwFlags = CD3D_ALL);
	virtual bool GetFunctionDesc (D3DXHANDLE hFunction, D3DXFUNCTION_DESC* pDesc, DWORD dwFlags = CD3D_ALL);
	virtual bool GetInt (D3DXHANDLE hParameter, int* pn, DWORD dwFlags = CD3D_ALL);
	virtual bool GetIntArray (D3DXHANDLE hParameter, int* pn, UINT Count, DWORD dwFlags = CD3D_ALL);
	virtual bool GetMatrix (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, DWORD dwFlags = CD3D_ALL);
	virtual bool GetMatrixArray (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count, DWORD dwFlags = CD3D_ALL);
	virtual bool GetMatrixPointerArray (D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count, DWORD dwFlags = CD3D_ALL);
	virtual bool GetMatrixTranspose (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, DWORD dwFlags = CD3D_ALL);
	virtual bool GetMatrixTransposeArray (D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count, DWORD dwFlags = CD3D_ALL);
	virtual bool GetMatrixTransposePointerArray (D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count, DWORD dwFlags = CD3D_ALL);
	virtual D3DXHANDLE GetParameter (D3DXHANDLE hParent, UINT Index, DWORD dwFlags = CD3D_ALL);
	virtual D3DXHANDLE GetParameterByName (D3DXHANDLE hParent, LPCSTR pName, DWORD dwFlags = CD3D_ALL);
	virtual D3DXHANDLE GetParameterBySemantic (D3DXHANDLE hParent, LPCSTR pSemantic, DWORD dwFlags = CD3D_ALL);
	virtual bool GetParameterDesc (D3DXHANDLE hParameter, D3DXPARAMETER_DESC* pDesc, DWORD dwFlags = CD3D_ALL);
	virtual D3DXHANDLE GetParameterElement (D3DXHANDLE hParent, UINT ElementIndex, DWORD dwFlags = CD3D_ALL);
	virtual D3DXHANDLE GetPass (D3DXHANDLE hTechnique, UINT Index, DWORD dwFlags = CD3D_ALL);
	virtual D3DXHANDLE GetPassByName (D3DXHANDLE hTechnique, LPCSTR pName, DWORD dwFlags = CD3D_ALL);
	virtual bool GetPassDesc (D3DXHANDLE hPass, D3DXPASS_DESC* pDesc, DWORD dwFlags = CD3D_ALL);
	virtual bool GetPixelShader (D3DXHANDLE hParameter, CnPixelShader** ppPShader, DWORD dwFlags = CD3D_ALL);
	virtual bool GetPool (LPD3DXEFFECTPOOL* ppPool, DWORD dwFlags = CD3D_ALL);
//	virtual bool GetStateManager (LPD3DXEFFECTSTATEMANAGER* ppManager, DWORD dwFlags = CD3D_ALL);
	virtual bool GetString (D3DXHANDLE hParameter, LPCSTR* ppString, DWORD dwFlags = CD3D_ALL);
	virtual D3DXHANDLE GetTechnique (UINT Index, DWORD dwFlags = CD3D_ALL);
	virtual D3DXHANDLE GetTechniqueByName (LPCSTR pName, DWORD dwFlags = CD3D_ALL);
	virtual bool GetTechniqueDesc (D3DXHANDLE hTechnique, D3DXTECHNIQUE_DESC* pDesc, DWORD dwFlags = CD3D_ALL);
	virtual bool GetTexture (D3DXHANDLE hParameter, CnTexture** ppTexture, DWORD dwFlags = CD3D_ALL);
	virtual bool GetCubeTexture (D3DXHANDLE hParameter, CnCubeTexture** ppCubeTexture, DWORD dwFlags = CD3D_ALL);
	virtual bool GetVolTexture (D3DXHANDLE hParameter, CnVolTexture** ppVolTexture, DWORD dwFlags = CD3D_ALL);
	virtual bool GetValue (D3DXHANDLE hParameter, LPVOID pData, UINT Bytes, DWORD dwFlags = CD3D_ALL);
	virtual bool GetVector (D3DXHANDLE hParameter, D3DXVECTOR4* pVector, DWORD dwFlags = CD3D_ALL);
	virtual bool GetVectorArray (D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count, DWORD dwFlags = CD3D_ALL);
	virtual bool GetVertexShader (D3DXHANDLE hParameter, CnVertexShader** ppVShader, DWORD dwFlags = CD3D_ALL);
		
	virtual bool ApplyParameterBlock (D3DXHANDLE hParameterBlock, DWORD dwFlags = CD3D_ALL);
	virtual bool BeginParameterBlock (DWORD dwFlags = CD3D_ALL);
	virtual D3DXHANDLE EndParameterBlock (DWORD dwFlags = CD3D_ALL);
	virtual bool DeleteParameterBlock (D3DXHANDLE hParameterBlock, DWORD dwFlags = CD3D_ALL);
	virtual bool SetRawValue (D3DXHANDLE hParameter, LPCVOID pData, UINT ByteOffset, UINT Bytes, DWORD dwFlags = CD3D_ALL);
	virtual bool FindNextValidTechnique (D3DXHANDLE hTechnique, D3DXHANDLE* pTechnique, DWORD dwFlags = CD3D_ALL);
	virtual BOOL IsParameterUsed (D3DXHANDLE hParameter, D3DXHANDLE hTechnique, DWORD dwFlags = CD3D_ALL);
	virtual bool OnLostDevice (DWORD dwFlags = CD3D_ALL);
	virtual bool OnResetDevice (DWORD dwFlags = CD3D_ALL);
	virtual bool ValidateTechnique (D3DXHANDLE hTechnique, DWORD dwFlags = CD3D_ALL);
	virtual bool ValidateTechniqueByIndex (UINT index, DWORD dwFlags = CD3D_ALL);
};

#endif