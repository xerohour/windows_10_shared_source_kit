#ifndef __CD3DX_H__
#define __CD3DX_H__
	
#include "Framework.h"

extern HINSTANCE	g_hD3DX;
extern float		g_fD3DX;


#define D3DXHANDLEVERSION(result, type, interfaceptr, function) \
	{\
		if(g_fD3DX >= 9.29f)\
			result ((type##93*)interfaceptr)->function;\
		else if(g_fD3DX < 9.29f && g_fD3DX >= 9.19f)\
			result ((type##92*)interfaceptr)->function;\
		else if (g_fD3DX < 9.19f && g_fD3DX >= 9.09f)\
			result ((type##91*)interfaceptr)->function;\
		else\
			result ((type##90*)interfaceptr)->function;\
	}

// non-inline
#ifdef __cplusplus
extern "C" {
#endif

	// --------------------------------------------------------------------------------------
	//  Versioned D3DX functions & types
	//		These are only for functions used by D3D(non-D3DX) apps
	//		that have changed function signatures across D3DX versions.
	// --------------------------------------------------------------------------------------
	HRESULT WINAPI D3DXAssembleShader8(LPCVOID pSrcData, UINT SrcDataLen, DWORD Flags, LPD3DXBUFFER* ppConstants, LPD3DXBUFFER* ppCompiledShader, LPD3DXBUFFER* ppCompilationErrors);
    HRESULT WINAPI D3DXAssembleShader9(LPCVOID pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrors);
	HRESULT WINAPI D3DXCreateFontIndirect8And90(LPVOID pDevice, CONST LOGFONT* pLogFont, LPD3DXFONT* ppFont);

	DECLARE_INTERFACE_(ID3DXFont90, IUnknown)
	{
		// IUnknown
		STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
		STDMETHOD_(ULONG, AddRef)(THIS) PURE;
		STDMETHOD_(ULONG, Release)(THIS) PURE;

		// ID3DXFont
		STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE9* ppDevice) PURE;
		STDMETHOD(GetLogFont)(THIS_ LOGFONT* pLogFont) PURE;

		STDMETHOD(Begin)(THIS) PURE;
		STDMETHOD_(INT, DrawTextA)(THIS_ LPCSTR  pString, INT Count, LPRECT pRect, DWORD Format, D3DCOLOR Color) PURE;
		STDMETHOD_(INT, DrawTextW)(THIS_ LPCWSTR pString, INT Count, LPRECT pRect, DWORD Format, D3DCOLOR Color) PURE;
		STDMETHOD(End)(THIS) PURE;

		STDMETHOD(OnLostDevice)(THIS) PURE;
		STDMETHOD(OnResetDevice)(THIS) PURE;
	};

	DECLARE_INTERFACE_(ID3DXFont91, IUnknown)
	{
		// IUnknown
		STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
		STDMETHOD_(ULONG, AddRef)(THIS) PURE;
		STDMETHOD_(ULONG, Release)(THIS) PURE;

		// ID3DXFont91
		STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE9 *ppDevice) PURE;
		STDMETHOD(GetDescA)(THIS_ D3DXFONT_DESCA *pDesc) PURE;
		STDMETHOD(GetDescW)(THIS_ D3DXFONT_DESCW *pDesc) PURE;

		STDMETHOD_(HDC, GetDC)(THIS) PURE;
		STDMETHOD(GetGlyphData)(THIS_ UINT Glyph, LPDIRECT3DTEXTURE9 *ppTexture, RECT *pBlackBox, POINT *pCellInc) PURE;

		STDMETHOD(PreloadCharacters)(THIS_ UINT First, UINT Last) PURE;
		STDMETHOD(PreloadGlyphs)(THIS_ UINT First, UINT Last) PURE;
		STDMETHOD(PreloadTextA)(THIS_ LPCSTR pString, INT Count) PURE;
		STDMETHOD(PreloadTextW)(THIS_ LPCWSTR pString, INT Count) PURE;

		STDMETHOD_(INT, DrawTextA)(THIS_ LPD3DXSPRITE pSprite, LPCSTR pString, INT Count, LPRECT pRect, DWORD Format, D3DCOLOR Color) PURE;
		STDMETHOD_(INT, DrawTextW)(THIS_ LPD3DXSPRITE pSprite, LPCWSTR pString, INT Count, LPRECT pRect, DWORD Format, D3DCOLOR Color) PURE;

		STDMETHOD(OnLostDevice)(THIS) PURE;
		STDMETHOD(OnResetDevice)(THIS) PURE;

	#ifdef __cplusplus
	#ifdef UNICODE
		HRESULT GetDesc(D3DXFONT_DESCW *pDesc) { return GetDescW(pDesc); }
		HRESULT PreloadText(LPCWSTR pString, INT Count) { return PreloadTextW(pString, Count); }
	#else
		HRESULT GetDesc(D3DXFONT_DESCA *pDesc) { return GetDescA(pDesc); }
		HRESULT PreloadText(LPCSTR pString, INT Count) { return PreloadTextA(pString, Count); }
	#endif
	#endif //__cplusplus
	};
	DECLARE_INTERFACE_(ID3DXFont92, IUnknown)
	{
		// IUnknown
		STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
		STDMETHOD_(ULONG, AddRef)(THIS) PURE;
		STDMETHOD_(ULONG, Release)(THIS) PURE;

		// ID3DXFont
		STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE9 *ppDevice) PURE;
		STDMETHOD(GetDescA)(THIS_ D3DXFONT_DESCA *pDesc) PURE;
		STDMETHOD(GetDescW)(THIS_ D3DXFONT_DESCW *pDesc) PURE;
		STDMETHOD_(BOOL, GetTextMetricsA)(THIS_ TEXTMETRICA *pTextMetrics) PURE;
		STDMETHOD_(BOOL, GetTextMetricsW)(THIS_ TEXTMETRICW *pTextMetrics) PURE;

		STDMETHOD_(HDC, GetDC)(THIS) PURE;
		STDMETHOD(GetGlyphData)(THIS_ UINT Glyph, LPDIRECT3DTEXTURE9 *ppTexture, RECT *pBlackBox, POINT *pCellInc) PURE;

		STDMETHOD(PreloadCharacters)(THIS_ UINT First, UINT Last) PURE;
		STDMETHOD(PreloadGlyphs)(THIS_ UINT First, UINT Last) PURE;
		STDMETHOD(PreloadTextA)(THIS_ LPCSTR pString, INT Count) PURE;
		STDMETHOD(PreloadTextW)(THIS_ LPCWSTR pString, INT Count) PURE;

		STDMETHOD_(INT, DrawTextA)(THIS_ LPD3DXSPRITE pSprite, LPCSTR pString, INT Count, LPRECT pRect, DWORD Format, D3DCOLOR Color) PURE;
		STDMETHOD_(INT, DrawTextW)(THIS_ LPD3DXSPRITE pSprite, LPCWSTR pString, INT Count, LPRECT pRect, DWORD Format, D3DCOLOR Color) PURE;

		STDMETHOD(OnLostDevice)(THIS) PURE;
		STDMETHOD(OnResetDevice)(THIS) PURE;

	#ifdef __cplusplus
	#ifdef UNICODE
		HRESULT GetDesc(D3DXFONT_DESCW *pDesc) { return GetDescW(pDesc); }
		HRESULT PreloadText(LPCWSTR pString, INT Count) { return PreloadTextW(pString, Count); }
	#else
		HRESULT GetDesc(D3DXFONT_DESCA *pDesc) { return GetDescA(pDesc); }
		HRESULT PreloadText(LPCSTR pString, INT Count) { return PreloadTextA(pString, Count); }
	#endif
	#endif //__cplusplus
	};
	
	typedef ID3DXFont92 ID3DXFont93;

	DECLARE_INTERFACE_(ID3DXEffect90, IUnknown)
	{
		// ID3DXBaseEffect
		STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
		STDMETHOD_(ULONG, AddRef)(THIS) PURE;
		STDMETHOD_(ULONG, Release)(THIS) PURE;

		// Descs
		STDMETHOD(GetDesc)(THIS_ VOID* pDesc) PURE;
		STDMETHOD(GetParameterDesc)(THIS_ D3DXHANDLE hParameter, VOID* pDesc) PURE;
		STDMETHOD(GetTechniqueDesc)(THIS_ D3DXHANDLE hTechnique, VOID* pDesc) PURE;
		STDMETHOD(GetPassDesc)(THIS_ D3DXHANDLE hPass, VOID* pDesc) PURE;
		STDMETHOD(GetFunctionDesc)(THIS_ D3DXHANDLE hShader, VOID* pDesc) PURE;

		// Handle operations
		STDMETHOD_(D3DXHANDLE, GetParameter)(THIS_ D3DXHANDLE hParameter, UINT Index) PURE;
		STDMETHOD_(D3DXHANDLE, GetParameterByName)(THIS_ D3DXHANDLE hParameter, LPCSTR pName) PURE;
		STDMETHOD_(D3DXHANDLE, GetParameterBySemantic)(THIS_ D3DXHANDLE hParameter, LPCSTR pSemantic) PURE;
		STDMETHOD_(D3DXHANDLE, GetParameterElement)(THIS_ D3DXHANDLE hParameter, UINT Index) PURE;
		STDMETHOD_(D3DXHANDLE, GetTechnique)(THIS_ UINT Index) PURE;
		STDMETHOD_(D3DXHANDLE, GetTechniqueByName)(THIS_ LPCSTR pName) PURE;
		STDMETHOD_(D3DXHANDLE, GetPass)(THIS_ D3DXHANDLE hTechnique, UINT Index) PURE;
		STDMETHOD_(D3DXHANDLE, GetPassByName)(THIS_ D3DXHANDLE hTechnique, LPCSTR pName) PURE;
		STDMETHOD_(D3DXHANDLE, GetFunction)(THIS_ UINT Index);
		STDMETHOD_(D3DXHANDLE, GetFunctionByName)(THIS_ LPCSTR pName);
		STDMETHOD_(D3DXHANDLE, GetAnnotation)(THIS_ D3DXHANDLE hObject, UINT Index) PURE;
		STDMETHOD_(D3DXHANDLE, GetAnnotationByName)(THIS_ D3DXHANDLE hObject, LPCSTR pName) PURE;

		// Get/Set Parameters
		STDMETHOD(SetValue)(THIS_ D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes) PURE;
		STDMETHOD(GetValue)(THIS_ D3DXHANDLE hParameter, LPVOID pData, UINT Bytes) PURE;
		STDMETHOD(SetBool)(THIS_ D3DXHANDLE hParameter, BOOL b) PURE;
		STDMETHOD(GetBool)(THIS_ D3DXHANDLE hParameter, BOOL* pb) PURE;
		STDMETHOD(SetBoolArray)(THIS_ D3DXHANDLE hParameter, CONST BOOL* pb, UINT Count) PURE;
		STDMETHOD(GetBoolArray)(THIS_ D3DXHANDLE hParameter, BOOL* pb, UINT Count) PURE;
		STDMETHOD(SetInt)(THIS_ D3DXHANDLE hParameter, INT n) PURE;
		STDMETHOD(GetInt)(THIS_ D3DXHANDLE hParameter, INT* pn) PURE;
		STDMETHOD(SetIntArray)(THIS_ D3DXHANDLE hParameter, CONST INT* pn, UINT Count) PURE;
		STDMETHOD(GetIntArray)(THIS_ D3DXHANDLE hParameter, INT* pn, UINT Count) PURE;
		STDMETHOD(SetFloat)(THIS_ D3DXHANDLE hParameter, FLOAT f) PURE;
		STDMETHOD(GetFloat)(THIS_ D3DXHANDLE hParameter, FLOAT* pf) PURE;
		STDMETHOD(SetFloatArray)(THIS_ D3DXHANDLE hParameter, CONST FLOAT* pf, UINT Count) PURE;
		STDMETHOD(GetFloatArray)(THIS_ D3DXHANDLE hParameter, FLOAT* pf, UINT Count) PURE;
		STDMETHOD(SetVector)(THIS_ D3DXHANDLE hParameter, CONST D3DXVECTOR4* pVector) PURE;
		STDMETHOD(GetVector)(THIS_ D3DXHANDLE hParameter, D3DXVECTOR4* pVector) PURE;
		STDMETHOD(SetVectorArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXVECTOR4* pVector, UINT Count) PURE;
		STDMETHOD(GetVectorArray)(THIS_ D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count) PURE;
		STDMETHOD(SetMatrix)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix) PURE;
		STDMETHOD(GetMatrix)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX* pMatrix) PURE;
		STDMETHOD(SetMatrixArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count) PURE;
		STDMETHOD(GetMatrixArray)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count) PURE;
		STDMETHOD(SetMatrixPointerArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count) PURE;
		STDMETHOD(GetMatrixPointerArray)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count) PURE;
		STDMETHOD(SetMatrixTranspose)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix) PURE;
		STDMETHOD(GetMatrixTranspose)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX* pMatrix) PURE;
		STDMETHOD(SetMatrixTransposeArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count) PURE;
		STDMETHOD(GetMatrixTransposeArray)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count) PURE;
		STDMETHOD(SetMatrixTransposePointerArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count) PURE;
		STDMETHOD(GetMatrixTransposePointerArray)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count) PURE;
		STDMETHOD(SetString)(THIS_ D3DXHANDLE hParameter, LPCSTR pString) PURE;
		STDMETHOD(GetString)(THIS_ D3DXHANDLE hParameter, LPCSTR* ppString) PURE;
		STDMETHOD(SetTexture)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture) PURE;
		STDMETHOD(GetTexture)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 *ppTexture) PURE;
		STDMETHOD(SetPixelShader)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DPIXELSHADER9 pPShader) PURE;
		STDMETHOD(GetPixelShader)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DPIXELSHADER9 *ppPShader) PURE;
		STDMETHOD(SetVertexShader)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DVERTEXSHADER9 pVShader) PURE;
		STDMETHOD(GetVertexShader)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DVERTEXSHADER9 *ppVShader) PURE;
		// ID3DXBaseEffect

		// Pool
		STDMETHOD(GetPool)(THIS_ LPD3DXEFFECTPOOL* ppPool) PURE;

		// Selecting and setting a technique
		STDMETHOD(SetTechnique)(THIS_ D3DXHANDLE hTechnique) PURE;
		STDMETHOD_(D3DXHANDLE, GetCurrentTechnique)(THIS) PURE;
		STDMETHOD(ValidateTechnique)(THIS_ D3DXHANDLE hTechnique) PURE;
		STDMETHOD(FindNextValidTechnique)(THIS_ D3DXHANDLE hTechnique, D3DXHANDLE *pTechnique) PURE;
		STDMETHOD_(BOOL, IsParameterUsed)(THIS_ D3DXHANDLE hParameter, D3DXHANDLE hTechnique) PURE;

		// Using current technique
		STDMETHOD(Begin)(THIS_ UINT *pPasses, DWORD Flags) PURE;
		STDMETHOD(Pass)(THIS_ UINT Pass) PURE;
		STDMETHOD(End)(THIS) PURE;

		// Managing D3D Device
		STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE9* ppDevice) PURE;
		STDMETHOD(OnLostDevice)(THIS) PURE;
		STDMETHOD(OnResetDevice)(THIS) PURE;

		// Cloning
		STDMETHOD(CloneEffect)(THIS_ LPDIRECT3DDEVICE9 pDevice, LPD3DXEFFECT* ppEffect) PURE;
	};

	DECLARE_INTERFACE_(ID3DXEffect91, IUnknown)
	{
		// ID3DXBaseEffect
		STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
		STDMETHOD_(ULONG, AddRef)(THIS) PURE;
		STDMETHOD_(ULONG, Release)(THIS) PURE;

		// Descs
		STDMETHOD(GetDesc)(THIS_ VOID* pDesc) PURE;
		STDMETHOD(GetParameterDesc)(THIS_ LPCSTR hParameter, VOID* pDesc) PURE;
		STDMETHOD(GetTechniqueDesc)(THIS_ LPCSTR hTechnique, VOID* pDesc) PURE;
		STDMETHOD(GetPassDesc)(THIS_ LPCSTR hPass, VOID* pDesc) PURE;
		STDMETHOD(GetFunctionDesc)(THIS_ LPCSTR hShader, VOID* pDesc) PURE;

		// Handle operations
		STDMETHOD_(LPCSTR, GetParameter)(THIS_ LPCSTR hParameter, UINT Index) PURE;
		STDMETHOD_(LPCSTR, GetParameterByName)(THIS_ LPCSTR hParameter, LPCSTR pName) PURE;
		STDMETHOD_(LPCSTR, GetParameterBySemantic)(THIS_ LPCSTR hParameter, LPCSTR pSemantic) PURE;
		STDMETHOD_(LPCSTR, GetParameterElement)(THIS_ LPCSTR hParameter, UINT Index) PURE;
		STDMETHOD_(LPCSTR, GetTechnique)(THIS_ UINT Index) PURE;
		STDMETHOD_(LPCSTR, GetTechniqueByName)(THIS_ LPCSTR pName) PURE;
		STDMETHOD_(LPCSTR, GetPass)(THIS_ LPCSTR hTechnique, UINT Index) PURE;
		STDMETHOD_(LPCSTR, GetPassByName)(THIS_ LPCSTR hTechnique, LPCSTR pName) PURE;
		STDMETHOD_(LPCSTR, GetFunction)(THIS_ UINT Index) PURE;
		STDMETHOD_(LPCSTR, GetFunctionByName)(THIS_ LPCSTR pName) PURE;
		STDMETHOD_(LPCSTR, GetAnnotation)(THIS_ LPCSTR hObject, UINT Index) PURE;
		STDMETHOD_(LPCSTR, GetAnnotationByName)(THIS_ LPCSTR hObject, LPCSTR pName) PURE;

		// Get/Set Parameters
		STDMETHOD(SetValue)(THIS_ LPCSTR hParameter, LPCVOID pData, UINT Bytes) PURE;
		STDMETHOD(GetValue)(THIS_ LPCSTR hParameter, LPVOID pData, UINT Bytes) PURE;
		STDMETHOD(SetBool)(THIS_ LPCSTR hParameter, BOOL b) PURE;
		STDMETHOD(GetBool)(THIS_ LPCSTR hParameter, BOOL* pb) PURE;
		STDMETHOD(SetBoolArray)(THIS_ LPCSTR hParameter, CONST BOOL* pb, UINT Count) PURE;
		STDMETHOD(GetBoolArray)(THIS_ LPCSTR hParameter, BOOL* pb, UINT Count) PURE;
		STDMETHOD(SetInt)(THIS_ LPCSTR hParameter, INT n) PURE;
		STDMETHOD(GetInt)(THIS_ LPCSTR hParameter, INT* pn) PURE;
		STDMETHOD(SetIntArray)(THIS_ LPCSTR hParameter, CONST INT* pn, UINT Count) PURE;
		STDMETHOD(GetIntArray)(THIS_ LPCSTR hParameter, INT* pn, UINT Count) PURE;
		STDMETHOD(SetFloat)(THIS_ LPCSTR hParameter, FLOAT f) PURE;
		STDMETHOD(GetFloat)(THIS_ LPCSTR hParameter, FLOAT* pf) PURE;
		STDMETHOD(SetFloatArray)(THIS_ LPCSTR hParameter, CONST FLOAT* pf, UINT Count) PURE;
		STDMETHOD(GetFloatArray)(THIS_ LPCSTR hParameter, FLOAT* pf, UINT Count) PURE;
		STDMETHOD(SetVector)(THIS_ LPCSTR hParameter, CONST D3DXVECTOR4* pVector) PURE;
		STDMETHOD(GetVector)(THIS_ LPCSTR hParameter, D3DXVECTOR4* pVector) PURE;
		STDMETHOD(SetVectorArray)(THIS_ LPCSTR hParameter, CONST D3DXVECTOR4* pVector, UINT Count) PURE;
		STDMETHOD(GetVectorArray)(THIS_ LPCSTR hParameter, D3DXVECTOR4* pVector, UINT Count) PURE;
		STDMETHOD(SetMatrix)(THIS_ LPCSTR hParameter, CONST D3DXMATRIX* pMatrix) PURE;
		STDMETHOD(GetMatrix)(THIS_ LPCSTR hParameter, D3DXMATRIX* pMatrix) PURE;
		STDMETHOD(SetMatrixArray)(THIS_ LPCSTR hParameter, CONST D3DXMATRIX* pMatrix, UINT Count) PURE;
		STDMETHOD(GetMatrixArray)(THIS_ LPCSTR hParameter, D3DXMATRIX* pMatrix, UINT Count) PURE;
		STDMETHOD(SetMatrixPointerArray)(THIS_ LPCSTR hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count) PURE;
		STDMETHOD(GetMatrixPointerArray)(THIS_ LPCSTR hParameter, D3DXMATRIX** ppMatrix, UINT Count) PURE;
		STDMETHOD(SetMatrixTranspose)(THIS_ LPCSTR hParameter, CONST D3DXMATRIX* pMatrix) PURE;
		STDMETHOD(GetMatrixTranspose)(THIS_ LPCSTR hParameter, D3DXMATRIX* pMatrix) PURE;
		STDMETHOD(SetMatrixTransposeArray)(THIS_ LPCSTR hParameter, CONST D3DXMATRIX* pMatrix, UINT Count) PURE;
		STDMETHOD(GetMatrixTransposeArray)(THIS_ LPCSTR hParameter, D3DXMATRIX* pMatrix, UINT Count) PURE;
		STDMETHOD(SetMatrixTransposePointerArray)(THIS_ LPCSTR hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count) PURE;
		STDMETHOD(GetMatrixTransposePointerArray)(THIS_ LPCSTR hParameter, D3DXMATRIX** ppMatrix, UINT Count) PURE;
		STDMETHOD(SetString)(THIS_ LPCSTR hParameter, LPCSTR pString) PURE;
		STDMETHOD(GetString)(THIS_ LPCSTR hParameter, LPCSTR* ppString) PURE;
		STDMETHOD(SetTexture)(THIS_ LPCSTR hParameter, LPDIRECT3DBASETEXTURE9 pTexture) PURE;
		STDMETHOD(GetTexture)(THIS_ LPCSTR hParameter, LPDIRECT3DBASETEXTURE9 *ppTexture) PURE;
		STDMETHOD(SetPixelShader)(THIS_ LPCSTR hParameter, LPDIRECT3DPIXELSHADER9 pPShader) PURE;
		STDMETHOD(GetPixelShader)(THIS_ LPCSTR hParameter, LPDIRECT3DPIXELSHADER9 *ppPShader) PURE;
		STDMETHOD(SetVertexShader)(THIS_ LPCSTR hParameter, LPDIRECT3DVERTEXSHADER9 pVShader) PURE;
		STDMETHOD(GetVertexShader)(THIS_ LPCSTR hParameter, LPDIRECT3DVERTEXSHADER9 *ppVShader) PURE;
		// ID3DXBaseEffect

		// Pool
		STDMETHOD(GetPool)(THIS_ LPD3DXEFFECTPOOL* ppPool) PURE;

		// Selecting and setting a technique
		STDMETHOD(SetTechnique)(THIS_ LPCSTR hTechnique) PURE;
		STDMETHOD_(LPCSTR, GetCurrentTechnique)(THIS) PURE;
		STDMETHOD(ValidateTechnique)(THIS_ LPCSTR hTechnique) PURE;
		STDMETHOD(FindNextValidTechnique)(THIS_ LPCSTR hTechnique, LPCSTR *pTechnique) PURE;
		STDMETHOD_(BOOL, IsParameterUsed)(THIS_ LPCSTR hParameter, LPCSTR hTechnique) PURE;

		// Using current technique
		STDMETHOD(Begin)(THIS_ UINT *pPasses, DWORD Flags) PURE;
		STDMETHOD(Pass)(THIS_ UINT Pass) PURE;
		STDMETHOD(End)(THIS) PURE;

		// Managing D3D Device
		STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE9* ppDevice) PURE;
		STDMETHOD(OnLostDevice)(THIS) PURE;
		STDMETHOD(OnResetDevice)(THIS) PURE;

		// Logging device calls
		STDMETHOD(SetStateManager)(THIS_ LPVOID pManager) PURE;
		STDMETHOD(GetStateManager)(THIS_ LPVOID *ppManager) PURE;

		// Parameter blocks
		STDMETHOD(BeginParameterBlock)(THIS) PURE;
		STDMETHOD_(LPCSTR, EndParameterBlock)(THIS) PURE;
		STDMETHOD(ApplyParameterBlock)(THIS_ LPCSTR hParameterBlock) PURE;

		// Cloning
		STDMETHOD(CloneEffect)(THIS_ LPDIRECT3DDEVICE9 pDevice, LPD3DXEFFECT* ppEffect) PURE;
	};

	DECLARE_INTERFACE_(ID3DXEffect92, IUnknown)
	{
		// ID3DXBaseEffect
		STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
		STDMETHOD_(ULONG, AddRef)(THIS) PURE;
		STDMETHOD_(ULONG, Release)(THIS) PURE;

		// Descs
		STDMETHOD(GetDesc)(THIS_ void* pDesc) PURE;
		STDMETHOD(GetParameterDesc)(THIS_ D3DXHANDLE hParameter, void* pDesc) PURE;
		STDMETHOD(GetTechniqueDesc)(THIS_ D3DXHANDLE hTechnique, void* pDesc) PURE;
		STDMETHOD(GetPassDesc)(THIS_ D3DXHANDLE hPass, void* pDesc) PURE;
		STDMETHOD(GetFunctionDesc)(THIS_ D3DXHANDLE hShader, void* pDesc) PURE;

		// Handle operations
		STDMETHOD_(D3DXHANDLE, GetParameter)(THIS_ D3DXHANDLE hParameter, UINT Index) PURE;
		STDMETHOD_(D3DXHANDLE, GetParameterByName)(THIS_ D3DXHANDLE hParameter, LPCSTR pName) PURE;
		STDMETHOD_(D3DXHANDLE, GetParameterBySemantic)(THIS_ D3DXHANDLE hParameter, LPCSTR pSemantic) PURE;
		STDMETHOD_(D3DXHANDLE, GetParameterElement)(THIS_ D3DXHANDLE hParameter, UINT Index) PURE;
		STDMETHOD_(D3DXHANDLE, GetTechnique)(THIS_ UINT Index) PURE;
		STDMETHOD_(D3DXHANDLE, GetTechniqueByName)(THIS_ LPCSTR pName) PURE;
		STDMETHOD_(D3DXHANDLE, GetPass)(THIS_ D3DXHANDLE hTechnique, UINT Index) PURE;
		STDMETHOD_(D3DXHANDLE, GetPassByName)(THIS_ D3DXHANDLE hTechnique, LPCSTR pName) PURE;
		STDMETHOD_(D3DXHANDLE, GetFunction)(THIS_ UINT Index) PURE;
		STDMETHOD_(D3DXHANDLE, GetFunctionByName)(THIS_ LPCSTR pName) PURE;
		STDMETHOD_(D3DXHANDLE, GetAnnotation)(THIS_ D3DXHANDLE hObject, UINT Index) PURE;
		STDMETHOD_(D3DXHANDLE, GetAnnotationByName)(THIS_ D3DXHANDLE hObject, LPCSTR pName) PURE;

		// Get/Set Parameters
		STDMETHOD(SetValue)(THIS_ D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes) PURE;
		STDMETHOD(GetValue)(THIS_ D3DXHANDLE hParameter, LPVOID pData, UINT Bytes) PURE;
		STDMETHOD(SetBool)(THIS_ D3DXHANDLE hParameter, BOOL b) PURE;
		STDMETHOD(GetBool)(THIS_ D3DXHANDLE hParameter, BOOL* pb) PURE;
		STDMETHOD(SetBoolArray)(THIS_ D3DXHANDLE hParameter, CONST BOOL* pb, UINT Count) PURE;
		STDMETHOD(GetBoolArray)(THIS_ D3DXHANDLE hParameter, BOOL* pb, UINT Count) PURE;
		STDMETHOD(SetInt)(THIS_ D3DXHANDLE hParameter, INT n) PURE;
		STDMETHOD(GetInt)(THIS_ D3DXHANDLE hParameter, INT* pn) PURE;
		STDMETHOD(SetIntArray)(THIS_ D3DXHANDLE hParameter, CONST INT* pn, UINT Count) PURE;
		STDMETHOD(GetIntArray)(THIS_ D3DXHANDLE hParameter, INT* pn, UINT Count) PURE;
		STDMETHOD(SetFloat)(THIS_ D3DXHANDLE hParameter, FLOAT f) PURE;
		STDMETHOD(GetFloat)(THIS_ D3DXHANDLE hParameter, FLOAT* pf) PURE;
		STDMETHOD(SetFloatArray)(THIS_ D3DXHANDLE hParameter, CONST FLOAT* pf, UINT Count) PURE;
		STDMETHOD(GetFloatArray)(THIS_ D3DXHANDLE hParameter, FLOAT* pf, UINT Count) PURE;
		STDMETHOD(SetVector)(THIS_ D3DXHANDLE hParameter, CONST D3DXVECTOR4* pVector) PURE;
		STDMETHOD(GetVector)(THIS_ D3DXHANDLE hParameter, D3DXVECTOR4* pVector) PURE;
		STDMETHOD(SetVectorArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXVECTOR4* pVector, UINT Count) PURE;
		STDMETHOD(GetVectorArray)(THIS_ D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count) PURE;
		STDMETHOD(SetMatrix)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix) PURE;
		STDMETHOD(GetMatrix)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX* pMatrix) PURE;
		STDMETHOD(SetMatrixArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count) PURE;
		STDMETHOD(GetMatrixArray)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count) PURE;
		STDMETHOD(SetMatrixPointerArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count) PURE;
		STDMETHOD(GetMatrixPointerArray)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count) PURE;
		STDMETHOD(SetMatrixTranspose)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix) PURE;
		STDMETHOD(GetMatrixTranspose)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX* pMatrix) PURE;
		STDMETHOD(SetMatrixTransposeArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count) PURE;
		STDMETHOD(GetMatrixTransposeArray)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count) PURE;
		STDMETHOD(SetMatrixTransposePointerArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count) PURE;
		STDMETHOD(GetMatrixTransposePointerArray)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count) PURE;
		STDMETHOD(SetString)(THIS_ D3DXHANDLE hParameter, LPCSTR pString) PURE;
		STDMETHOD(GetString)(THIS_ D3DXHANDLE hParameter, LPCSTR* ppString) PURE;
		STDMETHOD(SetTexture)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture) PURE;
		STDMETHOD(GetTexture)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 *ppTexture) PURE;
		STDMETHOD(SetPixelShader)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DPIXELSHADER9 pPShader) PURE;
		STDMETHOD(GetPixelShader)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DPIXELSHADER9 *ppPShader) PURE;
		STDMETHOD(SetVertexShader)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DVERTEXSHADER9 pVShader) PURE;
		STDMETHOD(GetVertexShader)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DVERTEXSHADER9 *ppVShader) PURE;
		STDMETHOD(SetArrayRange)(THIS_ D3DXHANDLE hParameter, UINT uStart, UINT uEnd) PURE; 

		// ID3DXBaseEffect


		// Pool
		STDMETHOD(GetPool)(THIS_ LPD3DXEFFECTPOOL* ppPool) PURE;

		// Selecting and setting a technique
		STDMETHOD(SetTechnique)(THIS_ D3DXHANDLE hTechnique) PURE;
		STDMETHOD_(D3DXHANDLE, GetCurrentTechnique)(THIS) PURE;
		STDMETHOD(ValidateTechnique)(THIS_ D3DXHANDLE hTechnique) PURE;
		STDMETHOD(FindNextValidTechnique)(THIS_ D3DXHANDLE hTechnique, D3DXHANDLE *pTechnique) PURE;
		STDMETHOD_(BOOL, IsParameterUsed)(THIS_ D3DXHANDLE hParameter, D3DXHANDLE hTechnique) PURE;

		// Using current technique
		// Begin           starts active technique
		// BeginPass       begins a pass
		// CommitChanges   updates changes to any set calls in the pass. This should be called before
		//                 any DrawPrimitive call to d3d
		// EndPass         ends a pass
		// End             ends active technique
		STDMETHOD(Begin)(THIS_ UINT *pPasses, DWORD Flags) PURE;
		STDMETHOD(BeginPass)(THIS_ UINT Pass) PURE;
		STDMETHOD(CommitChanges)(THIS) PURE;
		STDMETHOD(EndPass)(THIS) PURE;
		STDMETHOD(End)(THIS) PURE;

		// Managing D3D Device
		STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE9* ppDevice) PURE;
		STDMETHOD(OnLostDevice)(THIS) PURE;
		STDMETHOD(OnResetDevice)(THIS) PURE;

		// Logging device calls
		STDMETHOD(SetStateManager)(THIS_ LPVOID pManager) PURE;
		STDMETHOD(GetStateManager)(THIS_ LPVOID *ppManager) PURE;

		// Parameter blocks
		STDMETHOD(BeginParameterBlock)(THIS) PURE;
		STDMETHOD_(D3DXHANDLE, EndParameterBlock)(THIS) PURE;
		STDMETHOD(ApplyParameterBlock)(THIS_ D3DXHANDLE hParameterBlock) PURE;

		// Cloning
		STDMETHOD(CloneEffect)(THIS_ LPDIRECT3DDEVICE9 pDevice, LPD3DXEFFECT* ppEffect) PURE;
	};

	DECLARE_INTERFACE_(ID3DXEffect93, IUnknown)
	{
		// ID3DXBaseEffect
		STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
		STDMETHOD_(ULONG, AddRef)(THIS) PURE;
		STDMETHOD_(ULONG, Release)(THIS) PURE;

		// Descs
		STDMETHOD(GetDesc)(THIS_ void* pDesc) PURE;
		STDMETHOD(GetParameterDesc)(THIS_ D3DXHANDLE hParameter, void* pDesc) PURE;
		STDMETHOD(GetTechniqueDesc)(THIS_ D3DXHANDLE hTechnique, void* pDesc) PURE;
		STDMETHOD(GetPassDesc)(THIS_ D3DXHANDLE hPass, void* pDesc) PURE;
		STDMETHOD(GetFunctionDesc)(THIS_ D3DXHANDLE hShader, void* pDesc) PURE;

		// Handle operations
		STDMETHOD_(D3DXHANDLE, GetParameter)(THIS_ D3DXHANDLE hParameter, UINT Index) PURE;
		STDMETHOD_(D3DXHANDLE, GetParameterByName)(THIS_ D3DXHANDLE hParameter, LPCSTR pName) PURE;
		STDMETHOD_(D3DXHANDLE, GetParameterBySemantic)(THIS_ D3DXHANDLE hParameter, LPCSTR pSemantic) PURE;
		STDMETHOD_(D3DXHANDLE, GetParameterElement)(THIS_ D3DXHANDLE hParameter, UINT Index) PURE;
		STDMETHOD_(D3DXHANDLE, GetTechnique)(THIS_ UINT Index) PURE;
		STDMETHOD_(D3DXHANDLE, GetTechniqueByName)(THIS_ LPCSTR pName) PURE;
		STDMETHOD_(D3DXHANDLE, GetPass)(THIS_ D3DXHANDLE hTechnique, UINT Index) PURE;
		STDMETHOD_(D3DXHANDLE, GetPassByName)(THIS_ D3DXHANDLE hTechnique, LPCSTR pName) PURE;
		STDMETHOD_(D3DXHANDLE, GetFunction)(THIS_ UINT Index) PURE;
		STDMETHOD_(D3DXHANDLE, GetFunctionByName)(THIS_ LPCSTR pName) PURE;
		STDMETHOD_(D3DXHANDLE, GetAnnotation)(THIS_ D3DXHANDLE hObject, UINT Index) PURE;
		STDMETHOD_(D3DXHANDLE, GetAnnotationByName)(THIS_ D3DXHANDLE hObject, LPCSTR pName) PURE;

		// Get/Set Parameters
		STDMETHOD(SetValue)(THIS_ D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes) PURE;
		STDMETHOD(GetValue)(THIS_ D3DXHANDLE hParameter, LPVOID pData, UINT Bytes) PURE;
		STDMETHOD(SetBool)(THIS_ D3DXHANDLE hParameter, BOOL b) PURE;
		STDMETHOD(GetBool)(THIS_ D3DXHANDLE hParameter, BOOL* pb) PURE;
		STDMETHOD(SetBoolArray)(THIS_ D3DXHANDLE hParameter, CONST BOOL* pb, UINT Count) PURE;
		STDMETHOD(GetBoolArray)(THIS_ D3DXHANDLE hParameter, BOOL* pb, UINT Count) PURE;
		STDMETHOD(SetInt)(THIS_ D3DXHANDLE hParameter, INT n) PURE;
		STDMETHOD(GetInt)(THIS_ D3DXHANDLE hParameter, INT* pn) PURE;
		STDMETHOD(SetIntArray)(THIS_ D3DXHANDLE hParameter, CONST INT* pn, UINT Count) PURE;
		STDMETHOD(GetIntArray)(THIS_ D3DXHANDLE hParameter, INT* pn, UINT Count) PURE;
		STDMETHOD(SetFloat)(THIS_ D3DXHANDLE hParameter, FLOAT f) PURE;
		STDMETHOD(GetFloat)(THIS_ D3DXHANDLE hParameter, FLOAT* pf) PURE;
		STDMETHOD(SetFloatArray)(THIS_ D3DXHANDLE hParameter, CONST FLOAT* pf, UINT Count) PURE;
		STDMETHOD(GetFloatArray)(THIS_ D3DXHANDLE hParameter, FLOAT* pf, UINT Count) PURE;
		STDMETHOD(SetVector)(THIS_ D3DXHANDLE hParameter, CONST D3DXVECTOR4* pVector) PURE;
		STDMETHOD(GetVector)(THIS_ D3DXHANDLE hParameter, D3DXVECTOR4* pVector) PURE;
		STDMETHOD(SetVectorArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXVECTOR4* pVector, UINT Count) PURE;
		STDMETHOD(GetVectorArray)(THIS_ D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count) PURE;
		STDMETHOD(SetMatrix)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix) PURE;
		STDMETHOD(GetMatrix)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX* pMatrix) PURE;
		STDMETHOD(SetMatrixArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count) PURE;
		STDMETHOD(GetMatrixArray)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count) PURE;
		STDMETHOD(SetMatrixPointerArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count) PURE;
		STDMETHOD(GetMatrixPointerArray)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count) PURE;
		STDMETHOD(SetMatrixTranspose)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix) PURE;
		STDMETHOD(GetMatrixTranspose)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX* pMatrix) PURE;
		STDMETHOD(SetMatrixTransposeArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count) PURE;
		STDMETHOD(GetMatrixTransposeArray)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count) PURE;
		STDMETHOD(SetMatrixTransposePointerArray)(THIS_ D3DXHANDLE hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count) PURE;
		STDMETHOD(GetMatrixTransposePointerArray)(THIS_ D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count) PURE;
		STDMETHOD(SetString)(THIS_ D3DXHANDLE hParameter, LPCSTR pString) PURE;
		STDMETHOD(GetString)(THIS_ D3DXHANDLE hParameter, LPCSTR* ppString) PURE;
		STDMETHOD(SetTexture)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture) PURE;
		STDMETHOD(GetTexture)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 *ppTexture) PURE;
		STDMETHOD(GetPixelShader)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DPIXELSHADER9 *ppPShader) PURE;
		STDMETHOD(GetVertexShader)(THIS_ D3DXHANDLE hParameter, LPDIRECT3DVERTEXSHADER9 *ppVShader) PURE;
		STDMETHOD(SetArrayRange)(THIS_ D3DXHANDLE hParameter, UINT uStart, UINT uEnd) PURE; 

		// ID3DXBaseEffect

		// Pool
		STDMETHOD(GetPool)(THIS_ LPD3DXEFFECTPOOL* ppPool) PURE;

		// Selecting and setting a technique
		STDMETHOD(SetTechnique)(THIS_ D3DXHANDLE hTechnique) PURE;
		STDMETHOD_(D3DXHANDLE, GetCurrentTechnique)(THIS) PURE;
		STDMETHOD(ValidateTechnique)(THIS_ D3DXHANDLE hTechnique) PURE;
		STDMETHOD(FindNextValidTechnique)(THIS_ D3DXHANDLE hTechnique, D3DXHANDLE *pTechnique) PURE;
		STDMETHOD_(BOOL, IsParameterUsed)(THIS_ D3DXHANDLE hParameter, D3DXHANDLE hTechnique) PURE;

		// Using current technique
		// Begin           starts active technique
		// BeginPass       begins a pass
		// CommitChanges   updates changes to any set calls in the pass. This should be called before
		//                 any DrawPrimitive call to d3d
		// EndPass         ends a pass
		// End             ends active technique
		STDMETHOD(Begin)(THIS_ UINT *pPasses, DWORD Flags) PURE;
		STDMETHOD(BeginPass)(THIS_ UINT Pass) PURE;
		STDMETHOD(CommitChanges)(THIS) PURE;
		STDMETHOD(EndPass)(THIS) PURE;
		STDMETHOD(End)(THIS) PURE;

		// Managing D3D Device
		STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE9* ppDevice) PURE;
		STDMETHOD(OnLostDevice)(THIS) PURE;
		STDMETHOD(OnResetDevice)(THIS) PURE;

		// Logging device calls
		STDMETHOD(SetStateManager)(THIS_ LPVOID pManager) PURE;
		STDMETHOD(GetStateManager)(THIS_ LPVOID *ppManager) PURE;

		// Parameter blocks
		STDMETHOD(BeginParameterBlock)(THIS) PURE;
		STDMETHOD_(D3DXHANDLE, EndParameterBlock)(THIS) PURE;
		STDMETHOD(ApplyParameterBlock)(THIS_ D3DXHANDLE hParameterBlock) PURE;
		STDMETHOD(DeleteParameterBlock)(THIS_ D3DXHANDLE hParameterBlock) PURE;

		// Cloning
		STDMETHOD(CloneEffect)(THIS_ LPDIRECT3DDEVICE9 pDevice, LPD3DXEFFECT* ppEffect) PURE;

	    // Fast path for setting variables directly in ID3DXEffect
	    STDMETHOD(SetRawValue)(THIS_ D3DXHANDLE hParameter, LPCVOID pData, UINT ByteOffset, UINT Bytes) PURE;
	};

  
	// --------------------------------------------------------------------------------------
	//  Wrapped D3DX Functions
	//		We wrap only functions that are REQUIRED by D3D(non-D3DX) test apps.  If you
	//		think you need to wrap a new D3DX function, talk to ROBALD first to see if
	//		there is a framework equivalent.
	//
	//		We have wrapped most of the D3DXMath lib since it's so useful.
	// --------------------------------------------------------------------------------------

	typedef BOOL    (WINAPI *D3DXCHECKVERSION)(UINT D3DSdkVersion, UINT D3DXSdkVersion);
	typedef HRESULT (WINAPI *D3DXCREATEFONTINDIRECTA)(LPVOID pDevice, CONST D3DXFONT_DESCA* pDesc, LPD3DXFONT* ppFont);
	typedef HRESULT (WINAPI *D3DXCREATEMESH)(DWORD NumFaces, DWORD NumVertices, DWORD Options, CONST DWORD *pDeclaration, LPVOID pD3D, LPD3DXMESH* ppMesh);
	typedef HRESULT (WINAPI *D3DXCREATEMESHFVF)(DWORD NumFaces,DWORD NumVertices, DWORD Options, DWORD FVF, LPVOID pD3D, LPD3DXMESH* ppMesh);
	typedef HRESULT (WINAPI *D3DXRECTPATCHSIZE)(CONST float* pNumSegs, DWORD* pdwTriangles, DWORD* pdwVertices);
	typedef HRESULT (WINAPI *D3DXTRIPATCHSIZE)(CONST float* pNumSegs, DWORD* pdwTriangles, DWORD* pdwVertices);
	typedef HRESULT (WINAPI *D3DXTESSELLATERECTPATCH)(LPDIRECT3DVERTEXBUFFER9 pVB, CONST float* pNumSegs, LPD3DVERTEXELEMENT9 pInDecl, CONST D3DRECTPATCH_INFO *pRectPatchInfo, LPD3DXMESH pMesh);
	typedef HRESULT (WINAPI *D3DXTESSELLATETRIPATCH)(LPDIRECT3DVERTEXBUFFER9 pVB, CONST float* pNumSegs, LPD3DVERTEXELEMENT9 pInDecl, CONST D3DTRIPATCH_INFO *pTriPatchInfo, LPD3DXMESH pMesh);
	typedef HRESULT (WINAPI *D3DXGENERATEOUTPUTDECL)(LPD3DVERTEXELEMENT9 pOutDecl, LPD3DVERTEXELEMENT9 pInDecl);
	typedef HRESULT (WINAPI *D3DXFILTERTEXTURE)(LPVOID pBaseTexture,CONST PALETTEENTRY* pPalette,UINT SrcLevel,DWORD Filter);
	typedef HRESULT (WINAPI *D3DXFILLTEXTURE)(LPVOID pTexture,void* pFunction,LPVOID pData);
	typedef HRESULT (WINAPI *D3DXCREATETEXTURE)(LPVOID pDevice,UINT Width,UINT Height,UINT MipLevels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,LPVOID* ppTexture);
	typedef HRESULT (WINAPI *D3DXCREATEEFFECTFROMFILEA)(LPVOID pDevice,LPCSTR pSrcFile,CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect,LPD3DXBUFFER* ppCompilationErrors);
	typedef HRESULT (WINAPI *D3DXCREATEEFFECTFROMRESOURCEA)(LPVOID pDevice,HMODULE hSrcModule,LPCSTR pSrcResource,CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect,LPD3DXBUFFER* ppCompilationErrors);
	typedef HRESULT (WINAPI *D3DXASSEMBLESHADER9)(LPCVOID pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrorMsgs);
    typedef HRESULT (WINAPI *D3DXASSEMBLESHADER8)(LPCVOID pSrcData, UINT SrcDataLen, DWORD dwFlags, LPD3DXBUFFER* ppConstants, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrors);
	typedef HRESULT (WINAPI *D3DXCREATETEXTUREFROMFILEINMEMORY)(LPVOID pDevice,LPCVOID pSrcData,UINT SrcDataSize,LPVOID* ppTexture);
	typedef HRESULT (WINAPI *D3DXLOADSURFACEFROMFILEA)(LPVOID pDestSurface,CONST PALETTEENTRY* pDestPalette,CONST RECT* pDestRect,LPCSTR pSrcFile,CONST RECT* pSrcRect,DWORD Filter,D3DCOLOR ColorKey,D3DXIMAGE_INFO* pSrcInfo);
	typedef HRESULT (WINAPI *D3DXLOADSURFACEFROMFILEINMEMORY)(LPVOID pDestSurface,CONST PALETTEENTRY* pDestPalette,CONST RECT* pDestRect,LPCVOID SrcData,UINT rcDataSize,CONST RECT* pSrcRect,DWORD Filter,D3DCOLOR ColorKey,D3DXIMAGE_INFO* pSrcInfo);
	typedef HRESULT (WINAPI *D3DXLOADSURFACEFROMSURFACE)(LPVOID pDestSurface,CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPVOID pSrcSurface, CONST PALETTEENTRY* pSrcPalette,CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey);
	typedef HRESULT (WINAPI *D3DXLOADVOLUMEFROMVOLUME)(LPVOID pDestVolume,CONST PALETTEENTRY* pDestPalette,CONST D3DBOX* pDestBox,LPVOID pSrcVolume,CONST PALETTEENTRY* pSrcPalette,CONST D3DBOX* pSrcBox,DWORD Filter,D3DCOLOR ColorKey);
	typedef HRESULT (WINAPI *D3DXLOADVOLUMEFROMMEMORY)(LPVOID pDestVolume,CONST PALETTEENTRY* pDestPalette,CONST D3DBOX* pDestBox,LPCVOID pSrcMemory,D3DFORMAT SrcFormat,UINT SrcRowPitch,UINT SrcSlicePitch,CONST PALETTEENTRY* pSrcPalette,CONST D3DBOX* pSrcBox,DWORD Filter,D3DCOLOR ColorKey);
	typedef HRESULT (WINAPI *D3DXCREATETEXTUREFROMFILEEXA)(LPVOID pDevice,LPCSTR pSrcFile,UINT Width,UINT Height,UINT MipLevels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,DWORD Filter,DWORD MipFilter,D3DCOLOR ColorKey,D3DXIMAGE_INFO* pSrcInfo,PALETTEENTRY* pPalette,LPVOID* ppTexture);
	typedef HRESULT (WINAPI *D3DXCREATETEXTUREFROMFILEINMEMORYEX)(LPVOID pDevice,LPCVOID pSrcData,UINT SrcDataSize,UINT Width,UINT Height,UINT MipLevels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,DWORD Filter,DWORD MipFilter,D3DCOLOR ColorKey,D3DXIMAGE_INFO* pSrcInfo,PALETTEENTRY* pPalette,LPVOID* ppTexture);
	typedef HRESULT (WINAPI *D3DXCREATETEXTUREFROMRESOURCEA)(LPVOID pDevice,HMODULE hSrcModule,LPCSTR pSrcResource,LPVOID* ppTexture);
	typedef HRESULT (WINAPI *D3DXSAVETEXTURETOFILEA)(LPCSTR pDestFile,D3DXIMAGE_FILEFORMAT DestFormat,LPVOID pSrcTexture,CONST PALETTEENTRY* pSrcPalette);
	typedef HRESULT (WINAPI *D3DXSAVESURFACETOFILEA)(LPCSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat,LPVOID pSrcSurface,CONST PALETTEENTRY* pSrcPalette,CONST RECT* pSrcRect);
	typedef HRESULT (WINAPI *D3DXLOADSURFACEFROMMEMORY)(LPVOID pDestSurface,CONST PALETTEENTRY* pDestPalette,CONST RECT* pDestRect,LPCVOID pSrcMemory,D3DFORMAT SrcFormat,UINT SrcPitch,CONST PALETTEENTRY* pSrcPalette,CONST RECT* pSrcRect,DWORD Filter,D3DCOLOR ColorKey);
	typedef HRESULT (WINAPI *D3DXTESSELLATENPATCHES)(LPD3DXMESH pMeshIn,CONST DWORD* pAdjacencyIn,FLOAT NumSegs,BOOL  QuadraticInterpNormals,LPD3DXMESH *ppMeshOut,LPD3DXBUFFER *ppAdjacencyOut);	
	typedef HRESULT (WINAPI *D3DXCOMPILESHADER)(LPCSTR pSrcData,UINT SrcDataLen,CONST D3DXMACRO* pDefines,LPD3DXINCLUDE pInclude,LPCSTR pFunctionName,LPCSTR pProfile,DWORD Flags,LPD3DXBUFFER* ppShader,LPD3DXBUFFER* ppErrorMsgs,LPD3DXCONSTANTTABLE*ppConstantTable);
	typedef BOOL    (WINAPI *D3DXDEBUGMUTE) (BOOL Mute);
	
	typedef D3DXVECTOR2* (WINAPI *D3DXVEC2NORMALIZE)(D3DXVECTOR2 *pOut, CONST D3DXVECTOR2 *pV);
	typedef D3DXVECTOR2* (WINAPI *D3DXVEC2HERMITE)(D3DXVECTOR2 *pOut, CONST D3DXVECTOR2 *pV1, CONST D3DXVECTOR2 *pT1,CONST D3DXVECTOR2 *pV2, CONST D3DXVECTOR2 *pT2, FLOAT s);
	typedef D3DXVECTOR2* (WINAPI *D3DXVEC2CATMULLROM)(D3DXVECTOR2 *pOut, CONST D3DXVECTOR2 *pV0, CONST D3DXVECTOR2 *pV1,CONST D3DXVECTOR2 *pV2, CONST D3DXVECTOR2 *pV3, FLOAT s);
	typedef D3DXVECTOR2* (WINAPI *D3DXVEC2BARYCENTRIC)(D3DXVECTOR2 *pOut, CONST D3DXVECTOR2 *pV1, CONST D3DXVECTOR2 *pV2,CONST D3DXVECTOR2 *pV3, FLOAT f, FLOAT g);
	typedef D3DXVECTOR4* (WINAPI *D3DXVEC2TRANSFORM)(D3DXVECTOR4 *pOut, CONST D3DXVECTOR2 *pV, CONST D3DXMATRIX *pM);
	typedef D3DXVECTOR2* (WINAPI *D3DXVEC2TRANSFORMCOORD)(D3DXVECTOR2 *pOut, CONST D3DXVECTOR2 *pV, CONST D3DXMATRIX *pM);
	typedef D3DXVECTOR2* (WINAPI *D3DXVEC2TRANSFORMNORMAL)(D3DXVECTOR2 *pOut, CONST D3DXVECTOR2 *pV, CONST D3DXMATRIX *pM);
	typedef D3DXVECTOR3* (WINAPI *D3DXVEC3NORMALIZE)(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV);
	typedef D3DXVECTOR3* (WINAPI *D3DXVEC3HERMITE)(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pT1,CONST D3DXVECTOR3 *pV2, CONST D3DXVECTOR3 *pT2, FLOAT s);
	typedef D3DXVECTOR3* (WINAPI *D3DXVEC3CATMULLROM)(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV0, CONST D3DXVECTOR3 *pV1,CONST D3DXVECTOR3 *pV2, CONST D3DXVECTOR3 *pV3, FLOAT s);
	typedef D3DXVECTOR3* (WINAPI *D3DXVEC3BARYCENTRIC)(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pV2,CONST D3DXVECTOR3 *pV3, FLOAT f, FLOAT g);
	typedef D3DXVECTOR4* (WINAPI *D3DXVEC3TRANSFORM)(D3DXVECTOR4 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM);
	typedef D3DXVECTOR3* (WINAPI *D3DXVEC3TRANSFORMCOORD)(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM);
	typedef D3DXVECTOR3* (WINAPI *D3DXVEC3TRANSFORMNORMAL)(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM);
	typedef D3DXVECTOR3* (WINAPI *D3DXVEC3PROJECT)(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DVIEWPORT8 *pViewport,CONST D3DXMATRIX *pProjection, CONST D3DXMATRIX *pView, CONST D3DXMATRIX *pWorld);
	typedef D3DXVECTOR3* (WINAPI *D3DXVEC3UNPROJECT)(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DVIEWPORT8 *pViewport,CONST D3DXMATRIX *pProjection, CONST D3DXMATRIX *pView, CONST D3DXMATRIX *pWorld);
	typedef D3DXVECTOR4* (WINAPI *D3DXVEC4CROSS)(D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV1, CONST D3DXVECTOR4 *pV2,CONST D3DXVECTOR4 *pV3);
	typedef D3DXVECTOR4* (WINAPI *D3DXVEC4NORMALIZE)(D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV);
	typedef D3DXVECTOR4* (WINAPI *D3DXVEC4HERMITE)(D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV1, CONST D3DXVECTOR4 *pT1,CONST D3DXVECTOR4 *pV2, CONST D3DXVECTOR4 *pT2, FLOAT s );
	typedef D3DXVECTOR4* (WINAPI *D3DXVEC4CATMULLROM)(D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV0, CONST D3DXVECTOR4 *pV1,CONST D3DXVECTOR4 *pV2, CONST D3DXVECTOR4 *pV3, FLOAT s );
	typedef D3DXVECTOR4* (WINAPI *D3DXVEC4BARYCENTRIC)(D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV1, CONST D3DXVECTOR4 *pV2,CONST D3DXVECTOR4 *pV3, FLOAT f, FLOAT g);
	typedef D3DXVECTOR4* (WINAPI *D3DXVEC4TRANSFORM)(D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV, CONST D3DXMATRIX *pM);
	typedef FLOAT (WINAPI *D3DXMATRIXDETERMINANT)(CONST D3DXMATRIX *pM);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXTRANSPOSE)(D3DXMATRIX *pOut, CONST D3DXMATRIX *pM);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXMULTIPLY)(D3DXMATRIX *pOut, CONST D3DXMATRIX *pM1, CONST D3DXMATRIX *pM2);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXMULTIPLYTRANSPOSE)(D3DXMATRIX *pOut, CONST D3DXMATRIX *pM1, CONST D3DXMATRIX *pM2);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXINVERSE)(D3DXMATRIX *pOut, FLOAT *pDeterminant, CONST D3DXMATRIX *pM);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXSCALING)(D3DXMATRIX *pOut, FLOAT sx, FLOAT sy, FLOAT sz );
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXTRANSLATION)(D3DXMATRIX *pOut, FLOAT x, FLOAT y, FLOAT z );
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXROTATIONX)(D3DXMATRIX *pOut, FLOAT Angle);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXROTATIONY)(D3DXMATRIX *pOut, FLOAT Angle);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXROTATIONZ)(D3DXMATRIX *pOut, FLOAT Angle);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXROTATIONAXIS)(D3DXMATRIX *pOut, CONST D3DXVECTOR3 *pV, FLOAT Angle);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXROTATIONQUATERNION)(D3DXMATRIX *pOut, CONST D3DXQUATERNION *pQ);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXROTATIONYAWPITCHROLL)(D3DXMATRIX *pOut, FLOAT Yaw, FLOAT Pitch, FLOAT Roll);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXTRANSFORMATION)(D3DXMATRIX *pOut, CONST D3DXVECTOR3 *pScalingCenter,CONST D3DXQUATERNION *pScalingRotation, CONST D3DXVECTOR3 *pScaling,CONST D3DXVECTOR3 *pRotationCenter, CONST D3DXQUATERNION *pRotation,CONST D3DXVECTOR3 *pTranslation);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXAFFINETRANSFORMATION)(D3DXMATRIX *pOut, FLOAT Scaling, CONST D3DXVECTOR3 *pRotationCenter,CONST D3DXQUATERNION *pRotation, CONST D3DXVECTOR3 *pTranslation);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXLOOKATRH)(D3DXMATRIX *pOut, CONST D3DXVECTOR3 *pEye, CONST D3DXVECTOR3 *pAt,CONST D3DXVECTOR3 *pUp);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXLOOKATLH)(D3DXMATRIX *pOut, CONST D3DXVECTOR3 *pEye, CONST D3DXVECTOR3 *pAt,CONST D3DXVECTOR3 *pUp);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXPERSPECTIVERH)(D3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXPERSPECTIVELH)(D3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXPERSPECTIVEFOVRH)(D3DXMATRIX *pOut, FLOAT fovy, FLOAT Aspect, FLOAT zn, FLOAT zf);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXPERSPECTIVEFOVLH)(D3DXMATRIX *pOut, FLOAT fovy, FLOAT Aspect, FLOAT zn, FLOAT zf);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXPERSPECTIVEOFFCENTERRH)(D3DXMATRIX *pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn,FLOAT zf);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXPERSPECTIVEOFFCENTERLH)(D3DXMATRIX *pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn,FLOAT zf);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXORTHORH)(D3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXORTHOLH)(D3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXORTHOOFFCENTERRH)(D3DXMATRIX *pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn,FLOAT zf);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXORTHOOFFCENTERLH)(D3DXMATRIX *pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn,FLOAT zf);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXSHADOW)(D3DXMATRIX *pOut, CONST D3DXVECTOR4 *pLight,CONST D3DXPLANE *pPlane);
	typedef D3DXMATRIX* (WINAPI *D3DXMATRIXREFLECT)(D3DXMATRIX *pOut, CONST D3DXPLANE *pPlane);
	typedef void (WINAPI *D3DXQUATERNIONTOAXISANGLE)(CONST D3DXQUATERNION *pQ, D3DXVECTOR3 *pAxis, FLOAT *pAngle);
	typedef D3DXQUATERNION* (WINAPI *D3DXQUATERNIONROTATIONMATRIX)(D3DXQUATERNION *pOut, CONST D3DXMATRIX *pM);
	typedef D3DXQUATERNION* (WINAPI *D3DXQUATERNIONROTATIONAXIS)(D3DXQUATERNION *pOut, CONST D3DXVECTOR3 *pV, FLOAT Angle);
	typedef D3DXQUATERNION* (WINAPI *D3DXQUATERNIONROTATIONYAWPITCHROLL)(D3DXQUATERNION *pOut, FLOAT Yaw, FLOAT Pitch, FLOAT Roll);
	typedef D3DXQUATERNION* (WINAPI *D3DXQUATERNIONMULTIPLY)(D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ1,CONST D3DXQUATERNION *pQ2);
	typedef D3DXQUATERNION* (WINAPI *D3DXQUATERNIONNORMALIZE)(D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ);
	typedef D3DXQUATERNION* (WINAPI *D3DXQUATERNIONINVERSE)(D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ);
	typedef D3DXQUATERNION* (WINAPI *D3DXQUATERNIONLN)(D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ);
	typedef D3DXQUATERNION* (WINAPI *D3DXQUATERNIONEXP)(D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ);
	typedef D3DXQUATERNION* (WINAPI *D3DXQUATERNIONSLERP)(D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ1,CONST D3DXQUATERNION *pQ2, FLOAT t);
	typedef D3DXQUATERNION* (WINAPI *D3DXQUATERNIONSQUAD)(D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ1,CONST D3DXQUATERNION *pA, CONST D3DXQUATERNION *pB,CONST D3DXQUATERNION *pC, FLOAT t);
	typedef void (WINAPI *D3DXQUATERNIONSQUADSETUP)(D3DXQUATERNION *pAOut, D3DXQUATERNION *pBOut, D3DXQUATERNION *pCOut,CONST D3DXQUATERNION *pQ0, CONST D3DXQUATERNION *pQ1, CONST D3DXQUATERNION *pQ2, CONST D3DXQUATERNION *pQ3);
	typedef D3DXQUATERNION* (WINAPI *D3DXQUATERNIONBARYCENTRIC)(D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ1,CONST D3DXQUATERNION *pQ2, CONST D3DXQUATERNION *pQ3,FLOAT f, FLOAT g);
	typedef D3DXPLANE* (WINAPI *D3DXPLANENORMALIZE)(D3DXPLANE *pOut, CONST D3DXPLANE *pP);
	typedef D3DXVECTOR3* (WINAPI *D3DXPLANEINTERSECTLINE)(D3DXVECTOR3 *pOut, CONST D3DXPLANE *pP, CONST D3DXVECTOR3 *pV1,CONST D3DXVECTOR3 *pV2);
	typedef D3DXPLANE* (WINAPI *D3DXPLANEFROMPOINTNORMAL)(D3DXPLANE *pOut, CONST D3DXVECTOR3 *pPoint, CONST D3DXVECTOR3 *pNormal);
	typedef D3DXPLANE* (WINAPI *D3DXPLANEFROMPOINTS)(D3DXPLANE *pOut, CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pV2,CONST D3DXVECTOR3 *pV3);
	typedef D3DXPLANE* (WINAPI *D3DXPLANETRANSFORM)(D3DXPLANE *pOut, CONST D3DXPLANE *pP, CONST D3DXMATRIX *pM);
	typedef D3DXCOLOR* (WINAPI *D3DXCOLORADJUSTSATURATION)(D3DXCOLOR *pOut, CONST D3DXCOLOR *pC, FLOAT s);
	typedef D3DXCOLOR* (WINAPI *D3DXCOLORADJUSTCONTRAST)(D3DXCOLOR *pOut, CONST D3DXCOLOR *pC, FLOAT c);
	typedef HRESULT (WINAPI *D3DXCREATETEXTUREFROMFILEA)(LPVOID pDevice, LPCSTR pSrcFile, LPVOID* ppTexture);
	typedef HRESULT (WINAPI *D3DXCREATETEXTUREFROMRESOURCEEXA)(LPVOID pDevice,HMODULE hSrcModule,LPCSTR pSrcResource,UINT Width,UINT Height,UINT MipLevels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,DWORD Filter,DWORD MipFilter,D3DCOLOR ColorKey,D3DXIMAGE_INFO* pSrcInfo,PALETTEENTRY* pPalette,LPVOID* ppTexture);
	
    // non-inline
#ifdef __cplusplus
}
#endif

#endif