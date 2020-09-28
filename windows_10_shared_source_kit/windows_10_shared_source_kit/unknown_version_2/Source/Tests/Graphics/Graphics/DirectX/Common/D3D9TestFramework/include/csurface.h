#ifndef _CSURFACE_H_
#define _CSURFACE_H_

#include "Framework.h"
#include "CImage.h"

class CSurface: public CObject
{
	friend class CDevice;
	friend class CTexture;
	friend class CSwapChain;
	friend class CApiTest;

protected:
	IDirectDrawSurface4*	m_pSurface4;
	IDirectDrawSurface7*	m_pSurface7;
	IDirect3DSurface8*		m_pSurface8;
    IDirect3DSurface9*		m_pSurface9;
	void*					m_pParent;
	CDevice*				m_pDevice;

public:

    // Return the d3d versions of the surface
	LPVOID (CSurface::*pGetD3DSurface)();
	inline LPDIRECTDRAWSURFACE4 GetD3DSurface6() { return m_pSurface4; }
    inline LPDIRECTDRAWSURFACE7 GetD3DSurface7() { return m_pSurface7; }
    inline LPDIRECT3DSURFACE8   GetD3DSurface8() { return m_pSurface8; }
    inline LPDIRECT3DSURFACE9   GetD3DSurface9() { return m_pSurface9; }
	
	//Versioning functions
	HRESULT (CSurface::*pGetDevice)(LPDEVICE *ppDevice);
    virtual HRESULT GetDevice6(LPDEVICE *ppDevice);
	virtual HRESULT GetDevice7(LPDEVICE *ppDevice);
	virtual HRESULT GetDevice8(LPDEVICE *ppDevice);
    virtual HRESULT GetDevice9(LPDEVICE *ppDevice);

	HRESULT (CSurface::*pGetDesc)(PSURFACEDESC pDesc);
    virtual HRESULT GetDesc6(PSURFACEDESC pDesc);
	virtual HRESULT GetDesc7(PSURFACEDESC pDesc);
	virtual HRESULT GetDesc8(PSURFACEDESC pDesc);
    virtual HRESULT GetDesc9(PSURFACEDESC pDesc);
	
	HRESULT (CSurface::*pUnlockRect)();
	virtual	HRESULT UnlockRect6();
	virtual	HRESULT UnlockRect7();
	virtual	HRESULT UnlockRect8();
    virtual	HRESULT UnlockRect9();
	
	HRESULT (CSurface::*pGetDC) (HDC *phDC);
	virtual HRESULT GetDC9(HDC *phDC);
	virtual HRESULT GetDC8(HDC *phDC);
	virtual HRESULT GetDC7(HDC *phDC);
	virtual HRESULT GetDC6(HDC *phDC);

	HRESULT (CSurface::*pReleaseDC) (HDC hDC);
	virtual HRESULT ReleaseDC9(HDC hDC);
	virtual HRESULT ReleaseDC8(HDC hDC);
	virtual HRESULT ReleaseDC7(HDC hDC);
	virtual HRESULT ReleaseDC6(HDC hDC);

	bool (CSurface::*pRelSurf)();
    virtual	bool RelSurf6();
	virtual	bool RelSurf7();
	virtual	bool RelSurf8();
    virtual	bool RelSurf9();

	HRESULT (CSurface::*pGetPalette)(PALETTEENTRY rgPal[256], DWORD &dwNumEntries);
    virtual HRESULT GetPalette6(PALETTEENTRY rgPal[256], DWORD &dwNumEntries);
	virtual HRESULT GetPalette7(PALETTEENTRY rgPal[256], DWORD &dwNumEntries);
	virtual HRESULT GetPalette8(PALETTEENTRY rgPal[256], DWORD &dwNumEntries);
    virtual HRESULT GetPalette9(PALETTEENTRY rgPal[256], DWORD &dwNumEntries);

