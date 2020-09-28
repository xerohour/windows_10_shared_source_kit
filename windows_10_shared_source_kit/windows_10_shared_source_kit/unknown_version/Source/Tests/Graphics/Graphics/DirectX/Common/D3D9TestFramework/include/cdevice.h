// ======================================================================================
//
//  CDevice.h
//
//  Header file for the CDevice class.
//
//	This is a wrapper for all versions of the D3D interface.
//
//  Copyright (C) 2000 Microsoft Corporation
//
// ======================================================================================

#ifndef __CDEVICE_H__
#define __CDEVICE_H__

#include "Framework.h"
#include "CD3D.h"
#include <objidl.h>

// --------------------------------------------------------------------------------------


class CDevice : public CObject
{
protected:
	friend class CD3D;
	friend class CSurface;
	friend class CVolume;
	friend class CIndexBuffer;
	friend class CVertexBuffer;
	friend class CTexture;
	friend class CCubeTexture;
	friend class CVolTexture;
	friend class CApiTest;
	friend class CEffect;
	friend class CResource;


	// There attributes are Read/Write
	int 				m_uAdapter; 							// Adapter ID
	DWORD				m_dwDevType;							// Device type (HAL, REF, etc)
	HWND				m_hFocusWindow; 						// Focus window for device
	DWORD				m_dwBehaviors;							// Device creation flags
	float				m_fInterface;							// DX version for this device
	PRESENT_PARAMETERS *m_pPresParams;
	DWORD				m_dwPSGPRegChange;						// PSGP switch mode
	DWORD				m_dwVPEmuRegChange; 					// Vertex pipeline emulation switch mode
	DWORD				m_dwPPEmuRegChange; 					// Pixel pipeline emulation switch mode

	// These attributes are read only
	CD3D					*m_pD3D;							// Ptr to the D3D object for this device
	union
	{
		LPDIRECT3DDEVICE9		m_pDevice9;
		LPDIRECT3DDEVICE9EX		m_pDevice9Ex;
	};
	LPDIRECT3DDEVICE8		m_pDevice8;
	LPDIRECT3DDEVICE7		m_pDevice7;
	LPDIRECT3DDEVICE3		m_pDevice3;
	LPDIRECT3DVIEWPORT3 	m_pViewport3;
	LPDIRECT3DMATERIAL3 	m_pMaterial3;
	LPDIRECT3DLIGHT 		*m_pLights3;						// Array for light pointers
	DWORD					m_dwLights3;						// Current size of the array

	LPD3DXFONT				m_pD3DXFont;

	// Private data
	CSurface			*m_pPrimary;							// Primary surface
	CSurface			*m_pZBuffer;							// ZBuffer
	CSurface			**m_pBackBuffers;						// Array of BackBuffer pointers
	int 				m_nBackBuffers; 						// Number of BackBuffers

	GUID				m_Guid; 								// Device identifier
	TCHAR				m_tcsDescription[256];					// Device description
	bool				m_bHWVP;								// Device does HWVP
	bool				m_bVPEmu;								// Vertex pipeline emulation enabled
	bool				m_bPPEmu;								// Pixel pipeline emulation enabled
	bool				m_bPure;								// Device is a PURE device
	bool				m_bFullscreen;							// Device is currently fullscreen
	LOGFONT 			m_LogFont;								// Font for drawing text in fullscreen
	CAPS				*m_pCaps9Cached;						// Cache of dx9 caps to have when needed
																	// for downlevel devices
				
	int 				m_nDDILevel;							// DDI level
	DEVICEDESC			m_Desc; 								// Caps
	int 				m_nTextureFormat;						// Texture format count
	FORMAT* 			m_pTextureFormat;						// Texture formats
	int 				m_nCubeFormat;							// Cube format count
	FORMAT* 			m_pCubeFormat;							// Cube formats
	int 				m_nVolumeFormat;						// Volume format count
	FORMAT* 			m_pVolumeFormat;						// Volume formats
	int 				m_nZBufferFormat;						// Z-Buffer format count
	FORMAT* 			m_pZBufferFormat;						// Z-Buffer formats
	int 				m_nBackBufferFormat;					// BackBuffer format count
	FORMAT* 			m_pBackBufferFormat;					// BackBuffer formats
	int 				m_nMultisampType;						// Multisample format count
	MULTISAMPLE_TYPE*	m_pMultisampType;						// Multisample formats

	CDevice 		   *m_pNext;								// Linked list ptr

	// Device state
	DWORD				m_dwTexLoad;							// Toggles Texture loading method when DEFAULT is specified
	DWORD				m_dwWrapUV; 							// State needed to map DX6 D3DRS_WrapUV to DX7 and above
	PALETTEENTRY		m_Palette4[16]; 						// Default 4-bit pallete
	PALETTEENTRY		m_Palette8[256];						// Default 8-bit pallete


	// Private CDevice functions
	void CreateDescription(TCHAR* sString);
	void GenerateColorCube(PALETTEENTRY* rgPal, DWORD dwNumEntries, float fVersion);
	void NextTexLoad(SURFACEDESC *pDesc);
	void NextTexLoad(VOLUME_DESC *pDesc);
	HRESULT CDevice::RenderToQuad( RECT * pRect, CTexture * pTexture );

	//DX7 specific functions
	virtual HRESULT 	CreateBackBuffers7(PRESENT_PARAMETERS *pParams);			// Create the backbuffers

	//DX6 specific functions   
	virtual HRESULT 	CreateBackBuffers6(PRESENT_PARAMETERS *pParams);			// Create the backbuffers

	// Version wrappers
	void				InitFunctionPtrs(float fVersion);

	virtual HRESULT 	Initialize(PRESENT_PARAMETERS *pParams) { return (this->*pInitialize)(pParams); }
	HRESULT 			(CDevice::*pInitialize)(PRESENT_PARAMETERS *pParams);
	virtual HRESULT 	Initialize9(PRESENT_PARAMETERS *pParams);
	virtual HRESULT 	Initialize8(PRESENT_PARAMETERS *pParams);
	virtual HRESULT 	Initialize7(PRESENT_PARAMETERS *pParams);
	virtual HRESULT 	Initialize6(PRESENT_PARAMETERS *pParams);

	HRESULT 			(CDevice::*pWrapDevice)(PRESENT_PARAMETERS *pParams);
	virtual HRESULT 	WrapDevice9(PRESENT_PARAMETERS *pParams);
	virtual HRESULT 	WrapDevice8(PRESENT_PARAMETERS *pParams);

	bool				(CDevice::*pIsDeviceEqual)(PVOID pDevice);
	virtual bool		IsDeviceEqual6(PVOID pDevice);
	virtual bool		IsDeviceEqual7(PVOID pDevice);
	virtual bool		IsDeviceEqual8(PVOID pDevice);
	virtual bool		IsDeviceEqual9(PVOID pDevice);

	virtual void		ReleaseFormatLists();
	void				ReleaseAll();

	void				(CDevice::*pReleaseD3D)();
	virtual void		ReleaseD3D9();
	virtual void		ReleaseD3D8();
	virtual void		ReleaseD3D7();
	virtual void		ReleaseD3D6();

	void				(CDevice::*pReleaseWrappers)();
	virtual void		ReleaseWrappers8();
	virtual void		ReleaseWrappers9();

	bool				(CDevice::*pTextureFormatSupported)(FORMAT *pFmt);
	virtual bool		TextureFormatSupported9(FORMAT *pFmt);
	virtual bool		TextureFormatSupported8(FORMAT *pFmt);
	virtual bool		TextureFormatSupported7(FORMAT *pFmt);

	bool				(CDevice::*pZBufferFormatSupported)(FORMAT *pFmt);
	virtual bool		ZBufferFormatSupported9(FORMAT *pFmt);
	virtual bool		ZBufferFormatSupported8(FORMAT *pFmt);
	virtual bool		ZBufferFormatSupported7(FORMAT *pFmt);
	
	bool				(CDevice::*pCubeFormatSupported)(FORMAT *pFmt);
	virtual bool		CubeFormatSupported9(FORMAT *pFmt);
	virtual bool		CubeFormatSupported8(FORMAT *pFmt);
	virtual bool		CubeFormatSupported7(FORMAT *pFmt);

	bool				(CDevice::*pVolumeFormatSupported)(FORMAT *pFmt);
	virtual bool		VolumeFormatSupported9(FORMAT *pFmt);
	virtual bool		VolumeFormatSupported8(FORMAT *pFmt);
	virtual bool		VolumeFormatSupported7(FORMAT *pFmt);

	bool				(CDevice::*pDraw2DText)(DWORD dwColor, TCHAR *szString, int nLen, RECT* pRect, DWORD dwFlags);
	virtual bool		Draw2DText9(DWORD dwColor, TCHAR *szString, int nLen, RECT* pRect, DWORD dwFlags);
	virtual bool		Draw2DText8(DWORD dwColor, TCHAR *szString, int nLen, RECT* pRect, DWORD dwFlags);
	virtual bool		Draw2DText7(DWORD dwColor, TCHAR *szString, int nLen, RECT* pRect, DWORD dwFlags);
	virtual bool		Draw2DText6(DWORD dwColor, TCHAR *szString, int nLen, RECT* pRect, DWORD dwFlags);

	HRESULT 			(CDevice::*pValidateMode)(DEVICEMODE *pMode);
	virtual HRESULT 	ValidateMode9(DEVICEMODE *pMode);
	virtual HRESULT 	ValidateMode8(DEVICEMODE *pMode);
	virtual HRESULT 	ValidateMode7(DEVICEMODE *pMode);
	virtual HRESULT 	ValidateMode6(DEVICEMODE *pMode);

	bool				(CDevice::*pHandleDeviceLost)();
	virtual bool		HandleDeviceLost9();
	virtual bool		HandleDeviceLost8();
	virtual bool		HandleDeviceLost7();
	virtual bool		HandleDeviceLost6();

	bool				(CDevice::*pIsDeviceLost)();
	virtual bool		IsDeviceLost9L();
	virtual bool		IsDeviceLost9();
	virtual bool		IsDeviceLost8();
	virtual bool		IsDeviceLost7();
	virtual bool		IsDeviceLost6();

	bool				(CDevice::*pIsDeviceReadyForReset)();
	virtual bool		IsDeviceReadyForReset9L();
	virtual bool		IsDeviceReadyForReset9();
	virtual bool		IsDeviceReadyForReset8();
	virtual bool		IsDeviceReadyForReset7();
	virtual bool		IsDeviceReadyForReset6();

	HRESULT 			(CDevice::*pResize)(UINT uWidth, UINT uHeight);
	virtual HRESULT 	Resize9(UINT uWidth, UINT uHeight);
	virtual HRESULT 	Resize8(UINT uWidth, UINT uHeight);
	virtual HRESULT 	Resize7(UINT uWidth, UINT uHeight);
	virtual HRESULT 	Resize6(UINT uWidth, UINT uHeight);

	HRESULT 			(CDevice::*pD3DXFontOnLostDevice)();
	virtual HRESULT 	D3DXFontOnLostDevice9();
	virtual HRESULT 	D3DXFontOnLostDevice8();
	virtual HRESULT 	D3DXFontOnLostDeviceUnsup();

	HRESULT 			(CDevice::*pD3DXFontOnResetDevice)();
	virtual HRESULT 	D3DXFontOnResetDevice9();
	virtual HRESULT 	D3DXFontOnResetDevice8();
	virtual HRESULT 	D3DXFontOnResetDeviceUnsup();

	virtual void*		GetInternalPointer(void);
	DWORD				SetPSGPRegistry(DWORD dwPSGP);
	DWORD				SetVPEmuRegistry(DWORD dwVPEmu);
	DWORD				SetPPEmuRegistry(DWORD dwPPEmu);

public:
	
	CDevice(CD3D *pD3D, UINT uAdapter, DWORD dwDevType, DWORD dwBehaviors, HWND hFocus); 
	~CDevice();

	virtual ULONG	WINAPI AddRef(void);
	virtual ULONG	WINAPI Release(void);

	inline bool 	CapsDetected() { return m_nDDILevel > 0;}
	inline DEVICEDESC* GetCaps() { return &m_Desc; }
	const CAPS* 	GetCachedCaps9();
	    
	inline LPDIRECT3DDEVICE9 GetDevicePointer9() { return m_pDevice9; }
	inline LPDIRECT3DDEVICE8 GetDevicePointer8() { return m_pDevice8; }
	inline LPDIRECT3DDEVICE7 GetDevicePointer7() { return m_pDevice7; }
	inline LPDIRECT3DDEVICE3 GetDevicePointer6() { return m_pDevice3; }

	ADAPTER*		GetAdapter();
	inline CSurface* GetPrimary() { return m_pPrimary; }
	inline CSurface* GetZBuffer() { return m_pZBuffer; }
	inline PRESENT_PARAMETERS* GetPresParams() { return m_pPresParams; }
	TCHAR*			GetDescription() { return m_tcsDescription; }
	inline int		GetAdapterID() { return m_uAdapter; }
	inline void 	SetAdapterID(int nAdapter) { m_uAdapter = nAdapter; }
	inline float	GetInterface() { return m_fInterface; }
	inline void 	SetInterface(float fInterface) { m_fInterface = fInterface; }
	inline DWORD	GetDevType() { return m_dwDevType; }
	inline void 	SetDevType(DWORD dwDevType) { m_dwDevType = dwDevType; }
	inline DWORD	GetBehaviors() { return m_dwBehaviors; }
	inline void 	SetBehaviors(DWORD dwBehaviors) { m_dwBehaviors = dwBehaviors; }
	inline GUID 	GetGuid() { return m_Guid; }
	inline int		GetDDI() { return m_nDDILevel; }
	inline CD3D*	GetD3D() { return m_pD3D; }  //note: test apps should use "GetDirect3D"
	inline bool 	IsPure() { return m_bPure; }
	inline bool 	IsHWVP() { return m_bHWVP; }
	inline bool 	IsFullscreen() {return m_bFullscreen; }
	inline FORMAT*	GetZFormats() { return m_pZBufferFormat; }
	inline int		GetNumZFormats() { return m_nZBufferFormat; }
	inline FORMAT*	GetTexFormats() { return m_pTextureFormat; }
	inline int		GetNumTexFormats() { return m_nTextureFormat; }
	inline FORMAT*	GetCubeFormats() { return m_pCubeFormat; }
	inline int		GetNumCubeFormats() { return m_nCubeFormat; }
	inline FORMAT*	GetVolumeFormats() { return m_pVolumeFormat; }
	inline int		GetNumVolumeFormats() { return m_nVolumeFormat; }
	inline FORMAT*	GetBackBufferFormats() { return m_pBackBufferFormat; }
	inline int		GetNumBufferFormats() { return m_nBackBufferFormat; }
	inline MULTISAMPLE_TYPE* GetMultiSampleFormats() { return m_pMultisampType; }
	inline int		GetNumMultiSampleFormats() { return m_nMultisampType; }
	inline DISPLAYMODE* GetDisplayModes() { return GetAdapter()->pModes; }
	inline int		GetNumDisplayModes() { return GetAdapter()->nModes; }
	virtual HRESULT SetLastError(HRESULT hr);
	virtual HRESULT GetLastError();
	inline void 	SetFont( LOGFONT lf ) { m_LogFont = lf; }
	inline LOGFONT	GetFont() { return m_LogFont; }

	inline bool 	IsActive() { return (GetRefCount() > 1); }
	inline void 	UpdateDescription() { CreateDescription((TCHAR*)m_tcsDescription); };
	inline DWORD	GetTexLoad() { return m_dwTexLoad; }
	inline void 	SetTexLoad(DWORD dwTexLoad) { m_dwTexLoad = dwTexLoad; }
	inline PALETTEENTRY* GetDefault8bitPalette() { return m_Palette8; }
	inline void 	SetPSGPRegChange(DWORD dwPSGPRegChange) { m_dwPSGPRegChange = dwPSGPRegChange; }
	virtual void	SetVPEmuRegChange(DWORD dwVPEmuRegChange);
	virtual void	SetPPEmuRegChange(DWORD dwPPEmuRegChange);
	virtual bool	IsPSGPEnabled();
	virtual bool	IsVPEmuEnabled() { return m_bVPEmu; }
	virtual bool	IsPPEmuEnabled() { return m_bPPEmu; }
	
	LPDIRECT3DLIGHT 		 GetLight3Pointer(DWORD dwLightIndex);

	// Public version wrappers
	virtual bool		IsDeviceEqual(PVOID pDevice) { return (this->*pIsDeviceEqual)(pDevice); }
	virtual bool		TextureFormatSupported(FORMAT *pFmt) { return (this->*pTextureFormatSupported)(pFmt); }
	virtual bool		ZBufferFormatSupported(FORMAT *pFmt) { return (this->*pZBufferFormatSupported)(pFmt); }
	virtual bool		CubeFormatSupported(FORMAT *pFmt) { return (this->*pCubeFormatSupported)(pFmt); }
	virtual bool		VolumeFormatSupported(FORMAT *pFmt) { return (this->*pVolumeFormatSupported)(pFmt); }
	virtual bool		Draw2DText(DWORD dwColor, TCHAR *szString, int nLen, RECT* pRect, DWORD dwFlags) { return (this->*pDraw2DText)(dwColor, szString, nLen, pRect, dwFlags); }
	virtual HRESULT 	ValidateMode(DEVICEMODE *pMode) { return (this->*pValidateMode)(pMode); }
	virtual bool		HandleDeviceLost() { return (this->*pHandleDeviceLost)(); }
	virtual HRESULT 	Resize(UINT uWidth, UINT uHeight) { return (this->*pResize)(uWidth, uHeight); }
	virtual bool		IsDeviceLost() { return (this->*pIsDeviceLost)(); }
	virtual bool		IsDeviceReadyForReset() { return (this->*pIsDeviceReadyForReset)(); }
	virtual HRESULT 	D3DXFontOnLostDevice() { return (this->*pD3DXFontOnLostDevice)(); }
	virtual HRESULT 	D3DXFontOnResetDevice() { return (this->*pD3DXFontOnResetDevice)(); }
	virtual HRESULT 	UpdateDevice(PRESENT_PARAMETERS *pParams);
	virtual void		ReleaseD3D() { (this->*pReleaseD3D)(); }	
	virtual HRESULT 	GetBackBufferCached(UINT BackBuffer,DWORD Type, CSurface** ppBackBuffer);
	virtual DWORD		GetVertexSize(DWORD dwFVF);
	virtual bool		CopyFormatList(FORMAT *pSrc, FORMAT **pDest);
	virtual HRESULT 	WrapDevice(PRESENT_PARAMETERS *pParams) { return (this->*pWrapDevice)(pParams); }
	virtual void		ReleaseWrappers() { (this->*pReleaseWrappers)(); }
	
	virtual HRESULT 	Wrap(LPDIRECT3DTEXTURE9 pTexture, CTexture** ppTexture);
	virtual HRESULT 	Wrap(LPDIRECT3DCUBETEXTURE9 pTexture, CCubeTexture** ppTexture);
	virtual HRESULT 	Wrap(LPDIRECT3DVOLUMETEXTURE9 pTexture, CVolTexture** ppTexture);

	//
	// Generic Wrappers & Utilities
	//

	virtual bool		RenderPrimitive(D3DPRIMITIVETYPE PrimType, DWORD dwFVF,LPVOID pVertices, DWORD dwVertexCount, LPVOID pIndices, DWORD dwIndexCount, RPOPTIONS *pOptions); 
	virtual HRESULT 	ConvertToVertexBuffer(DWORD dwFVF, DWORD dwVertexCount, PVOID pVertexData, DWORD dwStride, DWORD dwUsage, DWORD dwPool, bool bFVFVB, CVertexBuffer** ppVB);
	virtual void		ConvertToStridedFormat(LPDRAWPRIMITIVESTRIDEDDATA pVertexData, DWORD dwVertexTypeDesc, PVOID pVertices, DWORD dwVertexSize);

	//
	// API cycling control functions
	//

	virtual void		SetAPI(RENDERPRIMITIVEAPI dwAPI);
	virtual RENDERPRIMITIVEAPI	GetAPI(void);
	virtual DWORD		RemapAPI(DWORD dwCurrentAPI, D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD dwVertexTypeDesc);
	virtual void		SetDestinationFVF(DWORD dwDestinationFVF);
	virtual DWORD		GetDestinationFVF(void);

protected:
	
	//
	// DX6
	//

	virtual HRESULT 	QueryInterface6(REFIID riid, LPVOID * ppvObj);
	virtual ULONG		AddRef6(void);
	virtual ULONG		Release6(void);
	
