// ======================================================================================
//
//  CD3D.h
//
//  Header file for the CD3D class.
//
//  This is a wrapper for all versions of the D3D interface.
//
//  Copyright (C) 2000 Microsoft Corporation
//
// ======================================================================================

#ifndef __CD3D_H__
#define __CD3D_H__

#include "Framework.h"
#include "DXErr9.h"

// --------------------------------------------------------------------------------------
DRIVERMODEL DetectDriverModel(void);
bool LDDMApertureCorruption(void);

bool DetectDXVersion
(
	__out_opt float *pfDXBuildNumber,
	__out_opt float *pfDXVersion
);

HRESULT GetD3DXBuildNumber(float fDXVersion, float &fD3DXVersion);

#define FLOAT_EQUAL(x, y) fabs(x - y) < 0.00001f

typedef void        (__cdecl *LPDEBUGSETMUTE)(BOOL);

class CD3D : public CObject
{
	friend class CDevice;
	friend class CSurface;
	friend class CTexture;
	friend class CCubeTexture;
	friend class CApiTest;

	// DX9 level objects
	//
	// We have potentially two different D3D objects, though usually only one.  This provides
	// the ability to test two different D3D9.DLL against each other if needed.
	union
	{
		LPDIRECT3D9             m_pSrcD3D9;
		LPDIRECT3D9EX           m_pSrcD3D9Ex;
	};

	union
	{
		LPDIRECT3D9             m_pRefD3D9;
		LPDIRECT3D9EX           m_pRefD3D9Ex;
	};

	// DX8 level objects
	LPDIRECT3D8             m_pD3D8;

	// DX7 level objects
	LPDIRECT3D7             m_pD3D7;
	LPDIRECTDRAW7           m_pDDraw7;
	CSurface               *m_pPrimary;

	// DX6 level objects
	LPDIRECTDRAW4           m_pDDraw4;
	LPDIRECT3D3             m_pD3D3;

	// D3D info
	DCAPS                   m_ddCaps;                               // DDraw caps
	HINSTANCE               m_hSrcD3DDLL;                           // Handle to the Src D3D dll
	HINSTANCE               m_hRefD3DDLL;                           // Handle to the Ref D3D dll
	HINSTANCE               m_hDebugD3DDLL;                         // Handle to the Debug D3D dll
	float                   m_fVersion;                             // DX version number
	float                   m_fBuild;                               // DX build number
	bool                    m_bDebug;                               // Is this a debug version of D3D
	bool                    m_bPSGP;                                // Is PSGP enabled
	float                   m_fMinDXVersion;                        // Minimum version of DX supported
	float                   m_fMaxDXVersion;                        // Maximum version of DX supported
	HINSTANCE               m_hSWRast;                              // Handle for SW rasterizer
	TCHAR*                  m_sSWRast;                              // DLL name for SW rasterizer
	TCHAR*                  m_sSWRastEntry;                         // Name of the SW rasterizer entry pt
	TCHAR                   *m_sSrcDLL, *m_sRefDLL, *m_sDebugDLL;   // DLL names/locations for the runtime
	bool                    m_bUserSelectedSWRast;                  // The flag indicates that SW rasterizer was selected by user
	DRIVERMODEL             m_DriverModel;                          // Display driver model

	// Enumerated D3D info
	PADAPTER                m_pAdapters;
	int                     m_nAdapters;

	// Misc app vars
	CD3DTestLog             *m_pLog;                            // Ptr to the testlog
	TCHAR                   *m_tcsLastError[MAX_D3D_STRING];    // Buffer to store D3D errors
	HRESULT                 m_hrLastError;                      // Stores last error
	bool                    m_bDeviceLost;                      // DEVICELOST reported by D3D
	bool                    m_bOutofMemory;                     // OutOfMemory reported by D3D
	bool                    m_bDriverInternalError;             // DRIVERINTERNALERROR has been returned
	bool                    m_bReferenceDetected;               // Reference detected for current version
	TCHAR                   *m_sReferenceDLL;                   // Refence DLL string(d3dref8.dll, etc)
	UINT                    m_nDefaultDisplyModesAlloc;         // Number of display modes to allocate during initialization