	HRESULT (CSurface::*pLockRect)(LOCKED_RECT *pLockedRect, CONST RECT* pRect, DWORD Flags);
	virtual HRESULT LockRect6(LOCKED_RECT *pLockedRect, CONST RECT* pRect, DWORD Flags);
	virtual HRESULT LockRect7(LOCKED_RECT *pLockedRect, CONST RECT* pRect, DWORD Flags);
	virtual HRESULT LockRect8(LOCKED_RECT *pLockedRect, CONST RECT* pRect, DWORD Flags);
    virtual HRESULT LockRect9(LOCKED_RECT *pLockedRect, CONST RECT* pRect, DWORD Flags);

    HRESULT	(CSurface::*pUpdateSurfaceImg)(CImage* pSrc);
    virtual HRESULT	UpdateSurfaceImg9(CImage* pSrc);
	virtual HRESULT	UpdateSurfaceImg8(CImage* pSrc);
	virtual HRESULT	UpdateSurfaceImg7(CImage* pSrc);
    virtual HRESULT UpdateDXTnImage7(CImage *pSrc);

    HRESULT	(CSurface::*pUpdateSurfaceFromSurface)(CSurface* pSrc);
    virtual HRESULT	UpdateSurfaceFromSurface9(CSurface* pSrc);
	virtual HRESULT	UpdateSurfaceFromSurface8(CSurface* pSrc);
	virtual HRESULT	UpdateSurfaceFromSurface7(CSurface* pSrc);

	HRESULT	(CSurface::*pAddAttachedSurface)(CSurface *pSurf);
    virtual HRESULT	AddAttachedSurface9(CSurface *pSurf);
	virtual HRESULT	AddAttachedSurface8(CSurface *pSurf);
	virtual HRESULT	AddAttachedSurface7(CSurface *pSurf);
    virtual HRESULT	AddAttachedSurface6(CSurface *pSurf);

	HRESULT	(CSurface::*pDeleteAttachedSurface)(CSurface *pSurf);
    virtual HRESULT	DeleteAttachedSurface9(CSurface *pSurf);
	virtual HRESULT	DeleteAttachedSurface8(CSurface *pSurf);
	virtual HRESULT	DeleteAttachedSurface7(CSurface *pSurf);
    virtual HRESULT	DeleteAttachedSurface6(CSurface *pSurf);

    bool    (CSurface::*pColorFill)(DWORD dwRGB, LPRECT pRect);
    virtual bool    ColorFill9(DWORD dwRGB, LPRECT pRect = NULL);
    virtual bool    ColorFill8(DWORD dwRGB, LPRECT pRect = NULL);
    virtual bool    ColorFill7(DWORD dwRGB, LPRECT pRect = NULL);
    virtual bool    ColorFill6(DWORD dwRGB, LPRECT pRect = NULL);

	HRESULT (CSurface::*pGetContainer)(REFIID riid, void** ppContainer);
	virtual HRESULT GetContainer9(REFIID riid, void** ppContainer);
    virtual HRESULT GetContainer8(REFIID riid, void** ppContainer);
	virtual HRESULT GetContainer7(REFIID riid, void** ppContainer);
	virtual HRESULT GetContainer6(REFIID riid, void** ppContainer);

	HRESULT (CSurface::*pFreePrivateData)(REFGUID refguid);
    virtual HRESULT FreePrivateData9(REFGUID refguid);
	virtual HRESULT FreePrivateData8(REFGUID refguid);
	virtual HRESULT FreePrivateData7(REFGUID refguid);
	virtual HRESULT FreePrivateData6(REFGUID refguid);

	HRESULT (CSurface::*pGetPrivateData)(REFGUID refguid, void* pData, DWORD* pSizeOfData);
    virtual HRESULT GetPrivateData9(REFGUID refguid, void* pData, DWORD* pSizeOfData);
	virtual HRESULT GetPrivateData8(REFGUID refguid, void* pData, DWORD* pSizeOfData);
	virtual HRESULT GetPrivateData7(REFGUID refguid, void* pData, DWORD* pSizeOfData);
	virtual HRESULT GetPrivateData6(REFGUID refguid, void* pData, DWORD* pSizeOfData);