	virtual HRESULT 	GetVerticalBlankStatus6(LPBOOL lpbIsInVB);
	virtual HRESULT 	EvictManagedResources6(void);
	virtual UINT		GetAvailableTextureMem6(void);
	virtual HRESULT 	GetCaps6(LPD3DDEVICEDESC lpD3DDevDesc);
	virtual HRESULT 	EnumTextureFormats6(LPD3DENUMPIXELFORMATSCALLBACK lpd3dEnumPixelProc, LPVOID lpArg);
	virtual HRESULT 	BeginScene6(void);
	virtual HRESULT 	EndScene6(void);
	virtual HRESULT 	GetDirect3D6(LPDIRECT3D3* lplpD3D);
	virtual HRESULT 	SetRenderTarget6(CSurface *pNew,CSurface *pNewDepth);
	virtual HRESULT 	GetRenderTarget6(CSurface **ppRenderTarget);
	virtual HRESULT 	Clear6(DWORD dwCount,LPD3DRECT lpRects,DWORD dwFlags,D3DCOLOR dwColor,D3DVALUE dvZ,DWORD dwStencil);
	virtual HRESULT 	SetTransform6(D3DTRANSFORMSTATETYPE dtstTransformStateType,LPD3DMATRIX lpD3DMatrix);
	virtual HRESULT 	GetTransform6(D3DTRANSFORMSTATETYPE dtstTransformStateType,LPD3DMATRIX lpD3DMatrix);
	virtual HRESULT 	SetViewport6(LPVIEWPORT lpViewport);
	virtual HRESULT 	MultiplyTransform6(D3DTRANSFORMSTATETYPE dtstTransformStateType,LPD3DMATRIX lpD3DMatrix);
	virtual HRESULT 	GetViewport6(LPVIEWPORT lpViewport);
	virtual HRESULT 	SetMaterial6(LPMATERIAL lpMaterial);
	virtual HRESULT 	GetMaterial6(LPMATERIAL lpMaterial);
	virtual HRESULT 	SetLight6(DWORD dwLightIndex,LPLIGHT lpLight);
	virtual HRESULT 	GetLight6(DWORD dwLightIndex,LPLIGHT lpLight);
	virtual HRESULT 	LightEnable6(DWORD dwLightIndex,BOOL bEnable);
	virtual HRESULT 	CreateTexture6(UINT Width,UINT Height,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CTexture** ppTexture);
	virtual HRESULT 	SetLightState6(D3DLIGHTSTATETYPE dwState,DWORD dwValue);
	virtual HRESULT 	GetLightState6(D3DLIGHTSTATETYPE dwState,DWORD *pdwValue);
	virtual HRESULT 	SetRenderState6(D3DRENDERSTATETYPE dwState,DWORD dwValue);
	virtual HRESULT 	GetRenderState6(D3DRENDERSTATETYPE dwState,DWORD *pdwValue);
	virtual HRESULT 	SetClipStatus6(LPCLIPSTATUS pClipStatus);
	virtual HRESULT 	GetClipStatus6(LPCLIPSTATUS pClipStatus);
	virtual HRESULT 	ComputeSphereVisibility6(LPD3DVECTOR lpCenters,LPD3DVALUE lpRadii,DWORD dwNumSpheres,DWORD dwFlags,LPDWORD lpdwReturnValues);
	virtual HRESULT 	GetTexture6(int nIndex, CBaseTexture** ppTexture);
	virtual HRESULT 	SetTexture6(int nIndex, CBaseTexture *pTexture);
	virtual HRESULT 	GetTextureStageState6(DWORD dwStage,D3DTEXTURESTAGESTATETYPE dwState,LPDWORD lpdwValue);
	virtual HRESULT 	SetTextureStageState6(DWORD dwStage,D3DTEXTURESTAGESTATETYPE dwState,DWORD dwValue);	
	virtual HRESULT 	ValidateDevice6(LPDWORD lpdwPasses);
	virtual HRESULT 	CreateVertexBuffer6(UINT Length, DWORD Usage, DWORD FVF, DWORD Pool, CVertexBuffer** ppVB);
	virtual HRESULT 	DrawPrimitive6(D3DPRIMITIVETYPE dptPrimitiveType,DWORD dwVertexTypeDesc,LPVOID lpvVertices,DWORD dwVertexCount,DWORD dwFlags);
	virtual HRESULT 	DrawIndexedPrimitive6(D3DPRIMITIVETYPE d3dptPrimitiveType,DWORD dwVertexTypeDesc,LPVOID lpvVertices,DWORD dwVertexCount,LPWORD lpwIndices,DWORD dwIndexCount,DWORD dwFlags);
	virtual HRESULT 	DrawPrimitiveVB6(D3DPRIMITIVETYPE d3dptPrimitiveType,LPDIRECT3DVERTEXBUFFER lpd3dVertexBuffer,DWORD dwStartVertex,DWORD dwNumVertices,DWORD dwFlags);
	virtual HRESULT 	DrawIndexedPrimitiveVB6(D3DPRIMITIVETYPE d3dptPrimitiveType,LPDIRECT3DVERTEXBUFFER lpd3dVertexBuffer,DWORD dwStartVertex,DWORD dwNumVertices,LPWORD lpwIndices,DWORD dwIndexCount,DWORD dwFlags);
	virtual HRESULT 	DrawPrimitiveStrided6(D3DPRIMITIVETYPE dptPrimitiveType,DWORD dwVertexTypeDesc,LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray,DWORD dwVertexCount,DWORD dwFlags);
	virtual HRESULT 	DrawIndexedPrimitiveStrided6(D3DPRIMITIVETYPE dptPrimitiveType,DWORD dwVertexTypeDesc,LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray,DWORD dwVertexCount,LPWORD lpwIndices,DWORD dwIndexCount,DWORD dwFlags);
	virtual HRESULT 	BeginEnd6(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD  dwVertexTypeDesc, LPVOID lpvVertices, DWORD  dwVertexCount, LPWORD lpwIndices, DWORD	dwIndexCount, DWORD  dwFlags);
	virtual bool		DrawPrimitiveUserMemory6(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD  dwVertexTypeDesc,
							LPVOID lpVertices, DWORD  dwVertexCount, LPVOID lpIndices, DWORD  dwIndexCount, 
							DWORD  dwFlags, bool bIs32BitIndices = false);
	virtual bool		DrawPrimitiveVertexBuffer6(D3DPRIMITIVETYPE d3dptPrimitiveType, CVertexBuffer *pBuffer, 
							DWORD dwStartVertex, DWORD dwNumVertices, LPWORD lpwIndices, DWORD dwIndexCount, 
							DWORD dwFlags, DWORD dwUsage, DWORD dwPool, bool bIs32BitIndices);
	virtual bool		DrawPrimitiveStridedData6(D3DPRIMITIVETYPE d3dptPrimitiveType,
							DWORD  dwVertexTypeDesc, LPDRAWPRIMITIVESTRIDEDDATA lpVertexArray,
							DWORD  dwVertexCount, LPWORD lpwIndices, DWORD	dwIndexCount,
							DWORD  dwFlags, DWORD  dwUsage, DWORD  dwPool, bool bIs32BitIndices);
	
	// DX6 wrappers for DX8 calls
	virtual HRESULT 	Present6(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion);
	virtual HRESULT 	CreateAdditionalSwapChain6(PRESENT_PARAMETERS* pPresentationParameters, CSwapChain** ppSwapChain);
	virtual HRESULT 	CopyRects6(CSurface* pSourceSurface,RECT* pSourceRectsArray,UINT cRects,CSurface* pDestinationSurface,POINT* pDestPointsArray);
	virtual HRESULT 	UpdateTexture6(CBaseTexture* pSourceTexture,CBaseTexture* pDestinationTexture);
	virtual HRESULT 	GetDepthStencilSurface6(CSurface** ppZStencilSurface);
	virtual HRESULT 	CreateDepthStencilSurface6(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,CSurface **ppSurface);
	virtual HRESULT 	CreateRenderTarget6(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,BOOL Lockable,CSurface **ppSurface);
	virtual HRESULT 	CreateImageSurface6(UINT Width,UINT Height,FORMAT Format,CSurface** ppSurface);
	virtual HRESULT 	SetSamplerState6(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD Value);
	virtual HRESULT 	GetSamplerState6(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD *Value);

	//
	// DX7
	//

	virtual HRESULT 	QueryInterface7(REFIID riid, LPVOID * ppvObj);
	virtual ULONG		AddRef7(void);
	virtual ULONG		Release7(void);

	virtual HRESULT 	GetVerticalBlankStatus7(LPBOOL lpbIsInVB);
	virtual HRESULT 	EvictManagedResources7(void);
	virtual UINT		GetAvailableTextureMem7(void);
	virtual HRESULT 	GetCaps7(LPD3DDEVICEDESC7 lpD3DDevDesc);
	virtual HRESULT 	EnumTextureFormats7(LPD3DENUMPIXELFORMATSCALLBACK lpd3dEnumPixelProc, LPVOID lpArg);
	virtual HRESULT 	BeginScene7(void);
	virtual HRESULT 	EndScene7(void);
	virtual HRESULT 	GetDirect3D7(LPDIRECT3D7* lplpD3D);
	virtual HRESULT 	SetRenderTarget7(CSurface *pNew,CSurface *pNewDepth);
	virtual HRESULT 	GetRenderTarget7(CSurface **ppRenderTarget);
	virtual HRESULT 	Clear7(DWORD dwCount,LPD3DRECT lpRects,DWORD dwFlags,D3DCOLOR dwColor,D3DVALUE dvZ,DWORD dwStencil);
	virtual HRESULT 	SetTransform7(D3DTRANSFORMSTATETYPE dtstTransformStateType,LPD3DMATRIX lpD3DMatrix);
	virtual HRESULT 	GetTransform7(D3DTRANSFORMSTATETYPE dtstTransformStateType,LPD3DMATRIX lpD3DMatrix);
	virtual HRESULT 	SetViewport7(LPVIEWPORT lpViewport);
	virtual HRESULT 	MultiplyTransform7(D3DTRANSFORMSTATETYPE dtstTransformStateType,LPD3DMATRIX lpD3DMatrix);
	virtual HRESULT 	GetViewport7(LPVIEWPORT lpViewport);
	virtual HRESULT 	SetMaterial7(LPMATERIAL lpMaterial);
	virtual HRESULT 	GetMaterial7(LPMATERIAL lpMaterial);
	virtual HRESULT 	SetLight7(DWORD dwLightIndex,LPLIGHT lpLight);
	virtual HRESULT 	GetLight7(DWORD dwLightIndex,LPLIGHT lpLight);
	virtual HRESULT 	SetRenderState7(D3DRENDERSTATETYPE dwState,DWORD dwValue);
	virtual HRESULT 	GetRenderState7(D3DRENDERSTATETYPE dwState,DWORD *pdwValue);
	virtual HRESULT 	BeginStateBlock7(void);
	virtual HRESULT 	EndStateBlock7(UINT_PTR* lpdwBlockHandle);
	virtual HRESULT 	PreLoad7(LPDIRECTDRAWSURFACE7 lpddsTexture);
	virtual HRESULT 	SetClipStatus7(LPCLIPSTATUS pClipStatus);
	virtual HRESULT 	GetClipStatus7(LPCLIPSTATUS pClipStatus);
	virtual HRESULT 	ComputeSphereVisibility7(LPD3DVECTOR lpCenters,LPD3DVALUE lpRadii,DWORD dwNumSpheres,DWORD dwFlags,LPDWORD lpdwReturnValues);
	virtual HRESULT 	CreateTexture7(UINT Width,UINT Height,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CTexture** ppTexture);
	virtual HRESULT 	CreateCubeTexture7(UINT EdgeLength,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CCubeTexture** ppCubeTexture);
	virtual HRESULT 	GetTexture7(int nIndex, CBaseTexture** ppTexture);
	virtual HRESULT 	SetTexture7(int nIndex, CBaseTexture *pTexture);
	virtual HRESULT 	GetTextureStageState7(DWORD dwStage,D3DTEXTURESTAGESTATETYPE dwState,LPDWORD lpdwValue);
	virtual HRESULT 	SetTextureStageState7(DWORD dwStage,D3DTEXTURESTAGESTATETYPE dwState,DWORD dwValue);
	virtual HRESULT 	ValidateDevice7(LPDWORD lpdwPasses);
	virtual HRESULT 	ApplyStateBlock7(UINT_PTR dwBlockHandle);
	virtual HRESULT 	CaptureStateBlock7(UINT_PTR dwBlockHandle);
	virtual HRESULT 	DeleteStateBlock7(UINT_PTR dwBlockHandle);
	virtual HRESULT 	CreateStateBlock7(D3DSTATEBLOCKTYPE d3dsbType,UINT_PTR* lpdwBlockHandle);
	virtual HRESULT 	Load7(LPDIRECTDRAWSURFACE7 lpDestTex,LPPOINT lpDestPoint,LPDIRECTDRAWSURFACE7 lpSrcTex,LPRECT lprcSrcRect,DWORD dwFlags);
	virtual HRESULT 	LightEnable7(DWORD dwLightIndex,BOOL bEnable);
	virtual HRESULT 	GetLightEnable7(DWORD dwLightIndex,BOOL* pbEnable);
	virtual HRESULT 	SetClipPlane7(DWORD dwIndex,D3DVALUE* pPlaneEquation);
	virtual HRESULT 	GetClipPlane7(DWORD dwIndex,D3DVALUE* pPlaneEquation);
	virtual HRESULT 	GetInfo7(DWORD dwDevInfoID,LPVOID pDevInfoStruct,DWORD dwSize);
	virtual HRESULT 	CreateVertexBuffer7(UINT Length, DWORD Usage, DWORD FVF, DWORD Pool, CVertexBuffer** ppVB);
	virtual HRESULT 	DrawPrimitive7(D3DPRIMITIVETYPE dptPrimitiveType,DWORD dwVertexTypeDesc,LPVOID lpvVertices,DWORD dwVertexCount,DWORD dwFlags);
	virtual HRESULT 	DrawIndexedPrimitive7(D3DPRIMITIVETYPE d3dptPrimitiveType,DWORD dwVertexTypeDesc,LPVOID lpvVertices,DWORD dwVertexCount,LPWORD lpwIndices,DWORD dwIndexCount,DWORD dwFlags);
	virtual HRESULT 	DrawPrimitiveVB7(D3DPRIMITIVETYPE d3dptPrimitiveType,LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer,DWORD dwStartVertex,DWORD dwNumVertices,DWORD dwFlags);
	virtual HRESULT 	DrawIndexedPrimitiveVB7(D3DPRIMITIVETYPE d3dptPrimitiveType,LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer,DWORD dwStartVertex,DWORD dwNumVertices,LPWORD lpwIndices,DWORD dwIndexCount,DWORD dwFlags);
	virtual HRESULT 	DrawPrimitiveStrided7(D3DPRIMITIVETYPE dptPrimitiveType,DWORD dwVertexTypeDesc,LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray,DWORD dwVertexCount,DWORD dwFlags);
	virtual HRESULT 	DrawIndexedPrimitiveStrided7(D3DPRIMITIVETYPE dptPrimitiveType,DWORD dwVertexTypeDesc,LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray,DWORD dwVertexCount,LPWORD lpwIndices,DWORD dwIndexCount,DWORD dwFlags);
	virtual bool		DrawPrimitiveUserMemory7(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD  dwVertexTypeDesc,
							LPVOID lpVertices, DWORD  dwVertexCount, LPVOID lpIndices, DWORD  dwIndexCount, 
							DWORD  dwFlags, bool bIs32BitIndices = false);
	virtual bool		DrawPrimitiveVertexBuffer7(D3DPRIMITIVETYPE d3dptPrimitiveType, CVertexBuffer *pBuffer, 
							DWORD dwStartVertex, DWORD dwNumVertices, LPWORD lpwIndices, DWORD dwIndexCount, 
							DWORD dwFlags, DWORD dwUsage, DWORD dwPool, bool bIs32BitIndices);
	virtual bool		DrawPrimitiveStridedData7(D3DPRIMITIVETYPE d3dptPrimitiveType,
							DWORD  dwVertexTypeDesc, LPDRAWPRIMITIVESTRIDEDDATA lpVertexArray,
							DWORD  dwVertexCount, LPWORD lpwIndices, DWORD	dwIndexCount,
							DWORD  dwFlags, DWORD  dwUsage, DWORD  dwPool, bool bIs32BitIndices);

	// DX7 wrappers for DX8 calls
	virtual HRESULT 	Present7(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion);
	virtual HRESULT 	CreateAdditionalSwapChain7(PRESENT_PARAMETERS* pPresentationParameters, CSwapChain** ppSwapChain);
	virtual HRESULT 	CopyRects7(CSurface* pSourceSurface,RECT* pSourceRectsArray,UINT cRects,CSurface* pDestinationSurface,POINT* pDestPointsArray);
	virtual HRESULT 	UpdateTexture7(CBaseTexture* pSourceTexture,CBaseTexture* pDestinationTexture);
	virtual HRESULT 	GetDepthStencilSurface7(CSurface** ppZStencilSurface);
	virtual HRESULT 	CreateDepthStencilSurface7(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,CSurface **ppSurface);
	virtual HRESULT 	CreateRenderTarget7(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,BOOL Lockable,CSurface **ppSurface);
	virtual HRESULT 	CreateImageSurface7(UINT Width,UINT Height,FORMAT Format,CSurface** ppSurface);
	virtual HRESULT 	SetSamplerState7(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD Value);
	virtual HRESULT 	GetSamplerState7(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD *Value);

	//
	// DX8
	//