	LPDEBUGSETMUTE          m_pDebugMute;                       // Function ptr to DebugSetMute(BOOL)

protected:

	// Private D3D functions
	bool    GetPSGPFromReg();
	HRESULT InitFunctionPtrs(float fVersion);
	void    UpdateDLLInfo();
	bool    UnRegisterRasterizer();
	bool    GetDebugInfoFromReg();
	bool    IsDisplayModeSupported(PADAPTER pAdapter, UINT nWidth, UINT nHeight, FMT fDisplayFmt);

	// Private DX9 functions
	bool    RegisterRasterizer9(__in LPCTSTR pcsDLL, __in LPCTSTR pcsEntry);

	// Private DX8 functions
	bool    RegisterRasterizer8(__in LPCTSTR pcsDLL, __in LPCTSTR pcsEntry);


	// Private DX7 functions
	static  BOOL    WINAPI  DirectDrawEnumCallbackEx7(__in LPGUID, __in LPSTR, __in LPSTR, __in PVOID, HMONITOR);
	static  HRESULT WINAPI  EnumDisplayModesCallback7(LPDDSURFACEDESC2,PVOID);
	static  HRESULT WINAPI  EnumD3DDevices7(__in LPTSTR szDesc, __in LPTSTR szDevice, __in LPD3DDEVICEDESC7 pDev, __in LPVOID pContext);
	static  HRESULT WINAPI  EnumAdapterInfo7(__in LPTSTR szDesc, __in LPTSTR szDevice, __in LPD3DDEVICEDESC7 pDev, __in LPVOID pContext);
	static  HRESULT WINAPI  EnumZBufferFormats7(LPDDPIXELFORMAT pddpf, LPVOID pContext);
	static  HRESULT WINAPI  EnumTextureFormats7(LPDDPIXELFORMAT pddpf, LPVOID pContext);
	virtual HRESULT         SetupD3D7(CDevice* pDevice, PRESENT_PARAMETERS *pParams);
	virtual bool            SetupScreen7(HWND hWnd, PRESENT_PARAMETERS *pParams);
	virtual HRESULT         CreatePrimary7(CDevice* pDevice, PRESENT_PARAMETERS *pParams);
	static  HRESULT         DDPFToD3DFmt(LPDDPIXELFORMAT  pDdPixFmt, FMT* pFmt, BOOL* pbIsDepth);

	// Private DX6 functions
	static  BOOL    WINAPI DirectDrawEnumCallbackEx6(__in LPGUID, __in LPSTR, __in LPSTR, __in PVOID, HMONITOR);
	static  HRESULT WINAPI EnumD3DDevices6(__in LPGUID pGuid, __in LPTSTR szDesc, __in LPTSTR szDevice, __in LPD3DDEVICEDESC pHW, __in LPD3DDEVICEDESC pSW, __in LPVOID pContext);
	static  HRESULT WINAPI EnumDisplayModesCallback6(LPDDSURFACEDESC2,PVOID);
	static  HRESULT WINAPI EnumZBufferFormats6(LPDDPIXELFORMAT pddpf, LPVOID pContext);
	static  HRESULT WINAPI EnumTextureFormats6(LPDDPIXELFORMAT pddpf, LPVOID pContext);
	virtual HRESULT         SetupD3D6(CDevice* pDevice, PRESENT_PARAMETERS *pParams);
	virtual bool            SetupScreen6(HWND hWnd, PRESENT_PARAMETERS *pParams);
	virtual HRESULT         CreatePrimary6(CDevice* pDevice, PRESENT_PARAMETERS *pParams);

	// Versioning wrappers
	virtual HRESULT     InitD3D() { return (this->*pInitD3D)(); }
	HRESULT                (CD3D::*pInitD3D)();
	virtual HRESULT     InitD3D9();
	virtual HRESULT     InitD3D8();
	virtual HRESULT     InitD3D7();
	virtual HRESULT     InitD3D6();

public:

	CD3D();
	~CD3D();

	bool m_bD3DDebugHelper;