	HRESULT (CSurface::*pSetPrivateData)(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);
    virtual HRESULT SetPrivateData9(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);
	virtual HRESULT SetPrivateData8(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);
	virtual HRESULT SetPrivateData7(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);
	virtual HRESULT SetPrivateData6(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);

    virtual HRESULT UpdateSurfaceFromSurface(CSurface *pSurf);

public:
	CSurface();
	~CSurface();	

    virtual ULONG   WINAPI AddRef(void);
    virtual ULONG   WINAPI Release(void);

    SURFACEDESC				m_Desc;
	MASK					m_Mask;

	void ReleaseAll(void);
    virtual bool Init(void* pSurface, CDevice *pDevice, void *pParent);
    virtual bool Init6(void* pSurface);
	virtual bool Init7(void* pSurface);
	virtual bool Init8(void* pSurface);
    virtual bool Init9(void* pSurface);
    virtual HRESULT GetDevice(LPDEVICE *ppDevice) { return (this->*pGetDevice)(ppDevice); }
	virtual void*	GetInternalPointer(void);

	virtual	HRESULT GetDesc(PSURFACEDESC pDesc) { return (this->*pGetDesc)(pDesc); }
	virtual	HRESULT LockRect(LOCKED_RECT *pLockedRect, CONST RECT* pRect, DWORD Flags) { return (this->*pLockRect)(pLockedRect, pRect, Flags); }
	virtual	HRESULT UnlockRect() { return (this->*pUnlockRect)(); }
	virtual	HRESULT GetDC(HDC *phDC) { return (this->*pGetDC)(phDC); }
	virtual	HRESULT ReleaseDC(HDC hDC) { return (this->*pReleaseDC)(hDC); }
    virtual HRESULT	AddAttachedSurface(CSurface *pSurf) { return (this->*pAddAttachedSurface)(pSurf); }
    virtual HRESULT	DeleteAttachedSurface(CSurface *pSurf) { return (this->*pDeleteAttachedSurface)(pSurf); }

	virtual HRESULT GetContainer(REFIID riid, void** ppContainer) { return (this->*pGetContainer)(riid, ppContainer); }
	virtual HRESULT FreePrivateData(REFGUID refguid) { return (this->*pFreePrivateData)(refguid); }
	virtual HRESULT GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)  { return (this->*pGetPrivateData)(refguid, pData, pSizeOfData); }
	virtual HRESULT SetPrivateData(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags) { return (this->*pSetPrivateData)(refguid, pData, dwSizeOfData, dwFlags); }

	//Handy Surface functions
    virtual HRESULT	UpdateSurface(CImage *pSrc) { return (this->*pUpdateSurfaceImg)(pSrc); }
    virtual HRESULT	UpdateSurface(CSurface *pSrc) { return (this->*pUpdateSurfaceFromSurface)(pSrc); }
	virtual HRESULT GetPalette(PALETTEENTRY rgPal[256], DWORD &dwNumEntries) { return (this->*pGetPalette)(rgPal, dwNumEntries); }
	virtual	bool	RelSurf() { return (this->*pRelSurf)(); }
    virtual DWORD   ReadColor(PVOID * ppSurfacePtr, bool fIncrement = true);
    virtual bool	ColorFill(DWORD dwRGB, LPRECT pRect = NULL)  { return (this->*pColorFill)(dwRGB, pRect); }

    virtual DWORD   ColorConvert(DWORD dwSource);
    virtual HRESULT LoadImage(CImage *pImg);
    virtual HRESULT LoadSurfaceFromSurface(CSurface *pSurfFrom);

    // filter must be one of FILTER_NONE, FILTER_SRGB,
    // FILTER_SRGB_IN or FILTER_SRGB_OUT.
    virtual HRESULT LoadSurfaceFromSurface(CSurface *pSurfFrom, DWORD filter);
    inline  CDevice* GetCDevice() { return m_pDevice; };
};