	virtual HRESULT 	UpdateSurface9ToCopyRects8(CSurface* pSourceSurface,RECT* pSourceRects,CSurface* pDestinationSurface,POINT* pDestPoint);
	virtual HRESULT 	QueryInterface8(REFIID riid, void** ppvObj);
	virtual ULONG		AddRef8(void);
	virtual ULONG		Release8(void);
	virtual HRESULT 	TestCooperativeLevel8(void);
	virtual UINT		GetAvailableTextureMem8(void);
	virtual HRESULT 	EvictManagedResources8();
	virtual HRESULT 	ResourceManagerDiscardBytes8(DWORD Bytes);
	virtual HRESULT 	GetDirect3D8(IDirect3D8** ppD3D8);
	virtual HRESULT 	GetDeviceCaps8(CAPS* pCaps);
	virtual HRESULT 	GetDisplayMode8(DISPLAYMODE* pMode);
	virtual HRESULT 	GetCreationParameters8(DEVICE_CREATION_PARAMETERS *pParameters);
	virtual HRESULT 	SetCursorProperties8(UINT XHotSpot,UINT YHotSpot,CSurface* pCursorBitmap);
	virtual void		SetCursorPosition8(UINT XScreenSpace,UINT YScreenSpace,DWORD Flags);
	virtual BOOL		ShowCursor8(BOOL bShow);
	virtual HRESULT 	CreateAdditionalSwapChain8(PRESENT_PARAMETERS* pPresentationParameters, CSwapChain** ppSwapChain);
	virtual HRESULT 	Reset8(PRESENT_PARAMETERS* pPresentationParameters);
	virtual HRESULT 	Present8(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion);
	virtual HRESULT 	GetBackBuffer8(UINT BackBuffer,DWORD Type,CSurface** ppBackBuffer);
	virtual HRESULT 	GetRasterStatus8(RASTER_STATUS* pRasterStatus);
	virtual void		SetGammaRamp8(DWORD Flags,GAMMARAMP* pRamp);
	virtual void		GetGammaRamp8(GAMMARAMP* pRamp);
	virtual HRESULT 	CreateTexture8(UINT Width,UINT Height,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CTexture** ppTexture);
	virtual HRESULT 	CreateVolumeTexture8(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CVolTexture** ppVolumeTexture);
	virtual HRESULT 	CreateCubeTexture8(UINT EdgeLength,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CCubeTexture** ppCubeTexture);
	virtual HRESULT 	CreateRenderTarget8(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,BOOL Lockable,CSurface **ppSurface);
	virtual HRESULT 	CreateDepthStencilSurface8(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,CSurface **ppSurface);
	virtual HRESULT 	CreateImageSurface8(UINT Width,UINT Height,FORMAT Format,CSurface** ppSurface);
	virtual HRESULT 	SetSamplerState8(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD Value);
	virtual HRESULT 	GetSamplerState8(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD *Value);
	virtual HRESULT 	SetSoftwareVertexProcessing8(BOOL bSoftware);
	virtual BOOL		GetSoftwareVertexProcessing8();
	virtual HRESULT 	CopyRects8(CSurface* pSourceSurface,RECT* pSourceRectsArray,UINT cRects,CSurface* pDestinationSurface,POINT* pDestPointsArray);
	virtual HRESULT 	UpdateTexture8(CBaseTexture* pSourceTexture,CBaseTexture* pDestinationTexture);
	virtual HRESULT 	GetFrontBuffer8(CSurface* pDestSurface);
	virtual HRESULT 	SetRenderTarget8(CSurface *pNew, CSurface *pNewDepth);
	virtual HRESULT 	GetRenderTarget8(CSurface **ppRenderTarget);
	virtual HRESULT 	GetDepthStencilSurface8(CSurface** ppZStencilSurface);
	virtual HRESULT 	BeginScene8(void);
	virtual HRESULT 	EndScene8(void);
	virtual HRESULT 	Clear8(DWORD Count,D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);
	virtual HRESULT 	SetTransform8(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix);
	virtual HRESULT 	GetTransform8(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix);
	virtual HRESULT 	MultiplyTransform8(D3DTRANSFORMSTATETYPE,D3DMATRIX*);
	virtual HRESULT 	SetViewport8(VIEWPORT* pViewport);
	virtual HRESULT 	GetViewport8(VIEWPORT* pViewport);
	virtual HRESULT 	SetMaterial8(MATERIAL* pMaterial);
	virtual HRESULT 	GetMaterial8(MATERIAL* pMaterial);
	virtual HRESULT 	SetLight8(DWORD Index,LIGHT*);
	virtual HRESULT 	GetLight8(DWORD Index,LIGHT*);
	virtual HRESULT 	LightEnable8(DWORD Index,BOOL Enable);
	virtual HRESULT 	GetLightEnable8(DWORD Index,BOOL* pEnable);
	virtual HRESULT 	SetClipPlane8(DWORD Index,float* pPlane);
	virtual HRESULT 	GetClipPlane8(DWORD Index,float* pPlane);
	virtual HRESULT 	SetRenderState8(D3DRENDERSTATETYPE dwState,DWORD dwValue);
	virtual HRESULT 	GetRenderState8(D3DRENDERSTATETYPE dwState,DWORD *pdwValue);
	virtual HRESULT 	BeginStateBlock8(void);
	virtual HRESULT 	EndStateBlock8(UINT_PTR* pToken);
	virtual HRESULT 	ApplyStateBlock8(UINT_PTR Token);
	virtual HRESULT 	CaptureStateBlock8(UINT_PTR Token);
	virtual HRESULT 	DeleteStateBlock8(UINT_PTR Token);
	virtual HRESULT 	CreateStateBlock8(D3DSTATEBLOCKTYPE Type,UINT_PTR* pToken);
	virtual HRESULT 	SetClipStatus8(LPCLIPSTATUS pClipStatus);
	virtual HRESULT 	GetClipStatus8(LPCLIPSTATUS pClipStatus);
	virtual HRESULT 	GetTexture8(int nIndex,CBaseTexture** ppTexture);
	virtual HRESULT 	SetTexture8(int nIndex, CBaseTexture *pTextures);
	virtual HRESULT 	GetTextureStageState8(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue);
	virtual HRESULT 	SetTextureStageState8(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);
	virtual HRESULT 	ValidateDevice8(DWORD* pNumPasses);
	virtual HRESULT 	GetInfo8(DWORD DevInfoID,void* pDevInfoStruct,DWORD DevInfoStructSize);
	virtual HRESULT 	SetPaletteEntries8(UINT PaletteNumber,PALETTEENTRY* pEntries);
	virtual HRESULT 	GetPaletteEntries8(UINT PaletteNumber,PALETTEENTRY* pEntries);
	virtual HRESULT 	SetCurrentTexturePalette8(UINT PaletteNumber);
	virtual HRESULT 	GetCurrentTexturePalette8(UINT *PaletteNumber);
	virtual HRESULT 	CreateVertexShader8(DWORD* pDeclaration,DWORD* pFunction,CVertexShader** pHandle,DWORD Usage);
	virtual HRESULT 	SetVertexShaderInt8(CVertexShader* pShader);
	virtual HRESULT 	SetVertexShader8(DWORD Handle);
	virtual HRESULT 	GetVertexShader8(DWORD* pHandle);
	virtual HRESULT 	DeleteVertexShader8(CVertexShader* pShader);
	virtual HRESULT 	SetVertexShaderConstant8(UINT Register, CONST float* pConstantData,UINT ConstantCount);
	virtual HRESULT 	GetVertexShaderConstant8(UINT Register, float* pConstantData,UINT ConstantCount);
	virtual HRESULT 	GetVertexShaderDeclaration8(DWORD Handle,void* pData,DWORD *pSizeOfData);
	virtual HRESULT 	GetVertexShaderFunction8(DWORD Handle,void* pData,DWORD *pSizeOfData);
	virtual HRESULT 	SetFVF8(DWORD dwFVF);
	virtual HRESULT 	SetStreamSource8(UINT streamNumber, CVertexBuffer  *pStreamData,  UINT	stride, UINT offset);
	virtual HRESULT 	GetStreamSource8(UINT streamNumber, CVertexBuffer **ppStreamData, UINT *pStride);
	virtual HRESULT 	SetIndices8(CIndexBuffer  *pIndexData,	UINT  baseVertexIndex);
	virtual HRESULT 	GetIndices8(CIndexBuffer **ppIndexData, UINT *pBaseVertexIndex);
	virtual HRESULT 	CreatePixelShader8(DWORD* pFunction,DWORD* pHandle);
	virtual HRESULT 	SetPixelShader8(DWORD Handle);
	virtual HRESULT 	GetPixelShader8(DWORD* pHandle);
	virtual HRESULT 	DeletePixelShader8(DWORD Handle);
	virtual HRESULT 	CreatePixelShaderInt8(DWORD* pFunction, CPShader** pShader);	
	virtual HRESULT 	SetPixelShaderInt8(CPShader *pShader);
	virtual HRESULT 	GetPixelShaderInt8(CPShader **pShader);
	virtual HRESULT 	DeletePixelShaderInt8(CPShader *pShader);
	virtual HRESULT 	SetPixelShaderConstant8(UINT Register,const float* pConstantData,UINT ConstantCount);
	virtual HRESULT 	GetPixelShaderConstant8(UINT Register,float* pConstantData,UINT ConstantCount);
	virtual HRESULT 	GetPixelShaderFunction8(DWORD Handle,void* pData,DWORD* pSizeOfData);
	virtual HRESULT 	CreateVertexBuffer8(UINT Length, DWORD Usage, DWORD FVF, DWORD Pool, CVertexBuffer** ppVB);
	virtual HRESULT 	CreateIndexBuffer8(UINT Length, DWORD Usage, FMT Format, DWORD Pool, CIndexBuffer** ppIB);
	virtual HRESULT 	ProcessVertices8(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,CVertexBuffer* pDestBuffer,CVertexDeclaration *pDecl,DWORD Flags);
	virtual HRESULT 	DrawPrimitive8(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
	virtual HRESULT 	DrawIndexedPrimitive8(D3DPRIMITIVETYPE,UINT minIndex,UINT NumVertices,UINT startIndex,UINT primCount,INT BaseVertexIndex);
	virtual HRESULT 	DrawPrimitiveUP8(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	virtual HRESULT 	DrawIndexedPrimitiveUP8(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertexIndices,UINT PrimitiveCount,void* pIndexData,FORMAT IndexDataFormat,void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	virtual HRESULT 	DrawRectPatch8(UINT Handle,float* pNumSegs,RECTPATCH_INFO* pRectPatchInfo);
	virtual HRESULT 	DrawTriPatch8(UINT Handle,float* pNumSegs,TRIPATCH_INFO* pTriPatchInfo);
	virtual HRESULT 	DeletePatch8(UINT Handle);
	virtual bool		DrawPrimitiveUserMemory8(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD  dwVertexTypeDesc,
							LPVOID lpVertices, DWORD  dwVertexCount, LPVOID lpIndices, DWORD  dwIndexCount, 
							DWORD  dwFlags, bool bIs32BitIndices = false);
	virtual bool		DrawPrimitiveVertexBuffer8(D3DPRIMITIVETYPE d3dptPrimitiveType, CVertexBuffer *pBuffer, 
							DWORD dwStartVertex, DWORD dwNumVertices, LPWORD lpwIndices, DWORD dwIndexCount, 
							DWORD dwFlags, DWORD dwUsage, DWORD dwPool, bool bIs32BitIndices);
	virtual bool		DrawPrimitiveStridedData8(D3DPRIMITIVETYPE d3dptPrimitiveType,
							DWORD  dwVertexTypeDesc, LPDRAWPRIMITIVESTRIDEDDATA lpVertexArray,
							DWORD  dwVertexCount, LPWORD lpwIndices, DWORD	dwIndexCount,
							DWORD  dwFlags, DWORD  dwUsage, DWORD  dwPool, bool bIs32BitIndices);
	virtual HRESULT 	SetNPatchMode8(float fSegments);

	//
	// DX9
	//

	virtual HRESULT 	QueryInterface9(REFIID riid, void** ppvObj);
	virtual ULONG		AddRef9(void);
	virtual ULONG		Release9(void);
	virtual HRESULT 	TestCooperativeLevel9(void);
	virtual HRESULT 	EvictManagedResources9(void);
	virtual UINT		GetAvailableTextureMem9(void);
	virtual HRESULT 	GetDirect3D9(IDirect3D9** ppD3D9);
	virtual HRESULT 	GetDeviceCaps9(CAPS* pCaps);
	virtual HRESULT 	GetDisplayMode8To9(DISPLAYMODE* pMode);
	virtual HRESULT 	GetDisplayMode9(UINT uSwapChain, DISPLAYMODE* pMode);
	virtual HRESULT 	GetCreationParameters9(DEVICE_CREATION_PARAMETERS *pParameters);
	virtual HRESULT 	SetCursorProperties9(UINT XHotSpot,UINT YHotSpot,CSurface* pCursorBitmap);
	virtual void		SetCursorPosition9(UINT XScreenSpace,UINT YScreenSpace,DWORD Flags);
	virtual BOOL		ShowCursor9(BOOL bShow);
	virtual HRESULT 	CreateAdditionalSwapChain9(PRESENT_PARAMETERS* pPresentationParameters, CSwapChain** ppSwapChain);
	virtual HRESULT 	Reset9(PRESENT_PARAMETERS* pPresentationParameters);
	virtual HRESULT 	Present9(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion);
	virtual HRESULT 	GetBackBuffer8To9(UINT BackBuffer,DWORD Type,CSurface** ppBackBuffer);
	virtual HRESULT 	GetBackBuffer9(UINT iSwapChain,UINT BackBuffer,DWORD Type,CSurface** ppBackBuffer);
	virtual HRESULT 	GetRasterStatus8To9(RASTER_STATUS* pRasterStatus);
	virtual void		SetGammaRamp8To9(DWORD Flags,GAMMARAMP* pRamp);
	virtual void		GetGammaRamp8To9(GAMMARAMP* pRamp);
	virtual HRESULT 	GetRasterStatus9(UINT uSwapchain, RASTER_STATUS* pRasterStatus);
	virtual void		SetGammaRamp9(UINT uSwapchain, DWORD Flags,GAMMARAMP* pRamp);
	virtual void		GetGammaRamp9(UINT uSwapchain, GAMMARAMP* pRamp);
	virtual HRESULT 	CreateImageSurface9(UINT Width,UINT Height,FORMAT Format,CSurface** ppSurface);
	virtual HRESULT 	CreateTexture8To9(UINT Width,UINT Height,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CTexture** ppTexture);
	virtual HRESULT 	CreateTexture9(UINT Width,UINT Height,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CTexture** ppTexture, HANDLE *pSharedHandle);
	virtual HRESULT 	CreateVolumeTexture8To9(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CVolTexture** ppVolumeTexture);
	virtual HRESULT 	CreateVolumeTexture9(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CVolTexture** ppVolumeTexture, HANDLE *pSharedHandle);
	virtual HRESULT 	CreateCubeTexture8To9(UINT EdgeLength,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CCubeTexture** ppCubeTexture);
	virtual HRESULT 	CreateCubeTexture9(UINT EdgeLength,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CCubeTexture** ppCubeTexture, HANDLE *pSharedHandle);
	virtual HRESULT 	CreateRenderTarget8To9(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,BOOL Lockable,CSurface **ppSurface);
	virtual HRESULT 	CreateRenderTarget9(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,CSurface **ppSurface, HANDLE *pSharedHandle = NULL);
	virtual HRESULT 	CreateDepthStencilSurface8To9(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,CSurface **ppSurface);
	virtual HRESULT 	CreateDepthStencilSurface9(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard, CSurface **ppSurface, HANDLE *pSharedHandle = NULL);
	virtual HRESULT 	UpdateSurface9(CSurface* pSourceSurface,RECT* pSourceRects,CSurface* pDestinationSurface,POINT* pDestPoints);
	virtual HRESULT 	UpdateTexture9(CBaseTexture* pSourceTexture,CBaseTexture* pDestinationTexture);
	virtual HRESULT 	GetFrontBuffer8To9(CSurface* pDestSurface);
	virtual HRESULT 	GetFrontBufferData9(UINT iSwapChain, CSurface* pDestSurface);
	virtual HRESULT 	GetRenderTargetData9(CSurface* pSrcRenderTarget, CSurface* pDestSurface);
	virtual HRESULT 	SetRenderTarget8To9(CSurface *pNew, CSurface *pNewDepth);
	virtual HRESULT 	GetRenderTarget8To9(CSurface **ppRenderTarget);
	virtual HRESULT 	SetRenderTarget9(DWORD RenderTargetIndex, CSurface *pNew);
	virtual HRESULT 	GetRenderTarget9(DWORD RenderTargetIndex, CSurface **ppRenderTarget);
	virtual HRESULT 	SetDepthStencilSurface9(CSurface *pNewDepth);
	virtual HRESULT 	GetDepthStencilSurface9(CSurface** ppZStencilSurface);
	virtual HRESULT 	BeginScene9(void);
	virtual HRESULT 	EndScene9(void);
	virtual HRESULT 	Clear9(DWORD Count,D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);
	virtual HRESULT 	SetTransform9(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix);
	virtual HRESULT 	GetTransform9(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix);
	virtual HRESULT 	MultiplyTransform9(D3DTRANSFORMSTATETYPE,D3DMATRIX*);
	virtual HRESULT 	SetViewport9(VIEWPORT* pViewport);
	virtual HRESULT 	GetViewport9(VIEWPORT* pViewport);
	virtual HRESULT 	SetMaterial9(MATERIAL* pMaterial);
	virtual HRESULT 	GetMaterial9(MATERIAL* pMaterial);
	virtual HRESULT 	SetLight9(DWORD Index,LIGHT*);
	virtual HRESULT 	GetLight9(DWORD Index,LIGHT*);
	virtual HRESULT 	LightEnable9(DWORD Index,BOOL Enable);
	virtual HRESULT 	GetLightEnable9(DWORD Index,BOOL* pEnable);
	virtual HRESULT 	SetClipPlane9(DWORD Index,float* pPlane);
	virtual HRESULT 	GetClipPlane9(DWORD Index,float* pPlane);
	virtual HRESULT 	SetRenderState9(D3DRENDERSTATETYPE dwState,DWORD dwValue);
	virtual HRESULT 	GetRenderState9(D3DRENDERSTATETYPE dwState,DWORD *pdwValue);
	virtual HRESULT 	BeginStateBlock9(void);
	virtual HRESULT 	EndStateBlock9(UINT_PTR* pToken);
	virtual HRESULT 	EndStateBlockInterface9(CStateblock** pStateblock);
	virtual HRESULT 	ApplyStateBlock9(UINT_PTR Token);
	virtual HRESULT 	CaptureStateBlock9(UINT_PTR Token);
	virtual HRESULT 	DeleteStateBlock9(UINT_PTR Token);
	virtual HRESULT 	CreateStateBlock9(D3DSTATEBLOCKTYPE Type,UINT_PTR* pToken);
	virtual HRESULT 	CreateStateBlockInterface9(D3DSTATEBLOCKTYPE Type,CStateblock** pStateblock);
	virtual HRESULT 	SetClipStatus9(LPCLIPSTATUS pClipStatus);
	virtual HRESULT 	GetClipStatus9(LPCLIPSTATUS pClipStatus);
	virtual HRESULT 	GetTexture9(int nIndex,CBaseTexture** ppTexture);
	virtual HRESULT 	SetTexture9(int nIndex, CBaseTexture *pTextures);
	virtual HRESULT 	GetTextureStageState9(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue);
	virtual HRESULT 	SetTextureStageState9(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);
	virtual HRESULT 	ValidateDevice9(DWORD* pNumPasses);
	virtual HRESULT 	GetInfo9(DWORD DevInfoID,void* pDevInfoStruct,DWORD DevInfoStructSize);
	virtual HRESULT 	SetPaletteEntries9(UINT PaletteNumber,PALETTEENTRY* pEntries);
	virtual HRESULT 	GetPaletteEntries9(UINT PaletteNumber,PALETTEENTRY* pEntries);
	virtual HRESULT 	SetCurrentTexturePalette9(UINT PaletteNumber);
	virtual HRESULT 	GetCurrentTexturePalette9(UINT *PaletteNumber);
	virtual HRESULT 	CreateVertexDeclaration9(VERTEXELEMENT *pVE, CVertexDeclaration **pDecl);
	virtual HRESULT 	CreateVertexShader9(DWORD* pDeclaration,DWORD* pFunction,CVertexShader** pHandle,DWORD Usage);
	virtual HRESULT 	CreateVertexShaderInt9(DWORD* pFunction, CVertexShader **pShader);
	virtual HRESULT 	DeleteVertexShader9(CVertexShader* pVS);
	virtual HRESULT 	SetVertexShaderInt9(CVertexShader *pVS);
	virtual HRESULT 	SetVertexDeclaration9(CVertexDeclaration *pDecl);
	virtual HRESULT 	SetFVF9(DWORD dwFVF);
	virtual HRESULT 	GetVertexShaderInt9(CVertexShader** pHandle);
	virtual HRESULT 	SetStreamSource9(UINT streamNumber, CVertexBuffer  *pStreamData,  UINT	stride, UINT offset);
	virtual HRESULT 	GetStreamSource9(UINT streamNumber, CVertexBuffer **ppStreamData, UINT *pStride);
	virtual HRESULT 	SetIndices9(CIndexBuffer  *pIndexData,	UINT  baseVertexIndex);
	virtual HRESULT 	GetIndices9(CIndexBuffer **ppIndexData, UINT *pBaseVertexIndex);
	virtual HRESULT 	SetPixelShaderConstantF9(UINT StartRegister,const float* pConstantData,UINT Vector4fCount);
	virtual HRESULT 	GetPixelShaderConstantF9(UINT StartRegister,float* pConstantData,UINT Vector4fCount);
	virtual HRESULT 	SetPixelShaderConstantI9(UINT StartRegister,const int* pConstantData,UINT Vector4iCount);
	virtual HRESULT 	GetPixelShaderConstantI9(UINT StartRegister,int* pConstantData,UINT Vector4iCount);
	virtual HRESULT 	SetPixelShaderConstantB9(UINT StartRegister,const BOOL* pConstantData,UINT BoolCount);
	virtual HRESULT 	GetPixelShaderConstantB9(UINT StartRegister,BOOL* pConstantData,UINT BoolCount);
	virtual HRESULT 	CreateVertexBuffer8To9(UINT Length, DWORD Usage, DWORD FVF, DWORD Pool, CVertexBuffer** ppVB);
	virtual HRESULT 	CreateVertexBuffer9(UINT Length, DWORD Usage, DWORD FVF, DWORD Pool, CVertexBuffer** ppVB, HANDLE *pSharedHandle);
	virtual HRESULT 	CreateIndexBuffer8To9(UINT Length, DWORD Usage, FMT Format, DWORD Pool, CIndexBuffer** ppIB);
	virtual HRESULT 	CreateIndexBuffer9(UINT Length, DWORD Usage, FMT Format, DWORD Pool, CIndexBuffer** ppIB, HANDLE *pSharedHandle);
	virtual HRESULT 	ProcessVertices9(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,CVertexBuffer* pDestBuffer,CVertexDeclaration *pDecl, DWORD Flags);
	virtual HRESULT 	DrawPrimitive9(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
	virtual HRESULT 	DrawIndexedPrimitive9(D3DPRIMITIVETYPE,UINT minIndex,UINT NumVertices,UINT startIndex,UINT primCount,INT BaseVertexIndex);
	virtual HRESULT 	DrawPrimitiveUP9(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	virtual HRESULT 	DrawIndexedPrimitiveUP9(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertexIndices,UINT PrimitiveCount,void* pIndexData,FORMAT IndexDataFormat,void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	virtual HRESULT 	DrawRectPatch9(UINT Handle,float* pNumSegs,RECTPATCH_INFO* pRectPatchInfo);
	virtual HRESULT 	DrawTriPatch9(UINT Handle,float* pNumSegs,TRIPATCH_INFO* pTriPatchInfo);
	virtual HRESULT 	DeletePatch9(UINT Handle);
	virtual HRESULT 	SetNPatchMode9(float nSegments);
	virtual float		GetNPatchMode9();
	virtual bool		DrawPrimitiveUserMemory9(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD  dwVertexTypeDesc,
							LPVOID lpVertices, DWORD  dwVertexCount, LPVOID lpIndices, DWORD  dwIndexCount, 
							DWORD  dwFlags, bool bIs32BitIndices = false);
	virtual bool		DrawPrimitiveVertexBuffer9(D3DPRIMITIVETYPE d3dptPrimitiveType, CVertexBuffer *pBuffer, 
							DWORD dwStartVertex, DWORD dwNumVertices, LPWORD lpwIndices, DWORD dwIndexCount, 
							DWORD dwFlags, DWORD dwUsage, DWORD dwPool, bool bIs32BitIndices);
	virtual bool		DrawPrimitiveStridedData9(D3DPRIMITIVETYPE d3dptPrimitiveType,
							DWORD  dwVertexTypeDesc, LPDRAWPRIMITIVESTRIDEDDATA lpVertexArray,
							DWORD  dwVertexCount, LPWORD lpwIndices, DWORD	dwIndexCount,
							DWORD  dwFlags, DWORD  dwUsage, DWORD  dwPool, bool bIs32BitIndices);
	virtual HRESULT 	StretchRect9(CSurface* pSourceSurface,CONST RECT* pSourceRect,CSurface* pDestSurface,CONST RECT* pDestRect, DWORD dwFilter);
	virtual HRESULT 	ColorFill9(CSurface* pSurface,CONST RECT* pRect,DWORD color);
	virtual HRESULT 	CreateOffscreenPlainSurface9(UINT Width,UINT Height,FORMAT Format,DWORD dwPool, CSurface** ppSurface, HANDLE *pSharedHandle = NULL);
	virtual HRESULT 	SetScissorRect9(CONST RECT* pRect);
	virtual HRESULT 	GetScissorRect9(RECT* pRect);
	virtual HRESULT 	GetVertexDeclaration9(CVertexDeclaration** ppDecl);
	virtual HRESULT 	GetFVF9(DWORD* pFVF);
	virtual HRESULT 	SetVertexShaderConstantF9(UINT Register,CONST float* pConstantData,UINT ConstantCount);
	virtual HRESULT 	GetVertexShaderConstantF9(UINT Register,float* pConstantData,UINT ConstantCount);
	virtual HRESULT 	SetVertexShaderConstantI9(UINT Register,CONST int* pConstantData,UINT ConstantCount);
	virtual HRESULT 	GetVertexShaderConstantI9(UINT Register,int* pConstantData,UINT ConstantCount);
	virtual HRESULT 	SetVertexShaderConstantB9(UINT StartRegister,CONST BOOL* pConstantData, UINT BoolCount);
	virtual HRESULT 	GetVertexShaderConstantB9(UINT StartRegister,BOOL* pConstantData, UINT BoolCount);
	virtual HRESULT 	SetPixelShader9(CPShader* pShader);
	virtual HRESULT 	GetPixelShader9(CPShader** ppShader);
	virtual HRESULT 	CreatePixelShader9(DWORD* pFunction,CPShader** pShader);
	virtual HRESULT 	DeletePixelShader9(CPShader* pShader);
	virtual HRESULT 	SetSamplerState9(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD Value);
	virtual HRESULT 	GetSamplerState9(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD *Value);
	virtual HRESULT 	SetDialogBoxMode9(BOOL bEnableDialogs);
	virtual HRESULT 	SetSoftwareVertexProcessing9(BOOL bSoftware);
	virtual BOOL		GetSoftwareVertexProcessing9();
	virtual HRESULT 	GetSwapChain9(UINT iSwapChain,CSwapChain** pSwapChain);
	virtual UINT		GetNumberOfSwapChains9();
	virtual HRESULT 	CreateQuery9(QUERYTYPE Type, CQuery** pQuery);
	virtual HRESULT 	SetStreamSourceFreq9(UINT nStreamNumber, UINT nDivider);
	virtual HRESULT 	GetStreamSourceFreq9(UINT nStreamNumber, UINT* pDividerOut);
	virtual HRESULT 	SetConvolutionMonoKernel9(UINT uWidth, UINT uHeight, float * pfRow, float * pfCol);
	virtual HRESULT 	ComposeRects9( LPSURFACE pSrc, LPSURFACE pDst, LPVERTEXBUFFER pSrcRectDescs, UINT NumRects, LPVERTEXBUFFER pDstRectDescs, UINT Operation, int Xoffset, int Yoffset );
	virtual HRESULT 	PresentEx9( CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags );
	virtual HRESULT		ResetEx9( PRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode );
	virtual HRESULT		GetGPUThreadPriority9(INT *);
	virtual HRESULT		SetGPUThreadPriority9(INT);
	virtual HRESULT 	WaitForVBlank9(UINT SwapChainIndex);
	virtual HRESULT 	CheckResourceResidency9(CResource** pResourceArray, UINT numResources);
	virtual HRESULT 	SetMaximumFrameLatency9(UINT MaxLatency);
	virtual HRESULT 	GetMaximumFrameLatency9(UINT* pMaxLatency);
	virtual HRESULT 	CheckDeviceState9(HWND hWindow);
	virtual HRESULT 	CreateRenderTargetEx9(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,CSurface **ppSurface, HANDLE *pSharedHandle = NULL, DWORD dwUsage = 0);
	virtual HRESULT 	CreateOffscreenPlainSurfaceEx9(UINT Width, UINT Height, FORMAT Format, DWORD dwPool, CSurface** ppSurface, HANDLE *pSharedHandle = NULL, DWORD dwUsage = 0);
	virtual HRESULT 	CreateDepthStencilSurfaceEx9(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard, CSurface **ppSurface, HANDLE *pSharedHandle = NULL, DWORD dwUsage = 0);
	virtual HRESULT 	ExtractResource9(PVOID pResourceIn, RESOURCETYPE type, PVOID* ppResourceOut);


public:
	// D3DX -- just those that are used by version independent tests
	virtual void		D3DXInitializePSGP9();
	virtual HRESULT 	D3DXLoadVolumeFromMemory9(CVolume* pDestVolume,CONST PALETTEENTRY* pDestPalette,CONST BOX* pDestBox,LPCVOID  pSrcMemory,FORMAT SrcFormat,UINT SrcRowPitch,UINT SrcSlicePitch,CONST PALETTEENTRY* pSrcPalette,CONST BOX* pSrcBox,DWORD Filter,D3DCOLOR ColorKey);
	virtual HRESULT 	D3DXLoadVolumeFromVolume9(CVolume* pDestVolume,CONST PALETTEENTRY* pDestPalette,CONST BOX* pDestBox,CVolume* pSrcVolume,CONST PALETTEENTRY* pSrcPalette,CONST BOX* pSrcBox,DWORD Filter,D3DCOLOR ColorKey);
	virtual HRESULT 	D3DXSaveSurfaceToFileA9 (LPCSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect);
	virtual HRESULT 	D3DXSaveTextureToFileA9 (LPCSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat, LPBASETEXTURE pSrcTexture, CONST PALETTEENTRY* pSrcPalette);
						//NOTE: only D3DX tests should use D3DXEffect functions which operate directly on LPD3DXEFFECT interfaces.
						//all other tests should use the wrapper functions and wrapper objects CEffect & CnEffect
	virtual HRESULT 	D3DXEffect_CloneEffect9(LPD3DXEFFECT pEffect, CDevice* pTargetDevice, LPD3DXEFFECT* ppResultEffect);
	virtual HRESULT 	D3DXCreateEffectFromFile9(LPCTSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors);
	virtual HRESULT 	CreateEffectFromFile9(LPCTSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, CEffect** ppEffect, LPD3DXBUFFER* ppCompilationErrors);
	virtual HRESULT 	D3DXCreateEffectFromResource9(HMODULE hSrcModule, LPCTSTR pSrcresource, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors);
	virtual HRESULT 	CreateEffectFromResource9(HMODULE hSrcModule, LPCTSTR pSrcresource, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, CEffect** ppEffect, LPD3DXBUFFER* ppCompilationErrors);
	virtual HRESULT 	D3DXFilterTexture9(CBaseTexture* pBaseTexture,CONST PALETTEENTRY* pPalette,UINT SrcLevel,DWORD MipFilter);
	virtual HRESULT 	D3DXCreateMesh9(DWORD NumFaces, DWORD NumVertices, DWORD Options, CONST DWORD* pDeclaration, LPD3DXMESH* ppMesh);
	virtual HRESULT 	D3DXCreateMeshFVF9(DWORD NumFaces, DWORD NumVertices, DWORD Options, DWORD FVF, LPD3DXMESH* ppMesh);	
	virtual HRESULT 	D3DXTessellateRectPatch9(CVertexBuffer* pVertexBuffer, CONST float* pNumSegs, VERTEXELEMENT* pInDecl, CONST RECTPATCH_INFO* pRectPatchInfo, LPD3DXMESH pMesh);
	virtual HRESULT 	D3DXTessellateTriPatch9(CVertexBuffer* pVertexBuffer, CONST float* pNumSegs, VERTEXELEMENT* pInDecl, CONST TRIPATCH_INFO* pTriPatchInfo, LPD3DXMESH pMesh);
	virtual HRESULT 	D3DXLoadSurfaceFromSurface9(CSurface* pDestSurface,CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey);
	virtual HRESULT 	D3DXLoadSurfaceFromFileInMemory9(CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCVOID pSrcData, UINT SrcData, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo);
	virtual HRESULT 	D3DXLoadSurfaceFromMemory9( CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCVOID pSrcMemory, D3DFORMAT SrcFormat, UINT SrcPitch, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey);
	virtual HRESULT 	D3DXLoadSurfaceFromFile9(CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCSTR sFilename, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo);
	virtual HRESULT 	D3DXFillTexture9( LPTEXTURE pTexture, void* pFunction, LPVOID pData );
	virtual HRESULT 	D3DXCreateTexture9(UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, CTexture** ppTexture);
	virtual HRESULT 	D3DXCreateTextureFromFileA9(LPCSTR pSrcFile, CTexture** ppTexture);
	virtual HRESULT 	D3DXCreateTextureFromFileExA9(LPCSTR pSrcFile, UINT Size, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, CTexture** ppTexture);
	virtual HRESULT 	D3DXCreateTextureFromFileInMemory9(LPCVOID pSrcData, UINT SrcData, CTexture** ppTexture);
	virtual HRESULT 	D3DXCreateTextureFromFileInMemoryEx9(LPCVOID pSrcData, UINT SrcData, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, CTexture** ppTexture);
	virtual HRESULT 	D3DXCreateTextureFromResourceA9(HMODULE hSrcModule, LPCSTR pSrcResource, CTexture** ppTexture);
	virtual HRESULT 	D3DXCreateTextureFromResourceExA9(HMODULE hSrcModule, LPCSTR pSrcResource, UINT Width, UINT Height, UINT MipLevels,
														DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo,
														PALETTEENTRY* pPalette, CTexture** ppTexture);
	
	virtual void		D3DXInitializePSGP8();
	virtual HRESULT 	GenerateTexture8(HRESULT hr, LPDIRECT3DTEXTURE8 pTexture, CTexture** ppTexture, CDevice* pDevice);
	virtual HRESULT 	D3DXFilterTexture8(CBaseTexture* pBaseTexture,CONST PALETTEENTRY* pPalette,UINT SrcLevel,DWORD MipFilter);
	virtual HRESULT 	D3DXLoadVolumeFromMemory8(CVolume* pDestVolume,CONST PALETTEENTRY* pDestPalette,CONST BOX* pDestBox,LPCVOID  pSrcMemory,FORMAT SrcFormat,UINT SrcRowPitch,UINT SrcSlicePitch,CONST PALETTEENTRY* pSrcPalette,CONST BOX* pSrcBox,DWORD Filter,D3DCOLOR ColorKey);
	virtual HRESULT 	D3DXLoadVolumeFromVolume8(CVolume* pDestVolume,CONST PALETTEENTRY* pDestPalette,CONST BOX* pDestBox,CVolume* pSrcVolume,CONST PALETTEENTRY* pSrcPalette,CONST BOX* pSrcBox,DWORD Filter,D3DCOLOR ColorKey);
	virtual HRESULT 	D3DXSaveSurfaceToFileA8 (LPCSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect);
	virtual HRESULT 	D3DXSaveTextureToFileA8 (LPCSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat, LPBASETEXTURE pSrcTexture, CONST PALETTEENTRY* pSrcPalette);
	virtual HRESULT 	D3DXCreateMesh8(DWORD NumFaces, DWORD NumVertices, DWORD Options, CONST DWORD* pDeclaration, LPD3DXMESH* ppMesh);
	virtual HRESULT 	D3DXCreateMeshFVF8(DWORD NumFaces, DWORD NumVertices, DWORD Options, DWORD FVF, LPD3DXMESH* ppMesh);
	virtual HRESULT 	D3DXLoadSurfaceFromSurface8(CSurface* pDestSurface,CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey);
	virtual HRESULT 	D3DXLoadSurfaceFromMemory8( CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCVOID pSrcMemory, D3DFORMAT SrcFormat, UINT SrcPitch, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey);
	virtual HRESULT 	D3DXLoadSurfaceFromFileInMemory8(CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCVOID pSrcData, UINT SrcData, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo);
	virtual HRESULT 	D3DXLoadSurfaceFromFile8(CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCSTR sFilename, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo);
	virtual HRESULT 	D3DXFillTexture8( LPTEXTURE pTexture, void* pFunction, LPVOID pData );
	virtual HRESULT 	D3DXCreateTexture8(UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, CTexture** ppTexture);
	virtual HRESULT 	D3DXCreateTextureFromFileA8(LPCSTR pSrcFile, CTexture** ppTexture);
	virtual HRESULT 	D3DXCreateTextureFromFileExA8(LPCSTR pSrcFile, UINT Size, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, CTexture** ppTexture);
	virtual HRESULT 	D3DXCreateTextureFromFileInMemory8(LPCVOID pSrcData, UINT SrcData, CTexture** ppTexture);
	virtual HRESULT 	D3DXCreateTextureFromFileInMemoryEx8(LPCVOID pSrcData, UINT SrcData, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, CTexture** ppTexture);
	virtual HRESULT 	D3DXCreateTextureFromResourceA8(HMODULE hSrcModule, LPCSTR pSrcResource, CTexture** ppTexture);
	virtual HRESULT 	D3DXCreateTextureFromResourceExA8(HMODULE hSrcModule, LPCSTR pSrcResource, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, CTexture** ppTexture);
	
	//
	// Other
	//
protected:

	virtual HRESULT 	GetDeviceCapsUnsup(CAPS* pCaps);
	virtual HRESULT 	GetVerticalBlankStatusUnsup(LPBOOL lpbIsInVB);	
	virtual HRESULT 	CreatePixelShaderUnsup(DWORD* pFunction,DWORD* pHandle);
	virtual HRESULT 	SetPixelShaderUnsup(DWORD Handle);
	virtual HRESULT 	GetPixelShaderUnsup(DWORD* pHandle);
	virtual HRESULT 	DeletePixelShaderUnsup(DWORD Handle);
	virtual HRESULT 	CreatePixelShaderIntUnsup(DWORD* pFunction,CPShader** pHandle);
	virtual HRESULT 	SetPixelShaderIntUnsup(CPShader* Handle);
	virtual HRESULT 	GetPixelShaderIntUnsup(CPShader** pHandle);
	virtual HRESULT 	DeletePixelShaderIntUnsup(CPShader* Handle);
	virtual HRESULT 	SetPixelShaderConstantUnsup(UINT StartRegister,const float* pConstantData,UINT Vector4fCount);
	virtual HRESULT 	GetPixelShaderConstantUnsup(UINT StartRegister,float* pConstantData,UINT Vector4fCount);
	virtual HRESULT 	SetPixelShaderConstantIUnsup(UINT StartRegister,const int* pConstantData,UINT Vector4iCount);
	virtual HRESULT 	GetPixelShaderConstantIUnsup(UINT StartRegister,int* pConstantData,UINT Vector4iCount);
	virtual HRESULT 	SetPixelShaderConstantBUnsup(UINT StartRegister,const BOOL* pConstantData,UINT BoolCount);
	virtual HRESULT 	GetPixelShaderConstantBUnsup(UINT StartRegister,BOOL* pConstantData,UINT BoolCount);
	virtual HRESULT 	GetPixelShaderFunctionUnsup(DWORD Handle,void* pData,DWORD* pSizeOfData);
	virtual HRESULT 	CreateIndexBufferUnsup(UINT Length, DWORD Usage, FMT Format, DWORD Pool, CIndexBuffer** ppIB);
	virtual HRESULT 	CreateVertexShaderUnsup(DWORD* pDeclaration,DWORD* pFunction,CVertexShader **pHandle,DWORD Usage);
	virtual HRESULT 	CreateVertexShaderIntUnsup(DWORD* pFunction, CVertexShader **pVShader);
	virtual HRESULT 	CreateVertexDeclarationUnsup(VERTEXELEMENT *pVE, CVertexDeclaration **pDecl);
	virtual HRESULT 	SetVertexShaderUnsup(DWORD Handle);
	virtual HRESULT 	SetVertexShaderIntUnsup(CVertexShader *pShader);
	virtual HRESULT 	GetVertexShaderUnsup(DWORD* pHandle);
	virtual HRESULT 	GetVertexShaderIntUnsup(CVertexShader **pShader);
	virtual HRESULT 	SetFVFUnsup(DWORD dwFVF);
	virtual HRESULT 	DeleteVertexShaderUnsup(CVertexShader* pShader);
	virtual HRESULT 	SetVertexShaderConstantUnsup(UINT Register, CONST float* pConstantData,UINT ConstantCount);
	virtual HRESULT 	GetVertexShaderConstantUnsup(UINT Register, float* pConstantData,UINT ConstantCount);
	virtual HRESULT 	GetVertexShaderDeclarationUnsup(DWORD Handle,void* pData,DWORD* pSizeOfData);
	virtual HRESULT 	GetVertexShaderFunctionUnsup(DWORD Handle,void* pData,DWORD* pSizeOfData);
	virtual HRESULT 	SetStreamSourceUnsup(UINT streamNumber, CVertexBuffer *pStreamData, UINT stride, UINT offset);
	virtual HRESULT 	GetStreamSourceUnsup(UINT streamNumber, CVertexBuffer **ppStreamData, UINT *pStride);
	virtual HRESULT 	SetIndicesUnsup(CIndexBuffer *pIndexData, UINT baseVertexIndex);
	virtual HRESULT 	GetIndicesUnsup(CIndexBuffer **ppIndexData,  UINT *pBaseVertexIndex);
	virtual HRESULT 	DrawRectPatchUnsup(UINT uHandle, float *pSegments, RECTPATCH_INFO *pInfo);
	virtual HRESULT 	DrawTriPatchUnsup(UINT uHandle, float *pSegments, TRIPATCH_INFO *pInfo);
	virtual HRESULT 	DeletePatchUnsup(UINT uHandle);
	virtual HRESULT 	SetNPatchModeUnsup(float nSegments);
	virtual float		GetNPatchModeUnsup();
	virtual HRESULT 	TestCooperativeLevelUnsup(void);
	virtual UINT		GetAvailableTextureMemUnsup(void);
	virtual HRESULT 	ResourceManagerDiscardBytesUnsup(DWORD Bytes);
	virtual HRESULT 	GetBackBufferUnsup(UINT BackBuffer,DWORD Type,CSurface** ppBackBuffer);
	virtual HRESULT 	GetBackBuffer2Unsup(UINT iSwapChain,UINT BackBuffer,DWORD Type,CSurface** ppBackBuffer);
	virtual HRESULT 	GetRasterStatusUnsup(RASTER_STATUS* pRasterStatus);
	virtual void		SetGammaRampUnsup(DWORD Flags,GAMMARAMP* pRamp);
	virtual void		GetGammaRampUnsup(GAMMARAMP* pRamp);
	virtual HRESULT 	GetRasterStatus2Unsup(UINT uSwapchain, RASTER_STATUS* pRasterStatus);
	virtual void		SetGammaRamp2Unsup(UINT uSwapchain, DWORD Flags,GAMMARAMP* pRamp);
	virtual void		GetGammaRamp2Unsup(UINT uSwapchain, GAMMARAMP* pRamp);
	virtual HRESULT 	CreateTextureUnsup(UINT Width,UINT Height,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CTexture** ppTexture);
	virtual HRESULT 	CreateVolumeTextureUnsup(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CVolTexture** ppVolumeTexture);
	virtual HRESULT 	CreateCubeTextureUnsup(UINT EdgeLength,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CCubeTexture** ppCubeTexture);
	virtual HRESULT 	SetPaletteEntriesUnsup(UINT PaletteNumber,PALETTEENTRY* pEntries);
	virtual HRESULT 	GetPaletteEntriesUnsup(UINT PaletteNumber,PALETTEENTRY* pEntries);
	virtual HRESULT 	SetCurrentTexturePaletteUnsup(UINT PaletteNumber);
	virtual HRESULT 	GetCurrentTexturePaletteUnsup(UINT *PaletteNumber);
	virtual HRESULT 	GetFrontBufferUnsup(CSurface* pDestSurface);
	virtual HRESULT 	GetFrontBufferDataUnsup(UINT iSwapChain, CSurface* pDestSurface);
	virtual HRESULT 	GetRenderTargetDataUnsup(CSurface* pSrcRenderTarget, CSurface* pDestSurface);
	virtual HRESULT 	UpdateTextureUnsup(CBaseTexture* pSourceTexture,CBaseTexture* pDestinationTexture);
	virtual HRESULT 	CopyRectsUnsup(CSurface* pSourceSurface,RECT* pSourceRectsArray,UINT cRects,CSurface* pDestinationSurface,POINT* pDestPointsArray);
	virtual HRESULT 	UpdateSurfaceUnsup(CSurface* pSourceSurface,RECT* pSourceRects,CSurface* pDestinationSurface,POINT* pDestPoints);
	virtual HRESULT 	GetDisplayModeUnsup(DISPLAYMODE* pMode);
	virtual HRESULT 	GetDisplayMode2Unsup(UINT uSwapChain, DISPLAYMODE* pMode);
	virtual HRESULT 	GetCreationParametersUnsup(DEVICE_CREATION_PARAMETERS *pParameters);
	virtual HRESULT 	ResetUnsup(PRESENT_PARAMETERS* pPresentationParameters);
	virtual HRESULT 	SetCursorPropertiesUnsup(UINT XHotSpot,UINT YHotSpot,CSurface* pCursorBitmap);
	virtual void		SetCursorPositionUnsup(UINT XScreenSpace,UINT YScreenSpace,DWORD Flags);
	virtual BOOL		ShowCursorUnsup(BOOL bShow);
	virtual HRESULT 	ProcessVerticesUnsup(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,CVertexBuffer* pDestBuffer, CVertexDeclaration *pDecl, DWORD Flags);
	virtual HRESULT 	DrawPrimitiveUnsup(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
	virtual HRESULT 	DrawIndexedPrimitiveUnsup(D3DPRIMITIVETYPE,UINT minIndex,UINT NumVertices,UINT startIndex,UINT primCount,INT BaseVertexIndex);
	virtual HRESULT 	DrawPrimitiveUPUnsup(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	virtual HRESULT 	DrawIndexedPrimitiveUPUnsup(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertexIndices,UINT PrimitiveCount,void* pIndexData,FORMAT IndexDataFormat,void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	virtual HRESULT 	SetLightUnsup(DWORD dwLightIndex,LPLIGHT lpLight);
	virtual HRESULT 	GetLightUnsup(DWORD dwLightIndex,LPLIGHT lpLight);
	virtual HRESULT 	LightEnableUnsup(DWORD dwLightIndex,BOOL bEnable);
	virtual HRESULT 	GetLightEnableUnsup(DWORD dwLightIndex,BOOL *bEnable);
	virtual HRESULT 	SetClipPlaneUnsup(DWORD dwIndex,D3DVALUE* pPlaneEquation);
	virtual HRESULT 	GetClipPlaneUnsup(DWORD dwIndex,D3DVALUE* pPlaneEquation);
	virtual HRESULT 	BeginStateBlockUnsup();
	virtual HRESULT 	EndStateBlockUnsup(UINT_PTR *dwHandle);
	virtual HRESULT 	ApplyStateBlockUnsup(UINT_PTR dwHandle);
	virtual HRESULT 	CaptureStateBlockUnsup(UINT_PTR dwHandle);
	virtual HRESULT 	DeleteStateBlockUnsup(UINT_PTR dwHandle);
	virtual HRESULT 	CreateStateBlockUnsup(D3DSTATEBLOCKTYPE sbt,  UINT_PTR *dwHandle);
	virtual HRESULT 	SetClipStatusUnsup(LPCLIPSTATUS pClipStatus);
	virtual HRESULT 	GetClipStatusUnsup(LPCLIPSTATUS pClipStatus);
	virtual HRESULT 	GetInfoUnsup(DWORD dwDevInfoID,LPVOID pDevInfoStruct,DWORD dwSize);
	virtual HRESULT 	SetLightStateUnsup(D3DLIGHTSTATETYPE dwState,DWORD dwValue);
	virtual HRESULT 	GetLightStateUnsup(D3DLIGHTSTATETYPE dwState,DWORD *pdwValue);
	virtual HRESULT 	CreateImageSurfaceUnsup(UINT Width,UINT Height,FORMAT Format,CSurface** ppSurface);
	virtual HRESULT 	StretchRectUnsup(CSurface* pSourceSurface,CONST RECT* pSourceRect,CSurface* pDestSurface,CONST RECT* pDestRect, DWORD dwFilter);
	virtual HRESULT 	ColorFillUnsup(CSurface* pSurface,CONST RECT* pRect,DWORD color);
	virtual HRESULT 	CreateOffscreenPlainSurfaceUnsup(UINT Width,UINT Height,FORMAT Format, DWORD dwPool, CSurface** ppSurface, HANDLE *pSharedHandle);
	virtual HRESULT 	SetScissorRectUnsup(CONST RECT* pRect);
	virtual HRESULT 	GetScissorRectUnsup(RECT* pRect);
	virtual HRESULT 	CreateVertexDeclarationUnsup(CONST VERTEXELEMENT* pVertexElements,CVertexDeclaration** ppDecl);
	virtual HRESULT 	SetVertexDeclarationUnsup(CVertexDeclaration* pDecl);
	virtual HRESULT 	GetVertexDeclarationUnsup(CVertexDeclaration** ppDecl);
	virtual HRESULT 	GetFVFUnsup(DWORD* pFVF);
	virtual HRESULT 	SetVertexShaderConstantFUnsup(UINT Register,CONST float* pConstantData,UINT ConstantCount);
	virtual HRESULT 	GetVertexShaderConstantFUnsup(UINT Register,float* pConstantData,UINT ConstantCount);
	virtual HRESULT 	SetVertexShaderConstantIUnsup(UINT Register,CONST int* pConstantData,UINT ConstantCount);
	virtual HRESULT 	GetVertexShaderConstantIUnsup(UINT Register,int* pConstantData,UINT ConstantCount);
	virtual HRESULT 	SetVertexShaderConstantBUnsup(UINT StartRegister,CONST BOOL* pConstantData, UINT BoolCount);
	virtual HRESULT 	GetVertexShaderConstantBUnsup(UINT StartRegister,BOOL* pConstantData, UINT BoolCount);
	virtual HRESULT 	SetSamplerStateUnsup(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD Value);
	virtual HRESULT 	GetSamplerStateUnsup(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD *Value);
	virtual HRESULT 	SetDialogBoxModeUnsup(BOOL bEnableDialogs);
	virtual HRESULT 	SetSoftwareVertexProcessingUnsup(BOOL bSoftware);
	virtual BOOL		GetSoftwareVertexProcessingUnsup();
	virtual HRESULT 	GetSwapChainUnsup(UINT iSwapChain,CSwapChain** pSwapChain);
	virtual UINT		GetNumberOfSwapChainsUnsup();
	virtual HRESULT 	CreateRenderTarget2Unsup(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,CSurface **ppSurface, HANDLE *pSharedHandle);
	virtual HRESULT 	CreateDepthStencilSurface2Unsup(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality, BOOL Discard, CSurface **ppSurface, HANDLE *pSharedHandle);
	virtual HRESULT 	CreateTexture2Unsup(UINT Width,UINT Height,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CTexture** ppTexture, HANDLE *pSharedHandle);
	virtual HRESULT 	CreateVolumeTexture2Unsup(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CVolTexture **ppVolumeTexture, HANDLE *pSharedHandle);
	virtual HRESULT 	CreateCubeTexture2Unsup(UINT EdgeLength,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CCubeTexture **ppCubeTexture, HANDLE *pSharedHandle);
	virtual HRESULT 	CreateVertexBuffer2Unsup(UINT Length, DWORD Usage, DWORD FVF, DWORD Pool, CVertexBuffer** ppVB, HANDLE *pSharedHandle);
	virtual HRESULT 	CreateIndexBuffer2Unsup(UINT Length, DWORD Usage, FMT Format, DWORD Pool, CIndexBuffer** ppIB, HANDLE *pSharedHandle);
	virtual HRESULT 	CreateQueryUnsup(QUERYTYPE Type, CQuery** pQuery);
	virtual HRESULT 	SetRenderTargetUnsup(DWORD RenderTargetIndex, CSurface *pNew);
	virtual HRESULT 	GetRenderTargetUnsup(DWORD RenderTargetIndex, CSurface **pNew);
	virtual HRESULT 	SetDepthStencilSurfaceUnsup(CSurface *pNewDepth);
	virtual HRESULT 	SetStreamSourceFreqUnsup(UINT nStreamNumber, UINT nDivider);
	virtual HRESULT 	GetStreamSourceFreqUnsup(UINT nStreamNumber, UINT* pDividerOut);
	virtual HRESULT 	CreateEffectFromFileUnsup(LPCTSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, CEffect** ppEffect, LPD3DXBUFFER* ppCompilationErrors);
	virtual HRESULT 	CreateEffectFromResourceUnsup(HMODULE hSrcModule, LPCTSTR pSrcresource, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, CEffect** ppEffect, LPD3DXBUFFER* ppCompilationErrors);
	virtual HRESULT 	SetConvolutionMonoKernelUnsup(UINT,UINT,float *, float *);
	virtual HRESULT 	ComposeRectsUnsup(LPSURFACE, LPSURFACE, LPVERTEXBUFFER, UINT, LPVERTEXBUFFER, UINT, int,int);
	virtual HRESULT 	PresentExUnsup( CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*, DWORD );
	virtual HRESULT		GetGPUThreadPriorityUnsup(INT *);
	virtual HRESULT		SetGPUThreadPriorityUnsup(INT);
	virtual HRESULT 	WaitForVBlankUnsup( UINT SwapChainIndex ); 
	virtual HRESULT 	CheckResourceResidencyUnsup(CResource**, UINT);
	virtual HRESULT 	SetMaximumFrameLatencyUnsup(UINT);
	virtual HRESULT 	GetMaximumFrameLatencyUnsup(UINT*);
	virtual HRESULT 	CheckDeviceStateUnsup(HWND hWindow);
	virtual HRESULT 	CreateRenderTargetExUnsup(UINT, UINT, FORMAT, MULTISAMPLE_TYPE, DWORD, BOOL, CSurface**, HANDLE*, DWORD);
	virtual HRESULT 	CreateOffscreenPlainSurfaceExUnsup(UINT, UINT, FORMAT, DWORD, CSurface**, HANDLE*, DWORD);
	virtual HRESULT 	CreateDepthStencilSurfaceExUnsup(UINT, UINT, FORMAT, MULTISAMPLE_TYPE, DWORD, BOOL, CSurface**, HANDLE*, DWORD);
	virtual HRESULT		ResetExUnsup( PRESENT_PARAMETERS*, D3DDISPLAYMODEEX* );
	//virtual HRESULT		GetDisplayModeExUnsup(UINT,D3DDISPLAYMODEEX*);


public:
	
	//
	// Abstracted DX calls which call the appropriate low level wrappers
	//

	virtual HRESULT 	GetDeviceCaps(CAPS* pCaps)
							{ return (this->*pGetDeviceCaps)(pCaps); }
	HRESULT 			(CDevice::*pGetDeviceCaps)(CAPS* pCaps);

	virtual HRESULT 	SetRenderTarget(CSurface *pNew, CSurface *pNewDepth = NULL)
							{ return (this->*pSetRenderTarget)(pNew, pNewDepth); }
	HRESULT 			(CDevice::*pSetRenderTarget)(CSurface *pNew, CSurface *pNewDepth);

	virtual HRESULT 	GetRenderTarget(CSurface **ppRenderTarget)
							{ return (this->*pGetRenderTarget)(ppRenderTarget);}
	HRESULT 			(CDevice::*pGetRenderTarget)(CSurface **ppRenderTarget);

	virtual HRESULT 	SetRenderTarget(DWORD RenderTargetIndex, CSurface *pNew)
							{ return (this->*pSetRenderTarget2)(RenderTargetIndex, pNew); }
	HRESULT 			(CDevice::*pSetRenderTarget2)(DWORD RenderTargetIndex, CSurface *pNew);

	virtual HRESULT 	GetRenderTarget(DWORD RenderTargetIndex, CSurface **ppRenderTarget)
							{ return (this->*pGetRenderTarget2)(RenderTargetIndex, ppRenderTarget);}
	HRESULT 			(CDevice::*pGetRenderTarget2)(DWORD RenderTargetIndex, CSurface **ppRenderTarget);

	virtual HRESULT 	GetDepthStencilSurface(CSurface **ppZStencilSurface)
							{ return (this->*pGetDepthStencilSurface)(ppZStencilSurface);}
	HRESULT 			(CDevice::*pGetDepthStencilSurface)(CSurface **ppZStencilSurface);

	virtual HRESULT 	SetDepthStencilSurface(CSurface *pNewDepth)
							{ return (this->*pSetDepthStencilSurface)(pNewDepth); }
	HRESULT 			(CDevice::*pSetDepthStencilSurface)(CSurface *pNewDepth);
	
	virtual HRESULT 	CreateRenderTarget(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,BOOL Lockable,CSurface** ppSurface)
							{ return (this->*pCreateRenderTarget)(Width,Height,Format,MultiSample,Lockable,ppSurface);}
	HRESULT 			(CDevice::*pCreateRenderTarget)(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,BOOL Lockable,CSurface** ppSurface);
									 
	virtual HRESULT 	CreateRenderTarget(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,CSurface** ppSurface,HANDLE *pSharedHandle = NULL)
							{ return (this->*pCreateRenderTarget2)(Width,Height,Format,MultiSample,MultisampleQuality,Lockable,ppSurface,pSharedHandle);}
	HRESULT 			(CDevice::*pCreateRenderTarget2)(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,CSurface** ppSurface,HANDLE *pSharedHandle);

	virtual HRESULT 	CreateDepthStencilSurface(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample, CSurface **ppSurface)
							{ return (this->*pCreateDepthStencilSurface)(Width,Height,Format,MultiSample,ppSurface);}
	HRESULT 			(CDevice::*pCreateDepthStencilSurface)(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample, CSurface **ppSurface);

	virtual HRESULT 	CreateDepthStencilSurface(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,CSurface **ppSurface,HANDLE *pSharedHandle = NULL)
							{ return (this->*pCreateDepthStencilSurface2)(Width,Height,Format,MultiSample,MultisampleQuality,Discard, ppSurface,pSharedHandle);}
	HRESULT 			(CDevice::*pCreateDepthStencilSurface2)(UINT Width,UINT Height,FORMAT Format,MULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,CSurface **ppSurface,HANDLE *pSharedHandle);

	virtual HRESULT 	CreateImageSurface(UINT Width,UINT Height,FORMAT Format,CSurface** ppSurface)
							{ return (this->*pCreateImageSurface)(Width,Height,Format,ppSurface);}
	HRESULT 			(CDevice::*pCreateImageSurface)(UINT Width,UINT Height,FORMAT Format,CSurface** ppSurface);
	
	virtual HRESULT 	CreateAdditionalSwapChain(PRESENT_PARAMETERS* pParams, CSwapChain** ppSwapChain)
							{ return (this->*pCreateAdditionalSwapChain)(pParams, ppSwapChain); }
	HRESULT 			(CDevice::*pCreateAdditionalSwapChain)(PRESENT_PARAMETERS* pParams, CSwapChain** ppSwapChain);

	virtual HRESULT 	GetFrontBuffer(CSurface* pDestSurface)
							{ return (this->*pGetFrontBuffer)(pDestSurface); }
	HRESULT 			(CDevice::*pGetFrontBuffer)(CSurface* pDestSurface);

	virtual HRESULT 	GetRenderTargetData(CSurface* pSrcRenderTarget, CSurface* pDestSurface)
							{ return (this->*pGetRenderTargetData)(pSrcRenderTarget, pDestSurface); }
	HRESULT 			(CDevice::*pGetRenderTargetData)(CSurface* pSrcRenderTarget, CSurface* pDestSurface);
	
	virtual HRESULT 	GetFrontBufferData(UINT iSwapChain, CSurface* pDestSurface)
							{ return (this->*pGetFrontBufferData)(iSwapChain, pDestSurface); }
	HRESULT 			(CDevice::*pGetFrontBufferData)(UINT iSwapChain, CSurface* pDestSurface);

	virtual HRESULT 	CopyRects(CSurface* pSourceSurface,RECT* pSourceRectsArray,UINT cRects,CSurface* pDestinationSurface,POINT* pDestPointsArray)
							{ return (this->*pCopyRects)(pSourceSurface,pSourceRectsArray,cRects,pDestinationSurface,pDestPointsArray); }
	HRESULT 			(CDevice::*pCopyRects)(CSurface* pSourceSurface,RECT* pSourceRectsArray,UINT cRects,CSurface* pDestinationSurface,POINT* pDestPointsArray);

	virtual HRESULT 	UpdateSurface(CSurface* pSourceSurface,RECT* pSourceRects,CSurface* pDestinationSurface,POINT* pDestPoint)
							{ return (this->*pUpdateSurface)(pSourceSurface,pSourceRects,pDestinationSurface,pDestPoint); }
	HRESULT 			(CDevice::*pUpdateSurface)(CSurface* pSourceSurface,RECT* pSourceRects,CSurface* pDestinationSurface,POINT* pDestPoint);

	virtual HRESULT 	UpdateTexture(CBaseTexture* pSourceTexture,CBaseTexture* pDestinationTexture)
							{ return (this->*pUpdateTexture)(pSourceTexture,pDestinationTexture); }
	HRESULT 			(CDevice::*pUpdateTexture)(CBaseTexture* pSourceTexture,CBaseTexture* pDestinationTexture);

	virtual HRESULT 	TestCooperativeLevel() 
							{ return (this->*pTestCooperativeLevel)(); }
	HRESULT 			(CDevice::*pTestCooperativeLevel)();

	virtual UINT		GetAvailableTextureMem() 
							{ return (this->*pGetAvailableTextureMem)(); }
	UINT				(CDevice::*pGetAvailableTextureMem)();
	
	virtual HRESULT 	GetDisplayMode(DISPLAYMODE* pMode)
							{ return (this->*pGetDisplayMode)(pMode); }
	HRESULT 			(CDevice::*pGetDisplayMode)(DISPLAYMODE* pMode);

	virtual HRESULT 	GetDisplayMode(UINT uSwapChain, DISPLAYMODE* pMode)
							{ return (this->*pGetDisplayMode2)(uSwapChain, pMode); }
	HRESULT 			(CDevice::*pGetDisplayMode2)(UINT uSwapchain, DISPLAYMODE* pMode);

	virtual HRESULT 	GetCreationParameters(DEVICE_CREATION_PARAMETERS *pParameters)
							{ return (this->*pGetCreationParameters)(pParameters); }
	HRESULT 			(CDevice::*pGetCreationParameters)(DEVICE_CREATION_PARAMETERS *pParameters);

	virtual HRESULT 	SetCursorProperties(UINT XHotSpot,UINT YHotSpot,CSurface* pCursorBitmap)
							{ return (this->*pSetCursorProperties)(XHotSpot,YHotSpot,pCursorBitmap); }
	HRESULT 			(CDevice::*pSetCursorProperties)(UINT XHotSpot,UINT YHotSpot,CSurface* pCursorBitmap);

	virtual void		SetCursorPosition(UINT XScreenSpace,UINT YScreenSpace,DWORD Flags)
							{ (this->*pSetCursorPosition)(XScreenSpace,YScreenSpace,Flags); }
	void				(CDevice::*pSetCursorPosition)(UINT XScreenSpace,UINT YScreenSpace,DWORD Flags);

	virtual BOOL		ShowCursor(BOOL bShow)
							{ return (this->*pShowCursor)(bShow); }
	BOOL				(CDevice::*pShowCursor)(BOOL bShow);

	virtual HRESULT 	Reset(PRESENT_PARAMETERS* pPresentationParameters)
							{ return (this->*pReset)(pPresentationParameters); }
	HRESULT 			(CDevice::*pReset)(PRESENT_PARAMETERS* pPresentationParameters);

	virtual HRESULT 	GetBackBuffer(UINT BackBuffer,DWORD Type,CSurface** ppBackBuffer) 
							{ return (this->*pGetBackBuffer)(BackBuffer,Type,ppBackBuffer); }
	HRESULT 			(CDevice::*pGetBackBuffer)(UINT BackBuffer,DWORD Type,CSurface** ppBackBuffer);

	virtual HRESULT 	GetBackBuffer(UINT iSwapChain,UINT BackBuffer,DWORD Type,CSurface** ppBackBuffer) 
							{ return (this->*pGetBackBuffer2)(iSwapChain,BackBuffer,Type,ppBackBuffer); }
	HRESULT 			(CDevice::*pGetBackBuffer2)(UINT iSwapChain,UINT BackBuffer,DWORD Type,CSurface** ppBackBuffer);

	virtual HRESULT 	EvictManagedResources(void) 
							{ return (this->*pEvictManagedResources)(); }
	HRESULT 			(CDevice::*pEvictManagedResources)(void);
	
	virtual HRESULT 	ResourceManagerDiscardBytes(DWORD Bytes) 
							{ return (this->*pResourceManagerDiscardBytes)(Bytes); }
	HRESULT 			(CDevice::*pResourceManagerDiscardBytes)(DWORD Bytes);

	virtual HRESULT 	BeginScene(void)
							{ return (this->*pBeginScene)(); }
	HRESULT 			(CDevice::*pBeginScene)(void);
	
	virtual HRESULT 	EndScene(void)
							{ return (this->*pEndScene)(); }
	HRESULT 			(CDevice::*pEndScene)(void);
	
	virtual HRESULT 	Clear(DWORD Count,D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil)
							{ return (this->*pClear)(Count,pRects,Flags,Color,Z,Stencil); }
	HRESULT 			(CDevice::*pClear)(DWORD Count,D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);

	virtual HRESULT 	SetTransform(D3DTRANSFORMSTATETYPE Type, LPD3DMATRIX Matrix)
							{ return (this->*pSetTransform)(Type, Matrix); }
	HRESULT 			(CDevice::*pSetTransform)(D3DTRANSFORMSTATETYPE Type, LPD3DMATRIX Matrix);
 
	virtual HRESULT 	GetTransform(D3DTRANSFORMSTATETYPE Type, LPD3DMATRIX Matrix)
							{ return (this->*pGetTransform)(Type, Matrix); }
	HRESULT 			(CDevice::*pGetTransform)(D3DTRANSFORMSTATETYPE Type, LPD3DMATRIX Matrix);
 
	virtual HRESULT 	MultiplyTransform(D3DTRANSFORMSTATETYPE Type, LPD3DMATRIX Matrix)
							{ return (this->*pMultiplyTransform)(Type, Matrix); }
	HRESULT 			(CDevice::*pMultiplyTransform)(D3DTRANSFORMSTATETYPE Type, LPD3DMATRIX Matrix);

	virtual HRESULT 	SetViewport(LPVIEWPORT pViewport)
							{ return (this->*pSetViewport)(pViewport); }
	HRESULT 			(CDevice::*pSetViewport)(LPVIEWPORT pViewport);

	virtual HRESULT 	GetViewport(LPVIEWPORT pViewport)
							{ return (this->*pGetViewport)(pViewport); }
	HRESULT 			(CDevice::*pGetViewport)(LPVIEWPORT pViewport);

	virtual HRESULT 	SetMaterial(MATERIAL *pMaterial)
							{ return (this->*pSetMaterial)(pMaterial); }
	HRESULT 			(CDevice::*pSetMaterial)(MATERIAL *pMaterial);

	virtual HRESULT 	GetMaterial(MATERIAL *pMaterial)
							{ return (this->*pGetMaterial)(pMaterial); }
	HRESULT 			(CDevice::*pGetMaterial)(MATERIAL *pMaterial);

	virtual HRESULT 	SetLight(DWORD dwIndex, LIGHT *pLight)
							{ return (this->*pSetLight)(dwIndex, pLight); }
	HRESULT 			(CDevice::*pSetLight)(DWORD dwIndex, LIGHT *pLight);
	
	virtual HRESULT 	GetLight(DWORD dwIndex, LIGHT *pLight)
							{ return (this->*pGetLight)(dwIndex, pLight); }
	HRESULT 			(CDevice::*pGetLight)(DWORD dwIndex, LIGHT *pLight);

	virtual HRESULT 	LightEnable(DWORD dwLightNum, BOOL bEnable)
							{ return (this->*pLightEnable)(dwLightNum, bEnable); }
	HRESULT 			(CDevice::*pLightEnable)(DWORD dwLightNum, BOOL bEnable);

	virtual HRESULT 	GetLightEnable(DWORD dwLightNum, BOOL* pEnable)
							{ return (this->*pGetLightEnable)(dwLightNum, pEnable); }
	HRESULT 			(CDevice::*pGetLightEnable)(DWORD dwLightNum, BOOL* pEnable);

	virtual HRESULT 	SetClipPlane(DWORD dwIndex, float* pPlane)
							{ return (this->*pSetClipPlane)(dwIndex, pPlane); }
	HRESULT 			(CDevice::*pSetClipPlane)(DWORD dwIndex, float* pPlane);

	virtual HRESULT 	GetClipPlane(DWORD dwIndex, float* pPlane)
							{ return (this->*pGetClipPlane)(dwIndex, pPlane); }
	HRESULT 			(CDevice::*pGetClipPlane)(DWORD dwIndex, float* pPlane);

	virtual HRESULT 	SetLightState(D3DLIGHTSTATETYPE dwState, DWORD dwValue)
							{ return (this->*pSetLightState)(dwState, dwValue); }
	HRESULT 			(CDevice::*pSetLightState)(D3DLIGHTSTATETYPE dwState, DWORD dwValue);

	virtual HRESULT 	GetLightState(D3DLIGHTSTATETYPE dwState, DWORD *pdwValue)
							{ return (this->*pGetLightState)(dwState, pdwValue); }
	HRESULT 			(CDevice::*pGetLightState)(D3DLIGHTSTATETYPE dwState, DWORD *pdwValue);

	virtual HRESULT 	SetRenderState(D3DRENDERSTATETYPE dwState, DWORD dwValue)
							{ return (this->*pSetRenderState)(dwState, dwValue); }
	HRESULT 			(CDevice::*pSetRenderState)(D3DRENDERSTATETYPE dwState, DWORD dwValue);

	virtual HRESULT 	GetRenderState(D3DRENDERSTATETYPE dwState, DWORD *pdwValue)
							{ return (this->*pGetRenderState)(dwState, pdwValue); }
	HRESULT 			(CDevice::*pGetRenderState)(D3DRENDERSTATETYPE dwState, DWORD *pdwValue);
	
	virtual HRESULT 	BeginStateBlock(void)
							{ return (this->*pBeginStateBlock)(); }
	HRESULT 			(CDevice::*pBeginStateBlock)(void);
	
	virtual HRESULT 	EndStateBlock(UINT_PTR *pHandle)
							{ return (this->*pEndStateBlock)(pHandle); }
	HRESULT 			(CDevice::*pEndStateBlock)(UINT_PTR *pHandle);

	virtual HRESULT 	EndStateBlock(CStateblock **pStateblock)
							{ return (this->*pEndStateBlockInterface)(pStateblock); }
	HRESULT 			(CDevice::*pEndStateBlockInterface)(CStateblock **pHandle);

	virtual HRESULT 	ApplyStateBlock(UINT_PTR uHandle)
							{ return (this->*pApplyStateBlock)(uHandle); }
	HRESULT 			(CDevice::*pApplyStateBlock)(UINT_PTR uHandle);
	
	virtual HRESULT 	CaptureStateBlock(UINT_PTR uHandle)
							{ return (this->*pCaptureStateBlock)(uHandle); }
	HRESULT 			(CDevice::*pCaptureStateBlock)(UINT_PTR uHandle);
	
	virtual HRESULT 	DeleteStateBlock(UINT_PTR uHandle)
							{ return (this->*pDeleteStateBlock)(uHandle); }
	HRESULT 			(CDevice::*pDeleteStateBlock)(UINT_PTR uHandle);
	
	virtual HRESULT 	CreateStateBlock(D3DSTATEBLOCKTYPE sbtType, UINT_PTR *pHandle)
							{ return (this->*pCreateStateBlock)(sbtType, pHandle); }
	HRESULT 			(CDevice::*pCreateStateBlock)(D3DSTATEBLOCKTYPE sbtType, UINT_PTR *pHandle);

	virtual HRESULT 	CreateStateBlock(D3DSTATEBLOCKTYPE sbtType, CStateblock **pStateblock)
							{ return (this->*pCreateStateBlockInterface)(sbtType, pStateblock); }
	HRESULT 			(CDevice::*pCreateStateBlockInterface)(D3DSTATEBLOCKTYPE sbtType, CStateblock **pStateblock);	 

	virtual HRESULT 	SetClipStatus(LPCLIPSTATUS pClipStatus)
							{ return (this->*pSetClipStatus)(pClipStatus); }
	HRESULT 			(CDevice::*pSetClipStatus)(LPCLIPSTATUS pClipStatus);

	virtual HRESULT 	GetClipStatus(LPCLIPSTATUS pClipStatus)
							{ return (this->*pGetClipStatus)(pClipStatus); }
	HRESULT 			(CDevice::*pGetClipStatus)(LPCLIPSTATUS pClipStatus);
	
	virtual HRESULT 	SetTexture(int nIndex, CBaseTexture *pTextures)
						{ return (this->*pSetTexture)(nIndex, pTextures); }
	HRESULT 			(CDevice::*pSetTexture)(int nIndex,CBaseTexture *pTextures);

	virtual HRESULT 	GetTexture(int nIndex, CBaseTexture **ppTextures)
						{ return (this->*pGetTexture)(nIndex, ppTextures); }
	HRESULT 			(CDevice::*pGetTexture)(int nIndex,CBaseTexture **ppTextures);

	virtual HRESULT 	GetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pdwValue)
							{ return (this->*pGetTextureStageState)(dwStage, Type, pdwValue); }
	HRESULT 			(CDevice::*pGetTextureStageState)(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pdwValue);

	virtual HRESULT 	SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD dwValue)
							{ return (this->*pSetTextureStageState)(dwStage, Type, dwValue); }
	HRESULT 			(CDevice::*pSetTextureStageState)(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD dwValue);
	
	virtual HRESULT 	ValidateDevice(DWORD *pdwPasses)
							{ return (this->*pValidateDevice)(pdwPasses); };
	HRESULT 			(CDevice::*pValidateDevice)(DWORD *pdwPasses);

	virtual HRESULT 	GetInfo(DWORD dwDevInfoID,LPVOID pDevInfoStruct,DWORD dwSize)
							{ return (this->*pGetInfo)(dwDevInfoID,pDevInfoStruct,dwSize); };
	HRESULT 			(CDevice::*pGetInfo)(DWORD dwDevInfoID,LPVOID pDevInfoStruct,DWORD dwSize);

	virtual HRESULT 	GetVerticalBlankStatus(LPBOOL lpbIsInVB)
							{ return (this->*pGetVerticalBlankStatus)(lpbIsInVB); };
	HRESULT 			(CDevice::*pGetVerticalBlankStatus)(LPBOOL lpbIsInVB);

	virtual HRESULT 	CreateVertexDeclaration(VERTEXELEMENT *pVE, CVertexDeclaration **pDecl)
							{ return (this->*pCreateVertexDeclaration)(pVE, pDecl); }
	HRESULT 			(CDevice::*pCreateVertexDeclaration)(VERTEXELEMENT *pVE, CVertexDeclaration **pDecl);

	virtual HRESULT 	CreateVertexShader(LPDWORD pdwDeclaration, LPDWORD pdwFunction, CVertexShader** pdwHandle, DWORD dwUsage)
							{ return (this->*pCreateVertexShader)(pdwDeclaration, pdwFunction, pdwHandle, dwUsage); }
	HRESULT 			(CDevice::*pCreateVertexShader)(LPDWORD pdwDeclaration, LPDWORD pdwFunction, CVertexShader** pdwHandle, DWORD dwUsage);
	
	virtual HRESULT 	CreateVertexShader(LPDWORD pdwFunction, CVertexShader **pVShader)
							{ return (this->*pCreateVertexShaderInt)(pdwFunction, pVShader); }
	HRESULT 			(CDevice::*pCreateVertexShaderInt)(LPDWORD pdwFunction, CVertexShader **pVShader);

	virtual HRESULT 	SetVertexShader(DWORD dwHandle)
							{ return (this->*pSetVertexShader)(dwHandle); }
	HRESULT 			(CDevice::*pSetVertexShader)(DWORD dwHandle);

	virtual HRESULT 	SetVertexShader(CVertexShader* pShader)
							{ return (this->*pSetVertexShaderInt)(pShader); }
	HRESULT 			(CDevice::*pSetVertexShaderInt)(CVertexShader* pShader);

	virtual HRESULT 	SetFVF(DWORD dwFVF)
							{ return (this->*pSetFVF)(dwFVF); }
	HRESULT 			(CDevice::*pSetFVF)(DWORD dwFVF);

	virtual HRESULT 	GetVertexShader(DWORD* pHandle)
							{ return (this->*pGetVertexShader)(pHandle); }
	HRESULT 			(CDevice::*pGetVertexShader)(DWORD* pHandle);

	virtual HRESULT 	GetVertexShader(CVertexShader** pShader)
							{ return (this->*pGetVertexShaderInt)(pShader); }
	HRESULT 			(CDevice::*pGetVertexShaderInt)(CVertexShader** pShader);

	virtual HRESULT 	DeleteVertexShader(CVertexShader* pShader)
							{ return (this->*pDeleteVertexShader)(pShader); }
	HRESULT 			(CDevice::*pDeleteVertexShader)(CVertexShader* pShader);

	virtual HRESULT 	SetVertexShaderConstant(UINT dwRegister, CONST void* lpvConstantData, UINT dwConstantCount)
							{ return (this->*pSetVertexShaderConstant)(dwRegister, (float*)lpvConstantData, dwConstantCount); }
	virtual HRESULT 	SetVertexShaderConstantF(UINT Register,CONST float* pConstantData,UINT ConstantCount)
							{return (this->*pSetVertexShaderConstant)(Register,pConstantData,ConstantCount); }
	HRESULT 			(CDevice::*pSetVertexShaderConstant)(UINT dwRegister, CONST float* lpvConstantData, UINT dwConstantCount);

	virtual HRESULT 	GetVertexShaderConstant(UINT Register,float* pConstantData,UINT ConstantCount)
							{ return (this->*pGetVertexShaderConstant)(Register,pConstantData,ConstantCount); }
	virtual HRESULT 	GetVertexShaderConstantF(UINT Register,float* pConstantData,UINT ConstantCount)
							{return (this->*pGetVertexShaderConstant)(Register,pConstantData,ConstantCount); }
	HRESULT 			(CDevice::*pGetVertexShaderConstant)(UINT Register,float* pConstantData,UINT ConstantCount);

	virtual HRESULT 	SetVertexShaderConstantI(UINT Register,CONST int* pConstantData,UINT ConstantCount)
							{return (this->*pSetVertexShaderConstantI)(Register,pConstantData,ConstantCount); }
	HRESULT 			(CDevice::*pSetVertexShaderConstantI)(UINT Register,CONST int* pConstantData,UINT ConstantCount);

	virtual HRESULT 	GetVertexShaderConstantI(UINT Register,int* pConstantData,UINT ConstantCount)
							{return (this->*pGetVertexShaderConstantI)(Register,pConstantData,ConstantCount); }
	HRESULT 			(CDevice::*pGetVertexShaderConstantI)(UINT Register,int* pConstantData,UINT ConstantCount);

	virtual HRESULT 	SetVertexShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData, UINT BoolCount)
							{return (this->*pSetVertexShaderConstantB)(StartRegister, pConstantData, BoolCount); }
	HRESULT 			(CDevice::*pSetVertexShaderConstantB)(UINT StartRegister,CONST BOOL* pConstantData, UINT BoolCount);

	virtual HRESULT 	GetVertexShaderConstantB(UINT StartRegister,BOOL* pConstantData, UINT BoolCount)
							{return (this->*pGetVertexShaderConstantB)(StartRegister,pConstantData, BoolCount); }
	HRESULT 			(CDevice::*pGetVertexShaderConstantB)(UINT StartRegister,BOOL* pConstantData, UINT BoolCount);

	virtual HRESULT 	GetVertexShaderDeclaration(DWORD Handle,void* pData,DWORD* pSizeOfData)
							{ return (this->*pGetVertexShaderDeclaration)(Handle,pData,pSizeOfData); }
	HRESULT 			(CDevice::*pGetVertexShaderDeclaration)(DWORD Handle,void* pData,DWORD* pSizeOfData);

	virtual HRESULT 	GetVertexShaderFunction(DWORD Handle,void* pData,DWORD* pSizeOfData)
							{ return (this->*pGetVertexShaderFunction)(Handle,pData,pSizeOfData); }
	HRESULT 			(CDevice::*pGetVertexShaderFunction)(DWORD Handle,void* pData,DWORD* pSizeOfData);

	virtual HRESULT 	SetStreamSource(UINT streamNumber, CVertexBuffer *pStreamData, UINT stride, UINT offset = 0)
							{ return (this->*pSetStreamSource)(streamNumber, pStreamData, stride, offset); }
	HRESULT 			(CDevice::*pSetStreamSource)(UINT streamNumber, CVertexBuffer *pStreamData, UINT stride, UINT offset);

	virtual HRESULT 	GetStreamSource(UINT streamNumber, CVertexBuffer **pStreamData, UINT *pStride)
							{ return (this->*pGetStreamSource)(streamNumber, pStreamData, pStride); }
	HRESULT 			(CDevice::*pGetStreamSource)(UINT streamNumber, CVertexBuffer **pStreamData, UINT *pStride);

	virtual HRESULT 	SetIndices(CIndexBuffer  *pIndexData,  UINT  baseVertexIndex)
							{ return (this->*pSetIndices)(pIndexData, baseVertexIndex); }
	HRESULT 			(CDevice::*pSetIndices)(CIndexBuffer  *pIndexData,	UINT  baseVertexIndex);

	virtual HRESULT 	GetIndices(CIndexBuffer **ppIndexData, UINT *pBaseVertexIndex)
							{ return (this->*pGetIndices)(ppIndexData, pBaseVertexIndex); }
	HRESULT 			(CDevice::*pGetIndices)(CIndexBuffer **ppIndexData, UINT *pBaseVertexIndex);

	virtual HRESULT 	CreatePixelShader(LPDWORD pdwFunction, DWORD* pdwHandle)
							{ return (this->*pCreatePixelShader)(pdwFunction, pdwHandle); }
	HRESULT 			(CDevice::*pCreatePixelShader)(LPDWORD pdwFunction, DWORD* pdwHandle);

	virtual HRESULT 	SetPixelShader(DWORD dwHandle)
							{ return (this->*pSetPixelShader)(dwHandle); }
	HRESULT 			(CDevice::*pSetPixelShader)(DWORD dwHandle);

	virtual HRESULT 	GetPixelShader(DWORD* pHandle)
							{ return (this->*pGetPixelShader)(pHandle); }
	HRESULT 			(CDevice::*pGetPixelShader)(DWORD* pHandle);

	virtual HRESULT 	DeletePixelShader(DWORD dwHandle)
							{ return (this->*pDeletePixelShader)(dwHandle); }
	HRESULT 			(CDevice::*pDeletePixelShader)(DWORD dwHandle);

	virtual HRESULT 	SetPixelShaderConstant(UINT dwRegister, CONST void* lpvConstantData, UINT dwConstantCount)
							{ return (this->*pSetPixelShaderConstant)(dwRegister, (float*)lpvConstantData, dwConstantCount); }
	virtual HRESULT 	SetPixelShaderConstantF(UINT StartRegister,const float* pConstantData,UINT Vector4fCount)
							{ return (this->*pSetPixelShaderConstant)(StartRegister, pConstantData, Vector4fCount); }
	HRESULT 			(CDevice::*pSetPixelShaderConstant)(UINT StartRegister,const float* pConstantData,UINT Vector4fCount);

	virtual HRESULT 	GetPixelShaderConstant(UINT dwRegister, void* lpvConstantData, UINT dwConstantCount)
							{ return (this->*pGetPixelShaderConstant)(dwRegister, (float*)lpvConstantData, dwConstantCount); }
	virtual HRESULT 	GetPixelShaderConstantF(UINT StartRegister,float* pConstantData,UINT Vector4fCount)
							{ return (this->*pGetPixelShaderConstant)(StartRegister, pConstantData, Vector4fCount); }
	HRESULT 			(CDevice::*pGetPixelShaderConstant)(UINT StartRegister,float* pConstantData,UINT Vector4fCount);

	virtual HRESULT 	SetPixelShaderConstantI(UINT StartRegister,const int* pConstantData,UINT Vector4iCount)
							{ return (this->*pSetPixelShaderConstantI)(StartRegister, pConstantData, Vector4iCount); }
	HRESULT 			(CDevice::*pSetPixelShaderConstantI)(UINT StartRegister,const int* pConstantData,UINT Vector4iCount);

	virtual HRESULT 	GetPixelShaderConstantI(UINT StartRegister,int* pConstantData,UINT Vector4iCount)
							{ return (this->*pGetPixelShaderConstantI)(StartRegister, pConstantData, Vector4iCount); }
	HRESULT 			(CDevice::*pGetPixelShaderConstantI)(UINT StartRegister,int* pConstantData,UINT Vector4iCount);

	virtual HRESULT 	SetPixelShaderConstantB(UINT StartRegister,const BOOL* pConstantData,UINT BoolCount)
							{ return (this->*pSetPixelShaderConstantB)(StartRegister, pConstantData, BoolCount); }
	HRESULT 			(CDevice::*pSetPixelShaderConstantB)(UINT StartRegister,const BOOL* pConstantData,UINT BoolCount);

	virtual HRESULT 	GetPixelShaderConstantB(UINT StartRegister,BOOL* pConstantData,UINT BoolCount)
							{ return (this->*pGetPixelShaderConstantB)(StartRegister, pConstantData, BoolCount); }
	HRESULT 			(CDevice::*pGetPixelShaderConstantB)(UINT StartRegister,BOOL* pConstantData,UINT BoolCount);

	virtual HRESULT 	GetPixelShaderFunction(DWORD Handle,void* pData,DWORD* pSizeOfData)
							{ return (this->*pGetPixelShaderFunction)(Handle,pData,pSizeOfData); }
	HRESULT 			(CDevice::*pGetPixelShaderFunction)(DWORD Handle,void* pData,DWORD* pSizeOfData);

	virtual HRESULT 	DrawRectPatch(UINT uHandle, float *pSegments, RECTPATCH_INFO *pInfo)
							{ return (this->*pDrawRectPatch)(uHandle, pSegments, pInfo);}
	HRESULT 			(CDevice::*pDrawRectPatch)(UINT uHandle, float *pSegments, RECTPATCH_INFO *pInfo);

	virtual HRESULT 	DrawTriPatch(UINT uHandle, float *pSegments, TRIPATCH_INFO *pInfo)
							{ return (this->*pDrawTriPatch)(uHandle, pSegments, pInfo);}
	HRESULT 			(CDevice::*pDrawTriPatch)(UINT uHandle, float *pSegments, TRIPATCH_INFO *pInfo);

	virtual HRESULT 	DeletePatch(UINT uHandle)
							{ return (this->*pDeletePatch)(uHandle);}
	HRESULT 			(CDevice::*pDeletePatch)(UINT uHandle);

	virtual HRESULT 	SetNPatchMode(float nSegments)
							{ return (this->*pSetNPatchMode)(nSegments);}
	HRESULT 			(CDevice::*pSetNPatchMode)(float nSegments);

	virtual float		GetNPatchMode()
							{ return (this->*pGetNPatchMode)();}
	float				(CDevice::*pGetNPatchMode)();

	virtual HRESULT 	ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,CVertexBuffer* pDestBuffer,DWORD Flags)
							{ return (this->*pProcessVertices)(SrcStartIndex,DestIndex,VertexCount,pDestBuffer,NULL,Flags);}

	virtual HRESULT 	ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,CVertexBuffer* pDestBuffer,CVertexDeclaration *pDecl,DWORD Flags)
							{ return (this->*pProcessVertices)(SrcStartIndex,DestIndex,VertexCount,pDestBuffer,pDecl,Flags);}
	HRESULT 			(CDevice::*pProcessVertices)(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,CVertexBuffer* pDestBuffer,CVertexDeclaration *pDecl,DWORD Flags);
	
	virtual bool		DrawPrimitiveUserMemory(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD	dwVertexTypeDesc,
							LPVOID lpVertices, DWORD  dwVertexCount, LPVOID lpIndices, DWORD  dwIndexCount, DWORD  dwFlags, bool bIs32BitIndices = false)
							{ return (this->*pDrawPrimitiveUserMemory)(d3dptPrimitiveType, dwVertexTypeDesc, lpVertices, dwVertexCount, lpIndices, dwIndexCount, dwFlags, bIs32BitIndices); }
	bool				(CDevice::*pDrawPrimitiveUserMemory)(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD  dwVertexTypeDesc,
								LPVOID lpVertices, DWORD  dwVertexCount, LPVOID lpIndices, DWORD  dwIndexCount, DWORD  dwFlags, bool bIs32BitIndices);
	
	virtual bool		DrawPrimitiveVertexBuffer(D3DPRIMITIVETYPE d3dptPrimitiveType, CVertexBuffer *pSrcBuffer, 
							DWORD dwStartVertex, DWORD dwNumVertices, LPWORD lpwIndices, DWORD dwIndexCount, DWORD dwFlags, 
							DWORD dwUsage=0, DWORD dwPool=0, bool bIs32BitIndices = false)
							{ return (this->*pDrawPrimitiveVertexBuffer)(d3dptPrimitiveType, pSrcBuffer, dwStartVertex, dwNumVertices, lpwIndices, dwIndexCount, dwFlags, dwUsage, dwPool, bIs32BitIndices); }
	bool				(CDevice::*pDrawPrimitiveVertexBuffer)(D3DPRIMITIVETYPE d3dptPrimitiveType, CVertexBuffer *pSrcBuffer,
							DWORD dwStartVertex, DWORD dwNumVertices, LPWORD lpwIndices, DWORD dwIndexCount, DWORD dwFlags,
							DWORD dwUsage, DWORD dwPool, bool bIs32BitIndices);
	
	virtual bool		DrawPrimitiveStridedData(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD  dwVertexTypeDesc, 
							LPDRAWPRIMITIVESTRIDEDDATA lpVertexArray, DWORD  dwVertexCount, LPWORD lpwIndices, DWORD  dwIndexCount, DWORD  dwFlags, 
							DWORD dwUsage=0, DWORD dwPool=0, bool bIs32BitIndices = false)
							{ return (this->*pDrawPrimitiveStridedData)(d3dptPrimitiveType, dwVertexTypeDesc, lpVertexArray, dwVertexCount, lpwIndices, dwIndexCount, dwFlags, dwUsage, dwPool, bIs32BitIndices); }
	bool				(CDevice::*pDrawPrimitiveStridedData)(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD  dwVertexTypeDesc, 
							LPDRAWPRIMITIVESTRIDEDDATA lpVertexArray, DWORD  dwVertexCount, LPWORD lpwIndices, DWORD  dwIndexCount, DWORD  dwFlags, 
							DWORD dwUsage, DWORD dwPool, bool bIs32BitIndices);

	virtual HRESULT 	DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
							{ return (this->*pDrawPrimitive)(PrimitiveType, StartVertex, PrimitiveCount); }
	HRESULT 			(CDevice::*pDrawPrimitive)(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);

	virtual HRESULT 	DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT MinIndex,UINT NumVertices,UINT StartIndex,UINT PrimitiveCount,INT BaseVertexIndex = 0)
							{ return (this->*pDrawIndexedPrimitive)(PrimitiveType, MinIndex, NumVertices, StartIndex, PrimitiveCount, BaseVertexIndex); }
	HRESULT 			(CDevice::*pDrawIndexedPrimitive)(D3DPRIMITIVETYPE PrimitiveType,UINT MinIndex,UINT NumVertices,UINT StartIndex,UINT PrimitiveCount,INT BaseVertexIndex);
	
	virtual HRESULT 	DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
							{ return (this->*pDrawPrimitiveUP)(PrimitiveType,PrimitiveCount,pVertexStreamZeroData,VertexStreamZeroStride); }
	HRESULT 			(CDevice::*pDrawPrimitiveUP)(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,void* pVertexStreamZeroData,UINT VertexStreamZeroStride);

	virtual HRESULT 	DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertexIndices,UINT PrimitiveCount,void* pIndexData,FORMAT IndexDataFormat,void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
							{ return (this->*pDrawIndexedPrimitiveUP)(PrimitiveType,MinVertexIndex,NumVertexIndices,PrimitiveCount,pIndexData,IndexDataFormat,pVertexStreamZeroData,VertexStreamZeroStride); }
	HRESULT 			(CDevice::*pDrawIndexedPrimitiveUP)(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertexIndices,UINT PrimitiveCount,void* pIndexData,FORMAT IndexDataFormat,void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	
	virtual HRESULT 	GetRasterStatus(RASTER_STATUS* pRasterStatus)
							{ return (this->*pGetRasterStatus)(pRasterStatus); }
	HRESULT 			(CDevice::*pGetRasterStatus)(RASTER_STATUS* pRasterStatus);
	
	virtual void		SetGammaRamp(DWORD Flags,GAMMARAMP* pRamp)
						   { (this->*pSetGammaRamp)(Flags,pRamp); }
	void				(CDevice::*pSetGammaRamp)(DWORD Flags,GAMMARAMP* pRamp);
	
	virtual void		GetGammaRamp(GAMMARAMP* pRamp)
							{ (this->*pGetGammaRamp)(pRamp); }
	void				(CDevice::*pGetGammaRamp)(GAMMARAMP* pRamp);

	virtual HRESULT 	GetRasterStatus(UINT uSwapchain, RASTER_STATUS* pRasterStatus)
							{ return (this->*pGetRasterStatus2)(uSwapchain, pRasterStatus); }
	HRESULT 			(CDevice::*pGetRasterStatus2)(UINT uSwapchain, RASTER_STATUS* pRasterStatus);
	
	virtual void		SetGammaRamp(UINT uSwapchain, DWORD Flags,GAMMARAMP* pRamp)
						   { (this->*pSetGammaRamp2)(uSwapchain, Flags,pRamp); }
	void				(CDevice::*pSetGammaRamp2)(UINT uSwapchain, DWORD Flags,GAMMARAMP* pRamp);
	
	virtual void		GetGammaRamp(UINT uSwapchain, GAMMARAMP* pRamp)
							{ (this->*pGetGammaRamp2)(uSwapchain, pRamp); }
	void				(CDevice::*pGetGammaRamp2)(UINT uSwapchain, GAMMARAMP* pRamp);

	virtual HRESULT 	Present(RECT *pSrc, RECT *pDst, HWND hDstWndOverride, RGNDATA *pDirtyRegion)
							{ return (this->*pPresent)(pSrc, pDst, hDstWndOverride, pDirtyRegion);}
	HRESULT 			(CDevice::*pPresent)(RECT *pSrc, RECT *pDst, HWND hDstWndOverride, RGNDATA *pDirtyRegion);

	virtual HRESULT 	SetPaletteEntries(UINT PaletteNumber,PALETTEENTRY* pEntries)
							{ return (this->*pSetPaletteEntries)(PaletteNumber,pEntries);}
	HRESULT 			(CDevice::*pSetPaletteEntries)(UINT PaletteNumber,PALETTEENTRY* pEntries);

	virtual HRESULT 	GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY* pEntries)
							{ return (this->*pGetPaletteEntries)(PaletteNumber,pEntries);}
	HRESULT 			(CDevice::*pGetPaletteEntries)(UINT PaletteNumber,PALETTEENTRY* pEntries);

	virtual HRESULT 	SetCurrentTexturePalette(UINT PaletteNumber)
							{ return (this->*pSetCurrentTexturePalette)(PaletteNumber);}
	HRESULT 			(CDevice::*pSetCurrentTexturePalette)(UINT PaletteNumber);

	virtual HRESULT 	GetCurrentTexturePalette(UINT *PaletteNumber)
							{ return (this->*pGetCurrentTexturePalette)(PaletteNumber);}
	HRESULT 			(CDevice::*pGetCurrentTexturePalette)(UINT *PaletteNumber);

	virtual HRESULT 	CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CTexture** ppTexture)
							{ return (this->*pCreateTexture)(Width,Height,Levels,Usage,Format,Pool,ppTexture);}
	HRESULT 			(CDevice::*pCreateTexture)(UINT Width,UINT Height,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CTexture** ppTexture);

	virtual HRESULT 	CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CTexture** ppTexture, HANDLE *pSharedHandle)
							{ return (this->*pCreateTexture2)(Width,Height,Levels,Usage,Format,Pool,ppTexture,pSharedHandle);}
	HRESULT 			(CDevice::*pCreateTexture2)(UINT Width,UINT Height,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CTexture** ppTexture, HANDLE *pSharedHandle);

	virtual HRESULT 	CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CVolTexture **ppVolumeTexture)
							{ return (this->*pCreateVolumeTexture)(Width,Height,Depth,Levels,Usage,Format,Pool,ppVolumeTexture);}
	HRESULT 			(CDevice::*pCreateVolumeTexture)(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CVolTexture **ppVolumeTexture);