	// General D3D functions
	virtual bool    Initialize();                               // Inits to the highest available
	virtual HRESULT SetupD3D(float fVersion);                   // Inits a specific version
	virtual void    SetSWRast(__in LPCTSTR pcsDll, __in LPCTSTR pcsEntry);  // Set the SW rast attributes
	virtual void    SetRuntimeDLLName(__in LPCTSTR sSrcDLL, __in LPCTSTR sRefDLL); // Set the runtime DLL filenames
	void    ReleaseAll();                                       // Release all objects

	inline LPCTSTR  HResultToString(HRESULT hr) const;
	inline float    GetMinVersion() { return m_fMinDXVersion; }
	inline float    GetMaxVersion() { return m_fMaxDXVersion; }
	inline DCAPS*   GetDDCaps() { return &m_ddCaps; }
	inline float    GetVersion() { return m_fVersion; }         // Returns the interface version not including "dot" releases(8.0, 9.0, etc.)
	float           GetRuntimeVersion();                        // Returns the runtime version, excluding SDK, including "dot" releases
	inline float    GetBuild() { return m_fBuild; }
	inline bool     GetDebug() { return m_bDebug; }
	inline bool     GetPSGP() { return m_bPSGP; }
	inline TCHAR*   GetSWRast() { return m_sSWRast; }
	PADAPTER        GetAdapter(int nAdapter);
	virtual HRESULT GetLastError();
	virtual HRESULT SetLastError(HRESULT hr);
	inline bool     IsDeviceLost() { return m_bDeviceLost; }
	inline bool     IsOutofMemory() { return m_bOutofMemory; }
	inline bool     IsDriverInternalError() { return m_bDriverInternalError; }
	inline void     SetDriverInternalError(bool bValue) { m_bDriverInternalError = bValue; }
	inline void     SetDeviceLost(bool bState) { m_bDeviceLost = bState; }
	inline bool     IsRefInstalled() { return m_bReferenceDetected; }
	inline DRIVERMODEL GetDriverModel() { return m_DriverModel; }
	virtual bool    IsStretchSupported();
	inline TCHAR*   GetRefDLL() { return m_sReferenceDLL; }
	virtual bool    UpdateAdapterInfo();
	virtual void    SetDebugMute( bool bMute );

	virtual HRESULT     CreateDevice(UINT Adapter, DWORD DevType, HWND hWnd, DWORD BehFlags, PRESENT_PARAMETERS* pParams, CDevice **pDevice);

	// Public versioning Wrappers
	virtual void        ReleaseD3D() { if(pReleaseD3D)(this->*pReleaseD3D)(); }
	void                (CD3D::*pReleaseD3D)();
	virtual void        ReleaseD3D9();
	virtual void        ReleaseD3D8();
	virtual void        ReleaseD3D7();
	virtual void        ReleaseD3D6();

	virtual bool        DetectCaps(CDevice* pDevice) { return (this->*pDetectCaps)(pDevice); }
	bool                (CD3D::*pDetectCaps)(CDevice *pDevice);
	virtual bool        DetectCaps9(CDevice *pDevice);
	virtual bool        DetectCaps8(CDevice *pDevice);
	virtual bool        DetectCaps7(CDevice *pDevice);
	virtual bool        DetectCaps6(CDevice *pDevice);

	virtual bool        ResolveBehaviors(IN UINT Adapter, IN DWORD DevType, IN DWORD Behaviors, __out DWORD * pResolvedBehaviors)  { return (this->*pResolveBehaviors)(Adapter, DevType, Behaviors, pResolvedBehaviors); };
	bool                (CD3D::*pResolveBehaviors)(IN UINT Adapter, IN DWORD DevType, IN DWORD Behaviors, __out DWORD * pResolvedBehaviors);
	virtual bool        ResolveBehaviors9(IN UINT Adapter, IN DWORD DevType, IN DWORD Behaviors, __out DWORD * pResolvedBehaviors);
	virtual bool        ResolveBehaviors8(IN UINT Adapter, IN DWORD DevType, IN DWORD Behaviors, __out DWORD * pResolvedBehaviors);
	virtual bool        ResolveBehaviors7(IN UINT Adapter, IN DWORD DevType, IN DWORD Behaviors, __out DWORD * pResolvedBehaviors);
	virtual bool        ResolveBehaviors6(IN UINT Adapter, IN DWORD DevType, IN DWORD Behaviors, __out DWORD * pResolvedBehaviors);