class CVolume : public CObject
{
	friend class CDevice;
	friend class CVolTexture;
	friend class CApiTest;
private:
	IDirect3DVolume8* m_pVolume8;
    IDirect3DVolume9* m_pVolume9;
	
	void *m_pParent;
	CDevice *m_pDevice;
	VOLUME_DESC m_Desc;
	MASK m_Mask;


	bool (CVolume::*pRelVol)();
    virtual	bool RelVol8();
    virtual	bool RelVol9();

protected:
	inline IDirect3DVolume8* GetD3DVolume8() { return m_pVolume8; }
    inline IDirect3DVolume9* GetD3DVolume9() { return m_pVolume9; }

	//Versioning functions
	HRESULT (CVolume::*pGetDevice)(LPDEVICE *ppDevice);
    HRESULT GetDevice8(LPDEVICE *ppDevice);
	HRESULT GetDevice9(LPDEVICE *ppDevice);

	HRESULT (CVolume::*pGetDesc)(LPVOLUME_DESC pDesc);
	HRESULT GetDesc8(LPVOLUME_DESC pDesc);
    HRESULT GetDesc9(LPVOLUME_DESC pDesc);

	HRESULT (CVolume::*pLockBox)(LOCKED_BOX* pLockedVolume, CONST BOX* pBox, DWORD dwFlags);
	HRESULT LockBox8(LOCKED_BOX* pLockedVolume, CONST BOX* pBox, DWORD dwFlags);
    HRESULT LockBox9(LOCKED_BOX* pLockedVolume, CONST BOX* pBox, DWORD dwFlags);

	HRESULT (CVolume::*pUnlockBox)();
	HRESULT UnlockBox8();
    HRESULT UnlockBox9();

	HRESULT (CVolume::*pUpdateVolumeImg)(UINT uSlice, CImage *pImage);
	HRESULT UpdateVolumeImg8(UINT uSlice, CImage *pImage);
    HRESULT UpdateVolumeImg9(UINT uSlice, CImage *pImage);

    HRESULT (CVolume::*pUpdateVolumeImgs)(CImage **pImage);
	HRESULT UpdateVolumeImgs8(CImage **pImage);
    HRESULT UpdateVolumeImgs9(CImage **pImage);

    HRESULT (CVolume::*pUpdateVolumeFromVolume)(CVolume *pVol);
	HRESULT UpdateVolumeFromVolume8(CVolume *pVol);
    HRESULT UpdateVolumeFromVolume9(CVolume *pVol);

	virtual void*	GetInternalPointer(void);
    HRESULT UpdateVolumeFromVolume(CVolume *pVol);
    HRESULT UpdateVolume(CVolume *pVol)  {return (this->*pUpdateVolumeFromVolume)(pVol); };
    HRESULT UpdateVolume(UINT uSlice, CImage *pImage) {return (this->*pUpdateVolumeImg)(uSlice, pImage); };
	HRESULT UpdateVolume(CImage **ppImage) {return (this->*pUpdateVolumeImgs)(ppImage); };
    
	HRESULT (CVolume::*pGetContainer)(REFIID riid, void** ppContainer);
	HRESULT GetContainer9(REFIID riid, void** ppContainer);
    HRESULT GetContainer8(REFIID riid, void** ppContainer);

	HRESULT (CVolume::*pFreePrivateData)(REFGUID refguid);
    HRESULT FreePrivateData9(REFGUID refguid);
	HRESULT FreePrivateData8(REFGUID refguid);

	HRESULT (CVolume::*pGetPrivateData)(REFGUID refguid, void* pData, DWORD* pSizeOfData);
    HRESULT GetPrivateData9(REFGUID refguid, void* pData, DWORD* pSizeOfData);
	HRESULT GetPrivateData8(REFGUID refguid, void* pData, DWORD* pSizeOfData);