	virtual HRESULT 	CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CVolTexture **ppVolumeTexture, HANDLE *pSharedHandle)
							{ return (this->*pCreateVolumeTexture2)(Width,Height,Depth,Levels,Usage,Format,Pool,ppVolumeTexture,pSharedHandle);}
	HRESULT 			(CDevice::*pCreateVolumeTexture2)(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CVolTexture **ppVolumeTexture, HANDLE *pSharedHandle);

	virtual HRESULT 	CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CCubeTexture **ppCubeTexture)
							{ return (this->*pCreateCubeTexture)(EdgeLength,Levels,Usage,Format,Pool,ppCubeTexture);}
	HRESULT 			(CDevice::*pCreateCubeTexture)(UINT EdgeLength,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CCubeTexture **ppCubeTexture);

	virtual HRESULT 	CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CCubeTexture **ppCubeTexture, HANDLE *pSharedHandle)
							{ return (this->*pCreateCubeTexture2)(EdgeLength,Levels,Usage,Format,Pool,ppCubeTexture,pSharedHandle);}
	HRESULT 			(CDevice::*pCreateCubeTexture2)(UINT EdgeLength,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CCubeTexture **ppCubeTexture, HANDLE *pSharedHandle);

	virtual HRESULT 	GetDirect3D( CD3D **ppD3D ) { if(ppD3D==NULL) return E_FAIL; *ppD3D = this->m_pD3D; return S_OK; }

	virtual HRESULT 	CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, DWORD Pool, CVertexBuffer** ppVB)
							{ return (this->*pCreateVertexBuffer)(Length, Usage, FVF, Pool, ppVB); }
	HRESULT 			(CDevice::*pCreateVertexBuffer)(UINT Length, DWORD Usage, DWORD FVF, DWORD Pool, CVertexBuffer** ppVB);

	virtual HRESULT 	CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, DWORD Pool, CVertexBuffer** ppVB, HANDLE *pSharedHandle)
							{ return (this->*pCreateVertexBuffer2)(Length, Usage, FVF, Pool, ppVB, pSharedHandle); }
	HRESULT 			(CDevice::*pCreateVertexBuffer2)(UINT Length, DWORD Usage, DWORD FVF, DWORD Pool, CVertexBuffer** ppVB, HANDLE *pSharedHandle);

	virtual HRESULT 	CreateIndexBuffer(UINT Length, DWORD Usage, FMT Format, DWORD Pool, CIndexBuffer** ppIB)
							{ return (this->*pCreateIndexBuffer)(Length, Usage, Format, Pool, ppIB); }
	HRESULT 			(CDevice::*pCreateIndexBuffer)(UINT Length, DWORD Usage, FMT Format, DWORD Pool, CIndexBuffer** ppIB);
  
	virtual HRESULT 	CreateIndexBuffer(UINT Length, DWORD Usage, FMT Format, DWORD Pool, CIndexBuffer** ppIB, HANDLE *pSharedHandle)
							{ return (this->*pCreateIndexBuffer2)(Length, Usage, Format, Pool, ppIB, pSharedHandle); }
	HRESULT 			(CDevice::*pCreateIndexBuffer2)(UINT Length, DWORD Usage, FMT Format, DWORD Pool, CIndexBuffer** ppIB, HANDLE *pSharedHandle);
  
	virtual HRESULT 	GetSwapChain(UINT iSwapChain, CSwapChain** pSwapChain)
							{ return (this->*pGetSwapChain)(iSwapChain, pSwapChain); }
	HRESULT 			(CDevice::*pGetSwapChain)(UINT iSwapChain, CSwapChain** pSwapChain);

	virtual UINT		GetNumberOfSwapChains()
							{ return (this->*pGetNumberOfSwapChains)(); }
	UINT				(CDevice::*pGetNumberOfSwapChains)();

	virtual HRESULT 	CreateQuery(QUERYTYPE Type, CQuery** pQuery)
							{ return (this->*pCreateQuery)(Type, pQuery); }
	HRESULT 			(CDevice::*pCreateQuery)(QUERYTYPE Type, CQuery** pQuery);

	virtual HRESULT 	SetStreamSourceFreq(UINT nStreamNumber, UINT nDivider)
						{ return (this->*pSetStreamSourceFreq)(nStreamNumber, nDivider); }
	HRESULT 			(CDevice::*pSetStreamSourceFreq)(UINT nStreamNumber, UINT nDivider);

	virtual HRESULT 	GetStreamSourceFreq(UINT nStreamNumber, UINT* pDividerOut)
						{ return (this->*pGetStreamSourceFreq)(nStreamNumber, pDividerOut); }
	HRESULT 			(CDevice::*pGetStreamSourceFreq)(UINT nStreamNumber, UINT* pDividerOut);

	virtual void		D3DXInitializePSGP()
							{ (this->*pD3DXInitializePSGP)(); }
	void				(CDevice::*pD3DXInitializePSGP)();

	virtual HRESULT 	D3DXCreateEffectFromFile(LPCTSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors)
							{ return (this->*pD3DXCreateEffectFromFile)(pSrcFile,pDefines, pInclude, dwFlags, pPool, ppEffect,ppCompilationErrors); }	
	HRESULT 			(CDevice::*pD3DXCreateEffectFromFile)(LPCTSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors);

	virtual HRESULT 	CreateEffectFromFile(LPCTSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, CEffect** ppEffect, LPD3DXBUFFER* ppCompilationErrors)
							{ return (this->*pCreateEffectFromFile)(pSrcFile,pDefines, pInclude, dwFlags, pPool, ppEffect,ppCompilationErrors); }	
	HRESULT 			(CDevice::*pCreateEffectFromFile)(LPCTSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, CEffect** ppEffect, LPD3DXBUFFER* ppCompilationErrors);

	virtual HRESULT 	D3DXCreateEffectFromResource(HMODULE hSrcModule, LPCTSTR pSrcresource, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors)
							{ return (this->*pD3DXCreateEffectFromResource)(hSrcModule,pSrcresource,pDefines, pInclude, dwFlags, pPool, ppEffect,ppCompilationErrors); }	
	HRESULT 			(CDevice::*pD3DXCreateEffectFromResource)(HMODULE hSrcModule, LPCTSTR pSrcresource, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors);

	virtual HRESULT 	CreateEffectFromResource(HMODULE hSrcModule, LPCTSTR pSrcresource, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, CEffect** ppEffect, LPD3DXBUFFER* ppCompilationErrors)
							{ return (this->*pCreateEffectFromResource)(hSrcModule,pSrcresource,pDefines, pInclude, dwFlags, pPool, ppEffect,ppCompilationErrors); }	
	HRESULT 			(CDevice::*pCreateEffectFromResource)(HMODULE hSrcModule, LPCTSTR pSrcresource, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, CEffect** ppEffect, LPD3DXBUFFER* ppCompilationErrors);

	virtual HRESULT 	D3DXCreateMesh(DWORD NumFaces, DWORD NumVertices, DWORD Options, CONST DWORD* pDeclaration, LPD3DXMESH* ppMesh)
							{ return (this->*pD3DXCreateMesh)(NumFaces, NumVertices, Options, pDeclaration, ppMesh); }
	HRESULT 			(CDevice::*pD3DXCreateMesh)(DWORD NumFaces, DWORD NumVertices, DWORD Options, CONST DWORD* pDeclaration, LPD3DXMESH* ppMesh);

	virtual HRESULT 	D3DXCreateMeshFVF(DWORD NumFaces, DWORD NumVertices, DWORD Options, DWORD FVF, LPD3DXMESH* ppMesh)
							{ return (this->*pD3DXCreateMeshFVF)(NumFaces,NumVertices,Options,FVF, ppMesh); }
	HRESULT 			(CDevice::*pD3DXCreateMeshFVF)(DWORD NumFaces, DWORD NumVertices, DWORD Options, DWORD FVF, LPD3DXMESH* ppMesh);

	virtual HRESULT 	D3DXCreateTexture(UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, CTexture** ppTexture)
							{ return (this->*pD3DXCreateTexture)(Width, Height,MipLevels,Usage,Format,Pool, ppTexture); }
	HRESULT 			(CDevice::*pD3DXCreateTexture)(UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, CTexture** ppTexture);
   
	virtual HRESULT 	D3DXCreateTextureFromFileA(LPCSTR pSrcFile, CTexture** ppTexture)
							{ return (this->*pD3DXCreateTextureFromFileA)(pSrcFile, ppTexture); }
	HRESULT 			(CDevice::*pD3DXCreateTextureFromFileA)(LPCSTR pSrcFile, CTexture** ppTexture);
	   
	virtual HRESULT 	D3DXCreateTextureFromFileExA(LPCSTR pSrcFile, UINT Size, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, CTexture** ppTexture)
							{ return (this->*pD3DXCreateTextureFromFileExA)(pSrcFile, Size, Width, Height, MipLevels, Usage, Format,Pool,Filter,MipFilter,ColorKey,pSrcInfo,pPalette, ppTexture); }
	HRESULT 			(CDevice::*pD3DXCreateTextureFromFileExA)(LPCSTR pSrcFile, UINT Size, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, CTexture** ppTexture);
	   
	virtual HRESULT 	D3DXCreateTextureFromFileExW(LPCWSTR pSrcFile, UINT Size, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, CTexture** ppTexture)
							{ return (this->*pD3DXCreateTextureFromFileExW)(pSrcFile, Size, Width, Height, MipLevels, Usage, Format, Pool,Filter, MipFilter,ColorKey, pSrcInfo, pPalette, ppTexture); }
	HRESULT 			(CDevice::*pD3DXCreateTextureFromFileExW)(LPCWSTR pSrcFile, UINT Size, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, CTexture** ppTexture);
	   
	virtual HRESULT 	D3DXCreateTextureFromFileInMemory(LPCVOID pSrcData, UINT SrcData, CTexture** ppTexture)
							{ return (this->*pD3DXCreateTextureFromFileInMemory)(pSrcData, SrcData, ppTexture); }
	HRESULT 			(CDevice::*pD3DXCreateTextureFromFileInMemory)(LPCVOID pSrcData, UINT SrcData, CTexture** ppTexture);
	   
	virtual HRESULT 	D3DXCreateTextureFromFileInMemoryEx(LPCVOID pSrcData, UINT SrcData, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, CTexture** ppTexture)
							{ return (this->*pD3DXCreateTextureFromFileInMemoryEx)(pSrcData, SrcData, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture); }
	HRESULT 			(CDevice::*pD3DXCreateTextureFromFileInMemoryEx)(LPCVOID pSrcData, UINT SrcData, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, CTexture** ppTexture);
	   
	virtual HRESULT 	D3DXCreateTextureFromFileW(LPCWSTR pSrcFile, CTexture** ppTexture)
							{ return (this->*pD3DXCreateTextureFromFileW)(pSrcFile, ppTexture); }
	HRESULT 			(CDevice::*pD3DXCreateTextureFromFileW)(LPCWSTR pSrcFile, CTexture** ppTexture);
	   

	virtual HRESULT 	D3DXCreateTextureFromResourceA(HMODULE hSrcModule, LPCSTR pSrcResource, CTexture** ppTexture) 
							{ return (this->*pD3DXCreateTextureFromResourceA)(hSrcModule, pSrcResource, ppTexture); }
	HRESULT 			(CDevice::*pD3DXCreateTextureFromResourceA)(HMODULE hSrcModule, LPCSTR pSrcResource, CTexture** ppTexture);

	virtual HRESULT 	D3DXCreateTextureFromResourceW(HMODULE hSrcModule, LPCWSTR pSrcResource, CTexture** ppTexture) 
							{ return (this->*pD3DXCreateTextureFromResourceW)(hSrcModule, pSrcResource, ppTexture); }
	HRESULT 			(CDevice::*pD3DXCreateTextureFromResourceW)(HMODULE hSrcModule, LPCWSTR pSrcResource, CTexture** ppTexture);
	   
	virtual HRESULT 	D3DXCreateTextureFromResourceExA(HMODULE hSrcModule, LPCSTR pSrcResource, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format,
														 D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, CTexture** ppTexture) 
							{ return (this->*pD3DXCreateTextureFromResourceExA)(hSrcModule,pSrcResource,Width,Height,MipLevels,Usage,Format,Pool,Filter,MipFilter,ColorKey,pSrcInfo,pPalette,ppTexture); }
	HRESULT 			(CDevice::*pD3DXCreateTextureFromResourceExA)(HMODULE hSrcModule, LPCSTR pSrcResource, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, CTexture** ppTexture);
	   
	virtual HRESULT 	D3DXCreateTextureFromResourceExW(HMODULE hSrcModule, LPCWSTR pSrcResource, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, CTexture** ppTexture)
							{ return (this->*pD3DXCreateTextureFromResourceExW)(hSrcModule, pSrcResource, Width,Height, MipLevels, Usage, Format,Pool,Filter, MipFilter,ColorKey, pSrcInfo, pPalette, ppTexture); }
	HRESULT 			(CDevice::*pD3DXCreateTextureFromResourceExW)(HMODULE hSrcModule, LPCWSTR pSrcResource, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, CTexture** ppTexture);

	virtual HRESULT 	D3DXEffect_CloneEffect(LPD3DXEFFECT pEffect, CDevice* pTargetDevice, LPD3DXEFFECT* ppResultEffect)
							{ return (this->*pD3DXEffect_CloneEffect)(pEffect,	pTargetDevice,	ppResultEffect); }
	HRESULT 			(CDevice::*pD3DXEffect_CloneEffect)(LPD3DXEFFECT pEffect, CDevice* pTargetDevice, LPD3DXEFFECT* ppResultEffect);

	virtual HRESULT 	D3DXFilterTexture(CBaseTexture* pBaseTexture,CONST PALETTEENTRY* pPalette,UINT SrcLevel,DWORD MipFilter)
							{ return (this->*pD3DXFilterTexture)(pBaseTexture, pPalette, SrcLevel, MipFilter); }
	HRESULT 			(CDevice::*pD3DXFilterTexture)(CBaseTexture* pBaseTexture,CONST PALETTEENTRY* pPalette,UINT SrcLevel,DWORD MipFilter);

	virtual HRESULT 	D3DXLoadVolumeFromMemory(CVolume* pDestVolume,CONST PALETTEENTRY* pDestPalette,CONST BOX* pDestBox,LPCVOID	pSrcMemory,FORMAT SrcFormat,UINT SrcRowPitch,UINT SrcSlicePitch,CONST PALETTEENTRY* pSrcPalette,CONST BOX* pSrcBox,DWORD Filter,D3DCOLOR ColorKey)
							{ return (this->*pD3DXLoadVolumeFromMemory)(pDestVolume,pDestPalette,pDestBox,pSrcMemory,SrcFormat,SrcRowPitch,SrcSlicePitch, pSrcPalette, pSrcBox,Filter,ColorKey); }
	HRESULT 			(CDevice::*pD3DXLoadVolumeFromMemory)(CVolume* pDestVolume,CONST PALETTEENTRY* pDestPalette,CONST BOX* pDestBox,LPCVOID  pSrcMemory,FORMAT SrcFormat,UINT SrcRowPitch,UINT SrcSlicePitch,CONST PALETTEENTRY* pSrcPalette,CONST BOX* pSrcBox,DWORD Filter,D3DCOLOR ColorKey);
	   
	virtual HRESULT 	D3DXLoadVolumeFromVolume(CVolume* pDestVolume,CONST PALETTEENTRY* pDestPalette,CONST BOX* pDestBox,CVolume* pSrcVolume,CONST PALETTEENTRY* pSrcPalette,CONST BOX* pSrcBox,DWORD Filter,D3DCOLOR ColorKey)
							{ return (this->*pD3DXLoadVolumeFromVolume)(pDestVolume,pDestPalette, pDestBox,pSrcVolume,pSrcPalette,pSrcBox,Filter,ColorKey); }
	HRESULT 			(CDevice::*pD3DXLoadVolumeFromVolume)(CVolume* pDestVolume,CONST PALETTEENTRY* pDestPalette,CONST BOX* pDestBox,CVolume* pSrcVolume,CONST PALETTEENTRY* pSrcPalette,CONST BOX* pSrcBox,DWORD Filter,D3DCOLOR ColorKey);

	virtual HRESULT 	D3DXLoadSurfaceFromSurface(CSurface* pDestSurface,CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey)
							{ return (this->*pD3DXLoadSurfaceFromSurface)(pDestSurface,pDestPalette, pDestRect, pSrcSurface, pSrcPalette, pSrcRect, Filter, ColorKey); }
	HRESULT 			(CDevice::*pD3DXLoadSurfaceFromSurface)(CSurface* pDestSurface,CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey);
	   
	virtual HRESULT 	D3DXLoadSurfaceFromFileInMemory(CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCVOID pSrcData, UINT SrcData, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo)
							{ return (this->*pD3DXLoadSurfaceFromFileInMemory)(pDestSurface, pDestPalette, pDestRect, pSrcData, SrcData, pSrcRect, Filter, ColorKey,  pSrcInfo); }
	HRESULT 			(CDevice::*pD3DXLoadSurfaceFromFileInMemory)(CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCVOID pSrcData, UINT SrcData, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo);
		
	virtual HRESULT 	D3DXLoadSurfaceFromFile(CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCSTR sFilename, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo)
							{ return (this->*pD3DXLoadSurfaceFromFile)(pDestSurface, pDestPalette, pDestRect, sFilename, pSrcRect, Filter, ColorKey, pSrcInfo); }
	HRESULT 			(CDevice::*pD3DXLoadSurfaceFromFile)(CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCSTR sFilename, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo);

	virtual HRESULT 	D3DXLoadSurfaceFromMemory( CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCVOID pSrcMemory, D3DFORMAT SrcFormat, UINT SrcPitch, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey)
							{ return (this->*pD3DXLoadSurfaceFromMemory)( pDestSurface, pDestPalette, pDestRect, pSrcMemory, SrcFormat, SrcPitch, pSrcPalette, pSrcRect, Filter, ColorKey); }
	HRESULT 			(CDevice::*pD3DXLoadSurfaceFromMemory)( CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCVOID pSrcMemory, D3DFORMAT SrcFormat, UINT SrcPitch, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey);
	   
	virtual HRESULT 	D3DXSaveTextureToFileA (LPCSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat, LPBASETEXTURE pSrcTexture, CONST PALETTEENTRY* pSrcPalette)
							{ return (this->*pD3DXSaveTextureToFileA)(pDestFile, DestFormat, pSrcTexture, pSrcPalette); }
	HRESULT 			(CDevice::*pD3DXSaveTextureToFileA) (LPCSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat, LPBASETEXTURE pSrcTexture, CONST PALETTEENTRY* pSrcPalette);
	   
	virtual HRESULT 	D3DXSaveTextureToFileW (LPCWSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat, LPBASETEXTURE pSrcTexture, CONST PALETTEENTRY* pSrcPalette)
							{ return (this->*pD3DXSaveTextureToFileW)(pDestFile, DestFormat, pSrcTexture, pSrcPalette); }
	HRESULT 			(CDevice::*pD3DXSaveTextureToFileW) (LPCWSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat, LPBASETEXTURE pSrcTexture, CONST PALETTEENTRY* pSrcPalette);

	virtual HRESULT 	D3DXSaveSurfaceToFileA (LPCSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect)
							{ return (this->*pD3DXSaveSurfaceToFileA)(pDestFile, DestFormat, pSrcSurface, pSrcPalette, pSrcRect); }
	HRESULT 			(CDevice::*pD3DXSaveSurfaceToFileA) (LPCSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect);
	   
	virtual HRESULT 	D3DXSaveSurfaceToFileW (LPCWSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect)
							{ return (this->*pD3DXSaveSurfaceToFileW)(pDestFile, DestFormat, pSrcSurface, pSrcPalette, pSrcRect); }
	HRESULT 			(CDevice::*pD3DXSaveSurfaceToFileW) (LPCWSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect);

	virtual HRESULT 	D3DXFillTexture( LPTEXTURE pTexture, void* pFunction, LPVOID pData )
							{ return (this->*pD3DXFillTexture)(pTexture, pFunction, pData); }
	HRESULT 			(CDevice::*pD3DXFillTexture)( LPTEXTURE pTexture, void* pFunction, LPVOID pData );

	virtual HRESULT 	D3DXTessellateRectPatch( CVertexBuffer* pVertexBuffer, CONST float* pNumSegs, VERTEXELEMENT* pInDecl, CONST RECTPATCH_INFO* pRectPatchInfo, LPD3DXMESH pMesh )
							{ return (this->*pD3DXTessellateRectPatch)(pVertexBuffer, pNumSegs, pInDecl, pRectPatchInfo, pMesh); }
	HRESULT 			(CDevice::*pD3DXTessellateRectPatch)( CVertexBuffer* pVertexBuffer, CONST float* pNumSegs, VERTEXELEMENT* pInDecl, CONST RECTPATCH_INFO* pRectPatchInfo, LPD3DXMESH pMesh );
	
	virtual HRESULT 	D3DXTessellateTriPatch( CVertexBuffer* pVertexBuffer, CONST float* pNumSegs, VERTEXELEMENT* pInDecl, CONST TRIPATCH_INFO* pTriPatchInfo, LPD3DXMESH pMesh )
							{ return (this->*pD3DXTessellateTriPatch)(pVertexBuffer, pNumSegs, pInDecl, pTriPatchInfo, pMesh); }
	HRESULT 			(CDevice::*pD3DXTessellateTriPatch)( CVertexBuffer* pVertexBuffer, CONST float* pNumSegs, VERTEXELEMENT* pInDecl, CONST TRIPATCH_INFO* pTriPatchInfo, LPD3DXMESH pMesh );

	virtual HRESULT 	StretchRect(CSurface* pSourceSurface,CONST RECT* pSourceRect,CSurface* pDestSurface,CONST RECT* pDestRect, DWORD dwFilter)
							{return (this->*pStretchRect)(pSourceSurface,pSourceRect,pDestSurface,pDestRect, dwFilter); }
	HRESULT 			(CDevice::*pStretchRect)(CSurface* pSourceSurface,CONST RECT* pSourceRect,CSurface* pDestSurface,CONST RECT* pDestRect, DWORD dwFilter);

	virtual HRESULT 	ColorFill(CSurface* pSurface,CONST RECT* pRect,DWORD color)
							{return (this->*pColorFill)(pSurface,pRect,color); }
	HRESULT 			(CDevice::*pColorFill)(CSurface* pSurface,CONST RECT* pRect,DWORD color);

	virtual HRESULT 	CreateOffscreenPlainSurface(UINT Width,UINT Height,FORMAT Format,DWORD dwPool, CSurface** ppSurface, HANDLE *pSharedHandle = NULL)
							{return (this->*pCreateOffscreenPlainSurface)(Width,Height,Format,dwPool,ppSurface,pSharedHandle); }
	HRESULT 			(CDevice::*pCreateOffscreenPlainSurface)(UINT Width,UINT Height,FORMAT Format, DWORD dwPool, CSurface** ppSurface, HANDLE *pSharedHandle);

	virtual HRESULT 	SetScissorRect(CONST RECT* pRect)
							{return (this->*pSetScissorRect)(pRect); }
	HRESULT 			(CDevice::*pSetScissorRect)(CONST RECT* pRect);

	virtual HRESULT 	GetScissorRect(RECT* pRect)
							{return (this->*pGetScissorRect)(pRect); }
	HRESULT 			(CDevice::*pGetScissorRect)(RECT* pRect);

	virtual HRESULT 	SetVertexDeclaration(CVertexDeclaration* pDecl)
							{return (this->*pSetVertexDeclaration)(pDecl); }
	HRESULT 			(CDevice::*pSetVertexDeclaration)(CVertexDeclaration* pDecl);

	virtual HRESULT 	GetVertexDeclaration(CVertexDeclaration** ppDecl)
							{return (this->*pGetVertexDeclaration)(ppDecl); }
	HRESULT 			(CDevice::*pGetVertexDeclaration)(CVertexDeclaration** ppDecl);

	virtual HRESULT 	GetFVF(DWORD* pFVF)
							{return (this->*pGetFVF)(pFVF); }
	HRESULT 			(CDevice::*pGetFVF)(DWORD* pFVF);

	virtual HRESULT 	CreatePixelShader(LPDWORD pdwFunction, CPShader** pdwHandle)
							{ return (this->*pCreatePixelShaderInt)(pdwFunction, pdwHandle); }
	HRESULT 			(CDevice::*pCreatePixelShaderInt)(LPDWORD pdwFunction, CPShader** pdwHandle);

	virtual HRESULT 	SetPixelShader(CPShader* pShader)
							{return (this->*pSetPixelShaderInt)(pShader); }
	HRESULT 			(CDevice::*pSetPixelShaderInt)(CPShader* pShader);

	virtual HRESULT 	GetPixelShader(CPShader** ppShader)
							{return (this->*pGetPixelShaderInt)(ppShader); }
	HRESULT 			(CDevice::*pGetPixelShaderInt)(CPShader** ppShader);

	virtual HRESULT 	DeletePixelShader(CPShader* pShader)
							{return (this->*pDeletePixelShaderInt)(pShader); }
	HRESULT 			(CDevice::*pDeletePixelShaderInt)(CPShader* pShader);

	virtual HRESULT 	SetSamplerState(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD Value)
							{return (this->*pSetSamplerState)(Sampler, Type, Value); }
	HRESULT 			(CDevice::*pSetSamplerState)(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD Value);

	virtual HRESULT 	GetSamplerState(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD *Value)
							{return (this->*pGetSamplerState)(Sampler, Type, Value); }
	HRESULT 			(CDevice::*pGetSamplerState)(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD *Value);

	virtual HRESULT 	SetDialogBoxMode(BOOL bEnableDialogs)
							{return (this->*pSetDialogBoxMode)(bEnableDialogs); }
	HRESULT 			(CDevice::*pSetDialogBoxMode)(BOOL bEnableDialogs);

	virtual HRESULT 	SetSoftwareVertexProcessing(BOOL bSoftware)
							{return (this->*pSetSoftwareVertexProcessing)(bSoftware); }
	HRESULT 			(CDevice::*pSetSoftwareVertexProcessing)(BOOL bSoftware);
	
	virtual BOOL		GetSoftwareVertexProcessing()
							{return (this->*pGetSoftwareVertexProcessing)(); }
	BOOL				(CDevice::*pGetSoftwareVertexProcessing)();

	// 9.L methods
	virtual HRESULT 	SetConvolutionMonoKernel(UINT uWidth, UINT uHeight, float * pfRows, float *pfCols)
							{return (this->*pSetConvolutionMonoKernel)(uWidth,uHeight,pfRows,pfCols);}
	HRESULT 			(CDevice::*pSetConvolutionMonoKernel)(UINT,UINT,float *, float *);

	virtual HRESULT 	ComposeRects( LPSURFACE pSrc, LPSURFACE pDst, LPVERTEXBUFFER pSrcRectDescs, UINT NumRects, LPVERTEXBUFFER pDstRectDescs, UINT Operation, int Xoffset, int Yoffset )
							{return (this->*pComposeRects)( pSrc, pDst, pSrcRectDescs, NumRects, pDstRectDescs, Operation, Xoffset, Yoffset );}
	HRESULT 			(CDevice::*pComposeRects)( LPSURFACE ,LPSURFACE, LPVERTEXBUFFER, UINT, LPVERTEXBUFFER, UINT, int, int );

	virtual HRESULT 	PresentEx( CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags )
							{return (this->*pPresentEx)( pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags ); }
	HRESULT 			(CDevice::*pPresentEx)( CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*, DWORD );

	virtual HRESULT		GetGPUThreadPriority(INT * pPriority)
							{return (this->*pGetGPUThreadPriority)(pPriority);}
	HRESULT				(CDevice::*pGetGPUThreadPriority)(INT*);

	virtual HRESULT		SetGPUThreadPriority(INT Priority)
							{return (this->*pSetGPUThreadPriority)(Priority);}
	HRESULT				(CDevice::*pSetGPUThreadPriority)(INT);

	virtual HRESULT		WaitForVBlank(UINT SwapChainIndex)
							{return (this->*pWaitForVBlank)(SwapChainIndex); }
	HRESULT (CDevice::*pWaitForVBlank)(UINT);
	
	virtual HRESULT		CheckResourceResidency(CResource** pResourceArray, UINT numResources)
							{return (this->*pCheckResourceResidency)(pResourceArray, numResources);}
	HRESULT (CDevice::*pCheckResourceResidency)(CResource**, UINT);

	virtual HRESULT		SetMaximumFrameLatency(UINT MaxLatency)
							{return (this->*pSetMaximumFrameLatency)(MaxLatency);}
	HRESULT (CDevice::*pSetMaximumFrameLatency)(UINT);

	virtual HRESULT		GetMaximumFrameLatency(UINT* pMaxLatency)
							{return (this->*pGetMaximumFrameLatency)(pMaxLatency);}
	HRESULT (CDevice::*pGetMaximumFrameLatency)(UINT*);

	virtual HRESULT		CheckDeviceState(HWND hWindow)
							{return (this->*pCheckDeviceState)(hWindow);}
	HRESULT (CDevice::*pCheckDeviceState)(HWND);

	virtual HRESULT		CreateRenderTargetEx(UINT Width, UINT Height, FORMAT Format, MULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable,CSurface **ppSurface, HANDLE *pSharedHandle = NULL, DWORD dwUsage = 0)
							{return (this->*pCreateRenderTargetEx)(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle, dwUsage);}
	HRESULT (CDevice::*pCreateRenderTargetEx)(UINT, UINT, FORMAT, MULTISAMPLE_TYPE, DWORD, BOOL, CSurface**, HANDLE*, DWORD);

	virtual HRESULT		CreateOffscreenPlainSurfaceEx(UINT Width, UINT Height, FORMAT Format, DWORD dwPool, CSurface** ppSurface, HANDLE *pSharedHandle = NULL, DWORD dwUsage = 0)
							{return (this->*pCreateOffscreenPlainSurfaceEx)(Width, Height, Format, dwPool, ppSurface, pSharedHandle, dwUsage);}
	HRESULT (CDevice::*pCreateOffscreenPlainSurfaceEx)(UINT, UINT, FORMAT, DWORD, CSurface**, HANDLE*, DWORD);

	virtual HRESULT		CreateDepthStencilSurfaceEx(UINT Width,UINT Height,FORMAT Format, MULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, CSurface **ppSurface, HANDLE *pSharedHandle = NULL, DWORD dwUsage = 0)
							{return (this->*pCreateDepthStencilSurfaceEx)(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle, dwUsage);}
	HRESULT (CDevice::*pCreateDepthStencilSurfaceEx)(UINT, UINT, FORMAT, MULTISAMPLE_TYPE, DWORD, BOOL, CSurface**, HANDLE*, DWORD);


	virtual HRESULT		ResetEx(PRESENT_PARAMETERS* pPresentationParameters,D3DDISPLAYMODEEX *pFullscreenDisplayMode)
							{return (this->*pResetEx)(pPresentationParameters,pFullscreenDisplayMode);}
	HRESULT (CDevice::*pResetEx)(PRESENT_PARAMETERS*,D3DDISPLAYMODEEX *);