	virtual void        ProcessError(HRESULT hr) { (this->*pProcessError)(hr); }
	void                (CD3D::*pProcessError)(HRESULT hr);
	virtual void        ProcessError9(HRESULT hr);
	virtual void        ProcessError8(HRESULT hr);
	virtual void        ProcessError7(HRESULT hr);

	virtual bool        CompareFormats(FORMAT *pSrcFmt, FORMAT *pRefFmt) { return (this->*pCompareFormats)(pSrcFmt, pRefFmt); }
	bool                (CD3D::*pCompareFormats)(FORMAT *pSrcFmt, FORMAT *peRefFmt);
	virtual bool        CompareFormats9(FORMAT *pSrcFmt, FORMAT *pRefFmt);
	virtual bool        CompareFormats8(FORMAT *pSrcFmt, FORMAT *pRefFmt);
	virtual bool        CompareFormats7(FORMAT *pSrcFmt, FORMAT *pRefFmt);

	virtual HRESULT     EvictManagedTextures() { return (this->*pEvictManagedTextures)(); }
	HRESULT             (CD3D::*pEvictManagedTextures)();
	virtual HRESULT     EvictManagedTexturesUnsup();
	virtual HRESULT     EvictManagedTextures7();
	virtual HRESULT     EvictManagedTextures6();

	virtual HRESULT     CreateDDrawPalette(DWORD dwFlags, LPPALETTEENTRY lpDDColorArray, LPDIRECTDRAWPALETTE *lplpDDPalette)
							{ return (this->*pCreateDDrawPalette)(dwFlags, lpDDColorArray, lplpDDPalette); }
	HRESULT             (CD3D::*pCreateDDrawPalette)(DWORD dwFlags, LPPALETTEENTRY lpDDColorArray, LPDIRECTDRAWPALETTE *lplpDDPalette);
	virtual HRESULT     CreateDDrawPaletteUnsup(DWORD dwFlags, LPPALETTEENTRY lpDDColorArray, LPDIRECTDRAWPALETTE *lplpDDPalette);
	virtual HRESULT     CreateDDrawPalette7(DWORD dwFlags, LPPALETTEENTRY lpDDColorArray, LPDIRECTDRAWPALETTE *lplpDDPalette);
	virtual HRESULT     CreateDDrawPalette6(DWORD dwFlags, LPPALETTEENTRY lpDDColorArray, LPDIRECTDRAWPALETTE *lplpDDPalette);

	// D3D9 object wrappers
	virtual HRESULT     RegisterSoftwareDevice9(void* pInitializeFunction);
	virtual UINT        GetAdapterCount9(void);
	virtual HRESULT     GetAdapterIdentifier9(UINT Adapter,DWORD Flags,ADAPTER_IDENTIFIER* pIdentifier);
	virtual HRESULT     EnumAdapterModes8To9(UINT Adapter,UINT Mode,DISPLAYMODE* pMode);
	virtual HRESULT     EnumAdapterModes9(UINT Adapter, FORMAT Format, UINT Mode,DISPLAYMODE* pMode);
	virtual HRESULT		EnumAdapterModesEx9( UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode );
	virtual HRESULT     GetAdapterDisplayMode9(UINT Adapter,DISPLAYMODE* pMode);
	virtual HRESULT     CheckDeviceType9(UINT Adapter,DWORD CheckType,FORMAT DisplayFormat,FORMAT BackBufferFormat,BOOL Windowed);
	virtual HRESULT     CheckDeviceFormat9(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,DWORD Usage,RESOURCETYPE RType,FORMAT CheckFormat);
	virtual HRESULT     CheckDeviceMultiSampleType9To8(UINT Adapter,DWORD DeviceType,FORMAT SurfaceFormat,BOOL Windowed,MULTISAMPLE_TYPE MultiSampleType, DWORD *dwQualityLevels);
	virtual HRESULT     CheckDeviceMultiSampleType8To9(UINT Adapter,DWORD DeviceType,FORMAT SurfaceFormat,BOOL Windowed,MULTISAMPLE_TYPE MultiSampleType);
	virtual HRESULT     CheckDeviceMultiSampleType9(UINT Adapter,DWORD DeviceType,FORMAT SurfaceFormat,BOOL Windowed,MULTISAMPLE_TYPE MultiSampleType, DWORD *dwQualityLevels);
	virtual HRESULT     CheckDepthStencilMatch9(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,FORMAT RenderTargetFormat,FORMAT DepthStencilFormat);
	virtual HRESULT     GetDeviceCaps9(UINT Adapter,DWORD DeviceType,CAPS* pCaps);
	virtual HMONITOR    GetAdapterMonitor9(UINT Adapter);
	virtual HRESULT     CheckDeviceFormatConversion9(UINT Adapter,DWORD DeviceType,FORMAT SourceFormat,FORMAT TargetFormat);
	virtual UINT		GetAdapterModeCountEx9( UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter );


	// D3D8 object wrappers
	virtual HRESULT     RegisterSoftwareDevice8(void* pInitializeFunction);
	virtual UINT        GetAdapterCount8(void);
	virtual HRESULT     GetAdapterIdentifier8(UINT Adapter,DWORD Flags,ADAPTER_IDENTIFIER* pIdentifier);
	virtual UINT        GetAdapterModeCount8(UINT Adapter);
	virtual HRESULT     EnumAdapterModes8(UINT Adapter,UINT Mode,DISPLAYMODE* pMode);
	virtual HRESULT     GetAdapterDisplayMode8(UINT Adapter,DISPLAYMODE* pMode);
	virtual HRESULT     CheckDeviceType8(UINT Adapter,DWORD CheckType,FORMAT DisplayFormat,FORMAT BackBufferFormat,BOOL Windowed);
	virtual HRESULT     CheckDeviceFormat8(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,DWORD Usage,RESOURCETYPE RType,FORMAT CheckFormat);
	virtual HRESULT     CheckDeviceMultiSampleType8(UINT Adapter,DWORD DeviceType,FORMAT SurfaceFormat,BOOL Windowed,MULTISAMPLE_TYPE MultiSampleType);
	virtual HRESULT     CheckDepthStencilMatch8(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,FORMAT RenderTargetFormat,FORMAT DepthStencilFormat);
	virtual HRESULT     GetDeviceCaps8(UINT Adapter,DWORD DeviceType,CAPS* pCaps);
	virtual HMONITOR    GetAdapterMonitor8(UINT Adapter);

	// if anyone wants to implement any of these for pre-dx8 interfaces, have at it...
	virtual HRESULT     RegisterSoftwareDeviceUnsup(void* pInitializeFunction);
	virtual UINT        GetAdapterCountUnsup(void);
	virtual HRESULT     GetAdapterIdentifierUnsup(UINT Adapter,DWORD Flags,ADAPTER_IDENTIFIER* pIdentifier);
	virtual UINT        GetAdapterModeCountUnsup(UINT Adapter);
	virtual UINT        GetAdapterModeCountExUnsup( UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter );
	virtual HRESULT     EnumAdapterModesUnsup(UINT Adapter,UINT Mode,DISPLAYMODE* pMode);
	virtual HRESULT     EnumAdapterModesUnsup9(UINT Adapter,FORMAT Format, UINT Mode,DISPLAYMODE* pMode);
	virtual HRESULT     EnumAdapterModesExUnsup( UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode );
	virtual HRESULT     GetAdapterDisplayModeUnsup(UINT Adapter,DISPLAYMODE* pMode);
	virtual HRESULT     CheckDeviceTypeUnsup(UINT Adapter,DWORD CheckType,FORMAT DisplayFormat,FORMAT BackBufferFormat,BOOL Windowed);
	virtual HRESULT     CheckDeviceFormatUnsup(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,DWORD Usage,RESOURCETYPE RType,FORMAT CheckFormat);
	virtual HRESULT     CheckDeviceMultiSampleTypeUnsup(UINT Adapter,DWORD DeviceType,FORMAT SurfaceFormat,BOOL Windowed,MULTISAMPLE_TYPE MultiSampleType);
	virtual HRESULT     CheckDeviceMultiSampleType9Unsup(UINT Adapter,DWORD DeviceType,FORMAT SurfaceFormat,BOOL Windowed,MULTISAMPLE_TYPE MultiSampleType, DWORD *dwQualityLevels);
	virtual HRESULT     CheckDepthStencilMatchUnsup(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,FORMAT RenderTargetFormat,FORMAT DepthStencilFormat);
	virtual HRESULT     GetDeviceCapsUnsup(UINT Adapter,DWORD DeviceType,CAPS* pCaps);
	virtual HMONITOR    GetAdapterMonitorUnsup(UINT Adapter);