	HRESULT (CVolume::*pSetPrivateData)(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);
    HRESULT SetPrivateData9(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);
	HRESULT SetPrivateData8(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);

    bool    Init8(void* pVolume);
    bool    Init9(void* pVolume);
public:

	CVolume::CVolume();
	CVolume::~CVolume();

    bool 	Init(void* pVolume, CDevice *pDevice, void *pParent);
    bool 	RelVol() {return (this->*pRelVol)(); }
 	void    ReleaseAll(void);
    virtual ULONG   WINAPI AddRef(void);
    virtual ULONG   WINAPI Release(void);
	virtual DWORD ColorConvert(DWORD dwColor);
    virtual DWORD   ReadColor(PVOID * ppSurfacePtr, bool fIncrement = true);

    HRESULT GetContainer(REFIID riid, void** ppContainer) { return (this->*pGetContainer)(riid, ppContainer); }
	HRESULT FreePrivateData(REFGUID refguid) { return (this->*pFreePrivateData)(refguid); }
	HRESULT GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) { return (this->*pGetPrivateData)(refguid, pData, pSizeOfData); }
	HRESULT SetPrivateData(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags) { return (this->*pSetPrivateData)(refguid, pData, dwSizeOfData, dwFlags); }

    HRESULT GetDevice(CDevice** ppDevice) { return (this->*pGetDevice)(ppDevice); }
	HRESULT GetDesc(LPVOLUME_DESC pDesc) { return (this->*pGetDesc)(pDesc); }
	HRESULT LockBox(LOCKED_BOX* pLockedBox, CONST BOX* pBox, DWORD dwFlags) { return (this->*pLockBox)(pLockedBox, pBox, dwFlags); }
	HRESULT UnlockBox() { return (this->*pUnlockBox)(); }
	
    HRESULT LoadImage(UINT uSlice, CImage *pImage);
    HRESULT LoadImage(CImage **ppImage);
	inline  CDevice* GetCDevice() { return m_pDevice; };

	// Helpful surface functions
	HRESULT GetImage(UINT uSlice, CImage *pImage);

};

//
// CnSurface - used to tell framework to handle lists of surfaces into wrapper calls
//
class CnSurface : public CObject
{
private:
	CSurface**	m_pSurfaces;
	UINT		m_uDevices;
	//CHandle		m_DCHandles;
public:
	CnSurface()  { m_pSurfaces = NULL; m_uDevices = 0; }
	~CnSurface();
	virtual bool Init(UINT uDevices);
	virtual bool GetSurface(CSurface **ppSurface, UINT n);
	virtual bool SetSurface(CSurface *pSurface, UINT n);
	//virtual bool GetDC(CHandle *pDCHandles);
	//virtual bool ReleaseDC(CHandle *pDCHandles);
};


//
// can be used by cd3dtest descendants to cycle through each surface in a list of surfaces
//
/**************************************************************************************
    Example Client Code

    LPSURFACES pSurfaces;
	GetBackBuffer(&pSurfaces);
	
	LPSURFACE pSurf;
	BeginSurfaceMap( pSurfaces, pSurf );
	
	pSurf->LockRect( pSurf, etc etc );
	if( memcmp(pLockedRectData, pExpectedData, nScanlineSize) )
		bFail = TRUE;

	EndSurfaceMap( pSurfaces, pSurf );

**************************************************************************************/
#define BeginSurfaceMap(lpNSurface, lpSurface) \
	for( UINT surface_index_i=0; surface_index_i<m_uDeviceList; surface_index_i++ ) \
	{ \
		if( lpNSurface->GetSurface( &lpSurface, surface_index_i  ) ) \
		{

#define EndSurfaceMap( lpNSurface, lpSurface) \
		} \
		RELEASE( lpSurface ); \
	}

#endif // _CSURFACE_H_