/*	virtual HRESULT		GetDisplayModeEx(UINT iSwapChain,D3DDISPLAYMODEEX* pMode)
							{return (this->*pGetDisplayModeEx(iSwapChain, pMode);}
	HRESULT (CDevice::*pGetDisplayModeEx)(UINT, D3DDISPLAYMODEEX*);
*/


protected:
	
	//---------------------------------------------------------------------
	// Internal variables for API wrapper and cycling. (RP prefix)

	RENDERPRIMITIVEAPI		m_dwRPCurrentAPI;				// Current API	  

	LPVOID					m_pRPVertexArray;				// Pointer to the internal buffer used in API conversion
	DWORD					m_dwRPVertexArraySize;			// Size (in bytes) of this array
	LPWORD					m_pRPIndexArray;				// Pointer to the internal buffer used in API conversion
	DWORD					m_dwRPIndexArraySize;			// Size (in WORDs) of this array

	bool					m_bRPLighting;					// Save D3DRENDERSTATE_LIGHTING for device
	bool					m_bRPClipping;					// Save D3DRENDERSTATE_CLIPPING for device
	
	DWORD					m_dwDestinationFVF; 			// Destination vertex format for ProcessVertices calls
	
	//---------------------------------------------------------------------
	// Information about D3DDP_* to D3DRENDERSTATE_* conversion

	bool					m_fClippingRSChanged;
	bool					m_fLightingRSChanged;
	bool					m_fExtentsRSChanged;

	virtual void			InitializeAPICycling(void); 	
	virtual void			ReleaseAPICycling(void);
	virtual DWORD			GetPrimitiveCount(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD dwVertexCount);
	virtual LPVOID			CreateVertexArray(LPVOID lpVertices, DWORD	dwVertexCount, DWORD dwVertexSize, 
								LPVOID lpIndices, DWORD  dwIndexCount, bool bIs32BitIndices);
	virtual LPWORD			CreateIndexArray(DWORD dwSize, bool bIs32BitIndices);

	// Functions to convert DX6 style DrawPrimitive flags to SetRenderState calls
	virtual DWORD			ProcessDrawPrimitiveFlags(DWORD dwFlags);
	virtual void			RestoreDrawPrimitiveFlags(void);


	// Wrapped GetDepthStencilSurface behavior: These methods are used to control whether
	// a call to GetDepthStencilSurface that returns D3DERR_NOTFOUND will be accompanied by
	// the generation of debug output.	The default behavior is to generate debug output for
	// all calls that do not return D3D_OK.  Use "SetSuppressGetDepthStencilDBGOutput(true)"
	// if you want the framework to generate debug output only when GetDepthStencilSurface
	// returns D3DERR_INVALIDCALL.