	// Version independant wrappers
	virtual HRESULT     RegisterSoftwareDevice(void* pInitializeFunction)
							{ return (this->*pRegisterSoftwareDevice)(pInitializeFunction); }
	HRESULT             (CD3D::*pRegisterSoftwareDevice)(void* pInitializeFunction);

	virtual UINT        GetAdapterCount(void)
							{ return (this->*pGetAdapterCount)(); }
	UINT                (CD3D::*pGetAdapterCount)(void);

	virtual HRESULT     GetAdapterIdentifier(UINT Adapter,DWORD Flags,ADAPTER_IDENTIFIER* pIdentifier)
							{ return (this->*pGetAdapterIdentifier)(Adapter,Flags,pIdentifier); }
	HRESULT             (CD3D::*pGetAdapterIdentifier)(UINT Adapter,DWORD Flags,ADAPTER_IDENTIFIER* pIdentifier);

	virtual UINT        GetAdapterModeCount(UINT Adapter)
							{ return (this->*pGetAdapterModeCount)(Adapter); }
	UINT                (CD3D::*pGetAdapterModeCount)(UINT Adapter);

	virtual UINT        GetAdapterModeCountEx( UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter )
							{ return (this->*pGetAdapterModeCountEx)( Adapter, pFilter ); }
	UINT                (CD3D::*pGetAdapterModeCountEx)( UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter );

	virtual HRESULT     EnumAdapterModes(UINT Adapter,UINT Mode,DISPLAYMODE* pMode)
							{ return (this->*pEnumAdapterModes)(Adapter,Mode,pMode); }
	HRESULT             (CD3D::*pEnumAdapterModes)(UINT Adapter,UINT Mode,DISPLAYMODE* pMode);

	virtual HRESULT     EnumAdapterModes(UINT Adapter, FORMAT Format, UINT Mode,DISPLAYMODE* pMode)
							{ return (this->*pEnumAdapterModes2)(Adapter, Format, Mode,pMode); }
	HRESULT             (CD3D::*pEnumAdapterModes2)(UINT Adapter, FORMAT Format, UINT Mode,DISPLAYMODE* pMode);

	virtual HRESULT     EnumAdapterModesEx( UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode )
							{ return (this->*pEnumAdapterModesEx)( Adapter, pFilter, Mode, pMode ); }
	HRESULT             (CD3D::*pEnumAdapterModesEx)( UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode );

	virtual HRESULT     GetAdapterDisplayMode(UINT Adapter,DISPLAYMODE* pMode)
							{ return (this->*pGetAdapterDisplayMode)(Adapter,pMode); }
	HRESULT             (CD3D::*pGetAdapterDisplayMode)(UINT Adapter,DISPLAYMODE* pMode);

	virtual HRESULT     CheckDeviceType(UINT Adapter,DWORD CheckType,FORMAT DisplayFormat,FORMAT BackBufferFormat,BOOL Windowed)
							{ return (this->*pCheckDeviceType)(Adapter,CheckType,DisplayFormat,BackBufferFormat,Windowed); }
	HRESULT             (CD3D::*pCheckDeviceType)(UINT Adapter,DWORD CheckType,FORMAT DisplayFormat,FORMAT BackBufferFormat,BOOL Windowed);

	virtual HRESULT     CheckDeviceFormat(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,DWORD Usage,RESOURCETYPE RType,FORMAT CheckFormat)
							{ return (this->*pCheckDeviceFormat)(Adapter,DeviceType,AdapterFormat,Usage,RType,CheckFormat); }
	HRESULT             (CD3D::*pCheckDeviceFormat)(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,DWORD Usage,RESOURCETYPE RType,FORMAT CheckFormat);

	virtual HRESULT     CheckDeviceMultiSampleType(UINT Adapter,DWORD DeviceType,FORMAT SurfaceFormat,BOOL Windowed,MULTISAMPLE_TYPE MultiSampleType)
							{ return (this->*pCheckDeviceMultiSampleType)(Adapter,DeviceType,SurfaceFormat,Windowed,MultiSampleType); }
	HRESULT             (CD3D::*pCheckDeviceMultiSampleType)(UINT Adapter,DWORD DeviceType,FORMAT SurfaceFormat,BOOL Windowed,MULTISAMPLE_TYPE MultiSampleType);

	virtual HRESULT     CheckDeviceMultiSampleType(UINT Adapter,DWORD DeviceType,FORMAT SurfaceFormat,BOOL Windowed,MULTISAMPLE_TYPE MultiSampleType, DWORD *dwQualityLevels)
						{ return (this->*pCheckDeviceMultiSampleType9)(Adapter,DeviceType,SurfaceFormat,Windowed,MultiSampleType, dwQualityLevels); }
	HRESULT             (CD3D::*pCheckDeviceMultiSampleType9)(UINT Adapter,DWORD DeviceType,FORMAT SurfaceFormat,BOOL Windowed,MULTISAMPLE_TYPE MultiSampleType, DWORD *dwQualityLevels);

	virtual HRESULT     CheckDepthStencilMatch(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,FORMAT RenderTargetFormat,FORMAT DepthStencilFormat)
							{ return (this->*pCheckDepthStencilMatch)(Adapter,DeviceType,AdapterFormat,RenderTargetFormat,DepthStencilFormat); }
	HRESULT             (CD3D::*pCheckDepthStencilMatch)(UINT Adapter,DWORD DeviceType,FORMAT AdapterFormat,FORMAT RenderTargetFormat,FORMAT DepthStencilFormat);

	virtual HRESULT     GetDeviceCaps(UINT Adapter,DWORD DeviceType,CAPS* pCaps)
							{ return (this->*pGetDeviceCaps)(Adapter,DeviceType,pCaps); }
	HRESULT             (CD3D::*pGetDeviceCaps)(UINT Adapter,DWORD DeviceType,CAPS* pCaps);

	virtual HMONITOR    GetAdapterMonitor(UINT Adapter)
							{ return (this->*pGetAdapterMonitor)(Adapter); }
	HMONITOR            (CD3D::*pGetAdapterMonitor)(UINT Adapter);

	virtual HRESULT     CheckDeviceFormatConversion(UINT Adapter,DWORD DeviceType,FORMAT SourceFormat,FORMAT TargetFormat)
							{ return (this->*pCheckDeviceFormatConversion)(Adapter, DeviceType, SourceFormat, TargetFormat); }
	HRESULT             (CD3D::*pCheckDeviceFormatConversion)(UINT Adapter,DWORD DeviceType,FORMAT SourceFormat,FORMAT TargetFormat);
	virtual HRESULT     CheckDeviceFormatConversionUnsup(UINT Adapter,DWORD DeviceType,FORMAT SourceFormat,FORMAT TargetFormat);


	virtual HRESULT     CreateDevice(GUID Guid, LPDIRECTDRAWSURFACE7 pSurf, LPDIRECT3DDEVICE7 *pDev7);
	virtual HRESULT     CreateDevice(GUID Guid, LPDIRECTDRAWSURFACE4 pSurf, LPDIRECT3DDEVICE3 *pDev3);

	virtual UINT        GetDisplayIDFromAdapterOrdinal( UINT nAdapter );
};


// D3DX wrapper functions
HRESULT InitD3DX(float fVersion);
void    ReleaseD3DX();
extern bool g_bStaticD3DX;

#endif // __CD3D_H__