protected:
	bool bSuppressGetDepthStencilDBGOutput;
public:
	void SetSuppressGetDepthStencilDBGOutput(bool bSetting) {bSuppressGetDepthStencilDBGOutput = bSetting;}
	bool GetSuppressGetDepthStencilDBGOutput() {return bSuppressGetDepthStencilDBGOutput;}
};

class CStateblock : public CObject
{
	friend class CDevice;

	LPDIRECT3DSTATEBLOCK9	m_pStateblock9;
	DWORD					m_dwStateblock;
	CDevice 			   *m_pDevice;

	HRESULT 			(CStateblock::*pCapture)();
	HRESULT 			Capture9();
	HRESULT 			CaptureUnsup();

	HRESULT 			(CStateblock::*pApply)();
	HRESULT 			Apply9();
	HRESULT 			ApplyUnsup();

	HRESULT 			(CStateblock::*pGetDevice)(PVOID *pDevice);
	HRESULT 			GetDevice9(PVOID *pDevice);
	HRESULT 			GetDeviceUnsup(PVOID *pDevice);

	bool				Init(PVOID pStateblock, CDevice *pDevice);
	bool				Init9(PVOID pStateblock);

public:
	CStateblock();
	~CStateblock();

	virtual ULONG	WINAPI AddRef(void);
	virtual ULONG	WINAPI Release(void);

	virtual HRESULT 	Capture(){ return (this->*pCapture)();}
	virtual HRESULT 	Apply(){ return (this->*pApply)();}
	virtual HRESULT 	GetDevice(PVOID *pDevice){ return (this->*pGetDevice)(pDevice);}
};

class CVertexShader : public CObject
{
protected:

	friend class CDevice;
	friend class CEffect;

	LPDIRECT3DVERTEXSHADER9 	m_pVS9;
	DWORD					m_dwVS;
	CDevice 			   *m_pDevice;
	CVertexDeclaration				   *m_pDecl;

	HRESULT 			(CVertexShader::*pGetFunction)(void*pDate, UINT* pSizeOfData);
	HRESULT 			GetFunction9(void*pDate, UINT* pSizeOfData);
	HRESULT 			GetFunctionUnsup(void*pDate, UINT* pSizeOfData);

	HRESULT 			(CVertexShader::*pGetDevice)(PVOID *pDevice);
	HRESULT 			GetDevice9(PVOID *pDevice);
	HRESULT 			GetDeviceUnsup(PVOID *pDevice);

	bool				Init(PVOID pVS, CDevice *pDevice);
	bool				Init9(PVOID pVS);
	void				SetDecl(CVertexDeclaration *pDecl);

	inline LPDIRECT3DVERTEXSHADER9 GetShader9() { return m_pVS9; }
	inline DWORD GetShader8() { return m_dwVS; }

public:
	CVertexShader();
	~CVertexShader();

	virtual ULONG	WINAPI AddRef(void);
	virtual ULONG	WINAPI Release(void);

	virtual HRESULT 	GetFunction(void*pDate, UINT* pSizeOfData){ return (this->*pGetFunction)(pDate, pSizeOfData);}
	virtual HRESULT 	GetDevice(PVOID *pDevice){ return (this->*pGetDevice)(pDevice);}
};

class CnVertexShader : public CObject
{
private:
	CVertexShader** 	m_pShaders;
	UINT			m_uDevices;
	bool			SkipDevice(UINT uDevice, DWORD dwFlags);

public:
	CnVertexShader();
	~CnVertexShader();
	virtual bool	Init(UINT uDevices);
	virtual bool	GetVShader(CVertexShader **ppVShader, UINT n);
	virtual bool	SetVShader(CVertexShader *pVShader, UINT n);

	// TODO: Add other methods for vertex shader...
};

class CPShader : public CObject
{
protected:

	friend class CDevice;
	friend class CEffect;

	LPDIRECT3DPIXELSHADER9		m_pPS9;
	DWORD					m_dwPS;
	CDevice 			   *m_pDevice;

	HRESULT 			(CPShader::*pGetFunction)(void*pDate, UINT* pSizeOfData);
	HRESULT 			GetFunction9(void*pDate, UINT* pSizeOfData);
	HRESULT 			GetFunctionUnsup(void*pDate, UINT* pSizeOfData);

	HRESULT 			(CPShader::*pGetDevice)(PVOID *pDevice);
	HRESULT 			GetDevice9(PVOID *pDevice);
	HRESULT 			GetDeviceUnsup(PVOID *pDevice);

	bool				Init(PVOID pPS, CDevice *pDevice);
	bool				Init9(PVOID pPS);

	inline LPDIRECT3DPIXELSHADER9 GetShader9() { return m_pPS9; }
	inline DWORD GetShader8() { return m_dwPS; }

public:
	CPShader();
	~CPShader();

	virtual ULONG	WINAPI AddRef(void);
	virtual ULONG	WINAPI Release(void);

	virtual HRESULT 	GetFunction(void*pDate, UINT* pSizeOfData){ return (this->*pGetFunction)(pDate, pSizeOfData);}
	virtual HRESULT 	GetDevice(PVOID *pDevice){ return (this->*pGetDevice)(pDevice);}
};

class CnPixelShader : public CObject
{
private:
	CPShader**		m_pShaders;
	UINT			m_uDevices;
	bool			SkipDevice(UINT uDevice, DWORD dwFlags);

public:
	CnPixelShader();
	~CnPixelShader();
	virtual bool	Init(UINT uDevices);
	virtual bool	GetPShader(CPShader **ppPShader, UINT n);
	virtual bool	SetPShader(CPShader *pPShader, UINT n);

	// TODO: Add other methods for vertex shader...
};

class CVertexDeclaration : public CObject
{
	friend class CDevice;

	LPDIRECT3DVERTEXDECLARATION9		m_pDecl9;

	CDevice 			   *m_pDevice;

	HRESULT 			(CVertexDeclaration::*pGetDeclaration)(VERTEXELEMENT*pDate, UINT* pSizeOfData);
	HRESULT 			GetDeclaration9(VERTEXELEMENT*pData, UINT* pSizeOfData);
	HRESULT 			GetDeclarationUnsup(VERTEXELEMENT*pData, UINT* pSizeOfData);

	HRESULT 			(CVertexDeclaration::*pGetDevice)(PVOID *pDevice);
	HRESULT 			GetDevice9(PVOID *pDevice);
	HRESULT 			GetDeviceUnsup(PVOID *pDevice);

	bool				Init(PVOID pDecl, CDevice *pDevice);
	bool				Init9(PVOID pDecl);

	inline LPDIRECT3DVERTEXDECLARATION9 GetDecl9() { return m_pDecl9; }

public:
	CVertexDeclaration();
	~CVertexDeclaration();

	virtual ULONG	WINAPI AddRef(void);
	virtual ULONG	WINAPI Release(void);

	virtual HRESULT 	GetDeclaration(VERTEXELEMENT*pData, UINT* pSizeOfData){ return (this->*pGetDeclaration)(pData, pSizeOfData);}
	virtual HRESULT 	GetDevice(PVOID *pDevice){ return (this->*pGetDevice)(pDevice);}
};

class CnVertexDeclaration : public CObject
{
private:
	CVertexDeclaration**		m_pDecls;
	UINT			m_uDevices;
	bool			SkipDevice(UINT uDevice, DWORD dwFlags);

public:
	CnVertexDeclaration();
	~CnVertexDeclaration();
	virtual bool	Init(UINT uDevices);
	virtual bool	GetVDecl(CVertexDeclaration **ppVDecl, UINT n);
	virtual bool	SetVDecl(CVertexDeclaration *pVDecl, UINT n);
};

#endif // __CDeviceOBJ_H__


