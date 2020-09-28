#ifndef __CTEXTURE_H__
#define __CTEXTURE_H__

#include "Framework.h"
#include "CSurface.h"

class CBaseTexture : public CObject
{
	friend class CDevice;
	friend class CnD3DXSprite;
	friend class CApiTest;

protected:
	RESOURCETYPE 			m_ResourceType;
	FORMAT					m_Format;
    CBaseTexture		   *m_pSysTexture;
    DWORD					m_dwLevelCount;
	CDevice				   *m_pDevice;
	bool					m_bPalettized;

    LPDIRECT3DBASETEXTURE9  m_pTexture9;
    LPDIRECT3DBASETEXTURE8  m_pTexture8;
    LPDIRECTDRAWSURFACE7	m_pTexture7;
	LPDIRECTDRAWPALETTE		m_pPalette;
    LPDIRECTDRAWSURFACE4	m_pTexture6;

    inline virtual LPDIRECT3DBASETEXTURE9	GetD3DTexture9() { return m_pTexture9; }
    inline virtual LPDIRECT3DBASETEXTURE8	GetD3DTexture8() { return m_pTexture8; }
	inline virtual LPDIRECTDRAWSURFACE7		GetD3DTexture7() { return m_pTexture7; }
    inline virtual LPDIRECTDRAWSURFACE4		GetD3DTexture6() { return m_pTexture6; }
	virtual void*							GetInternalPointer(void);

    virtual bool BltSupported(FORMAT *pFmt, DWORD Pool);

public:
	CBaseTexture();

    inline virtual RESOURCETYPE 			GetType() { return m_ResourceType;}
	inline virtual void						SetSysTexture(CBaseTexture *pTex) { m_pSysTexture = pTex; }
    inline virtual CBaseTexture*			GetSysTexture() { return m_pSysTexture; }
    inline virtual bool						IsPalettized() { return m_bPalettized; }
	inline virtual DWORD					GetLevelCount() { return m_dwLevelCount; }
    inline virtual CDevice* 				GetDevice() { return m_pDevice; }
    virtual HRESULT GetDevice(CDevice** ppDevice) { if(ppDevice == NULL )return E_INVALIDARG; *ppDevice=m_pDevice; return S_OK; }
};

class CTexture : public CBaseTexture
{
    friend class CDevice;
	friend class CEffect;
protected:
    CSurface**	m_pSurfaces;		// DX6/DX7 only

	DWORD (CTexture::*pGetLOD)();
    DWORD GetLOD9();
    DWORD GetLOD8();
    DWORD GetLOD7();
    DWORD GetLODUnsup();

	DWORD (CTexture::*pSetLOD)(DWORD dwLODNew);
    DWORD SetLOD9(DWORD dwLODNew);
    DWORD SetLOD8(DWORD dwLODNew);
    DWORD SetLOD7(DWORD dwLODNew);
    DWORD SetLODUnsup(DWORD dwLODNew);

	void  (CTexture::*pPreLoad)();
	void  PreLoadUnsup();
    void  PreLoad7();
	void  PreLoad8();
    void  PreLoad9();


	DWORD (CTexture::*pSetPriority)(DWORD dwNewPriority);
	DWORD SetPriority9(DWORD dwNewPriority);
	DWORD SetPriority8(DWORD dwNewPriority);
	DWORD SetPriority7(DWORD dwNewPriority);
	DWORD SetPriorityUnsup(DWORD dwNewPriority);

	DWORD (CTexture::*pGetPriority)(void);
    DWORD GetPriority9(void);
	DWORD GetPriority8(void);
	DWORD GetPriority7(void);
	DWORD GetPriorityUnsup(void);

	void ReleaseD3D() { (this->*pReleaseD3D)(); }
    void (CTexture::*pReleaseD3D)();
    void ReleaseD3D9();
	void ReleaseD3D8();
	void ReleaseD3D7();
    void ReleaseD3D6();

	HRESULT (CTexture::*pAddDirtyRect)(CONST RECT* pDirtyRect);
    HRESULT AddDirtyRect9(CONST RECT* pDirtyRect);
	HRESULT AddDirtyRect8(CONST RECT* pDirtyRect);
    HRESULT AddDirtyRect7(CONST RECT* pDirtyRect);
    HRESULT AddDirtyRect6(CONST RECT* pDirtyRect);

	HRESULT (CTexture::*pSetPalette)(PALETTEENTRY* pPal);
    HRESULT SetPalette9(PALETTEENTRY *pPal);
	HRESULT SetPalette8(PALETTEENTRY *pPal);
    HRESULT SetPalette7(PALETTEENTRY *pPal);
    HRESULT SetPalette6(PALETTEENTRY *pPal);

	HRESULT (CTexture::*pSetDDrawPalette)(LPDIRECTDRAWPALETTE pPal);
    HRESULT SetDDrawPaletteUnsup(LPDIRECTDRAWPALETTE pPal);
    HRESULT SetDDrawPalette7(LPDIRECTDRAWPALETTE pPal);
    HRESULT SetDDrawPalette6(LPDIRECTDRAWPALETTE pPal);

	HRESULT (CTexture::*pGetDDrawPalette)(LPDIRECTDRAWPALETTE *ppPal);
	HRESULT GetDDrawPaletteUnsup(LPDIRECTDRAWPALETTE *ppPal);
	HRESULT GetDDrawPalette7(LPDIRECTDRAWPALETTE *ppPal);
	HRESULT GetDDrawPalette6(LPDIRECTDRAWPALETTE *ppPal);

    HRESULT (CTexture::*pSetColorKey)(DWORD dwColorKeyFlags, DWORD *pdwColorKey);
    HRESULT SetColorKeyUnsup(DWORD dwColorKeyFlags, DWORD *pdwColorKey);
    HRESULT SetColorKey7(DWORD dwColorKeyFlags, DWORD *pdwColorKey);
    HRESULT SetColorKey6(DWORD dwColorKeyFlags, DWORD *pdwColorKey);

	HRESULT (CTexture::*pLockRect)(UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect,DWORD dwFlags);
    HRESULT LockRect9(UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect,DWORD dwFlags);
	HRESULT LockRect8(UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect,DWORD dwFlags);
	HRESULT LockRectN(UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect,DWORD dwFlags);

	HRESULT (CTexture::*pUnlockRect)(UINT uLevel);
    HRESULT UnlockRect9(UINT uLevel);
	HRESULT UnlockRect8(UINT uLevel);
	HRESULT UnlockRectN(UINT uLevel);

    HRESULT (CTexture::*pGetLevelDesc)(UINT uLevel, PSURFACEDESC pDesc);
    HRESULT GetLevelDesc9(UINT uLevel, PSURFACEDESC pDesc);
	HRESULT GetLevelDesc8(UINT uLevel, PSURFACEDESC pDesc);
	HRESULT GetLevelDescN(UINT uLevel, PSURFACEDESC pDesc);

    HRESULT (CTexture::*pGetSurfaceLevel)(UINT uLevel, CSurface** pSurf);
    HRESULT GetSurfaceLevel9(UINT uLevel, CSurface** pSurf);
	HRESULT GetSurfaceLevel8(UINT uLevel, CSurface** pSurf);
	HRESULT GetSurfaceLevelN(UINT uLevel, CSurface** pSurf);

	HRESULT (CTexture::*pGetDevice)(CDevice** ppDevice);
    HRESULT GetDeviceN(CDevice** ppDevice);
    HRESULT GetDevice9(CDevice** ppDevice);
	HRESULT GetDevice8(CDevice** ppDevice);

	HRESULT (CTexture::*pFreePrivateData)(REFGUID refguid);
    HRESULT FreePrivateDataN(REFGUID refguid);
    HRESULT FreePrivateData9(REFGUID refguid);
	HRESULT FreePrivateData8(REFGUID refguid);

	HRESULT (CTexture::*pGetPrivateData)(REFGUID refguid, void* pData, DWORD* pSizeOfData);
    HRESULT GetPrivateDataN(REFGUID refguid, void* pData, DWORD* pSizeOfData);
    HRESULT GetPrivateData9(REFGUID refguid, void* pData, DWORD* pSizeOfData);
	HRESULT GetPrivateData8(REFGUID refguid, void* pData, DWORD* pSizeOfData);

	HRESULT (CTexture::*pSetPrivateData)(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);
    HRESULT SetPrivateDataN(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);
    HRESULT SetPrivateData9(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);
	HRESULT SetPrivateData8(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);

	HRESULT (CTexture::*pSetAutoGenFilterType)(DWORD FilterType);
	HRESULT SetAutoGenFilterType9(DWORD FilterType);
	HRESULT SetAutoGenFilterTypeUnsup(DWORD FilterType);

	DWORD (CTexture::*pGetAutoGenFilterType)();
	DWORD GetAutoGenFilterType9();
	DWORD GetAutoGenFilterTypeUnsup();

	void (CTexture::*pGenerateMipSubLevels)();
	void GenerateMipSubLevels9();
	void GenerateMipSubLevelsUnsup();

	bool Init9(LPVOID pTexture);
    bool Init8(LPVOID pTexture);
    bool Init7(LPVOID pTexture);
    bool Init6(LPVOID pTexture);

    static HRESULT WINAPI  EnumAttached7(LPDIRECTDRAWSURFACE7 pSurf, LPDDSURFACEDESC2 pDesc, LPVOID pContext);
    static HRESULT WINAPI  EnumAttached6(LPDIRECTDRAWSURFACE4 pSurf, LPDDSURFACEDESC2 pDesc, LPVOID pContext);

    virtual HRESULT UpdateTexture(UINT uLevel, CImage *pImage, DWORD dwTexLoad);
    virtual HRESULT UpdateTexture(UINT uLevel, CImage **pImage, DWORD dwTexLoad);
    virtual HRESULT CopyRects(UINT uLevel, CTexture *pSrcTexture);
    virtual HRESULT LockCopy(UINT uLevel, CTexture* pSrcTexture);
    virtual HRESULT UpdateSurface(UINT uLevel, CTexture* pSrcTexture);

public:

    CTexture();
    ~CTexture();

    virtual ULONG   WINAPI AddRef(void);
    virtual ULONG   WINAPI Release(void);
	
    //Initialize the texture version
	bool Init(LPVOID pD3DTexture, CDevice *pDevice);

	//Version Independent interface
	HRESULT FreePrivateData(REFGUID refguid) { return (this->*pFreePrivateData)(refguid); }
	HRESULT GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) { return (this->*pGetPrivateData)(refguid,pData,pSizeOfData); }
	HRESULT SetPrivateData(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags) { return (this->*pSetPrivateData)(refguid,pData,dwSizeOfData,dwFlags); }
    HRESULT GetDevice(CDevice** ppDevice) { return (this->*pGetDevice)(ppDevice); }

	DWORD   GetLevelCount() { return m_dwLevelCount; }
	void    PreLoad() {(this->*pPreLoad)();}
	DWORD   GetLOD() { return (this->*pGetLOD)(); }
	DWORD   SetLOD(DWORD dwLODNew) { return (this->*pSetLOD)(dwLODNew); }	
	DWORD   SetPriority(DWORD dwNewPriority){ return (this->*pSetPriority)(dwNewPriority); } 
	DWORD   GetPriority(void){ return (this->*pGetPriority)(); } 
	HRESULT SetAutoGenFilterType(DWORD FilterType) {return (this->*pSetAutoGenFilterType)(FilterType);}
	DWORD	GetAutoGenFilterType() {return (this->*pGetAutoGenFilterType)();}
	void	GenerateMipSubLevels() {(this->*pGenerateMipSubLevels)();}
	HRESULT AddDirtyRect(CONST RECT* pDirtyRect) { return (this->*pAddDirtyRect)(pDirtyRect); }
	HRESULT GetLevelDesc(UINT uLevel, PSURFACEDESC pDesc){ return (this->*pGetLevelDesc)(uLevel,pDesc); } 
	HRESULT GetSurfaceLevel(UINT uLevel, CSurface** ppSurfaceLevel){ return (this->*pGetSurfaceLevel)(uLevel,ppSurfaceLevel); } 
	HRESULT LockRect(UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect,DWORD dwFlags) { return (this->*pLockRect)(uLevel,pLockedRect,pRect,dwFlags); } 
	HRESULT UnlockRect(UINT uLevel) { return (this->*pUnlockRect)(uLevel); }
	HRESULT SetPalette(PALETTEENTRY *pPal) { return (this->*pSetPalette)(pPal); }
	HRESULT SetDDrawPalette(LPDIRECTDRAWPALETTE pPal) { return (this->*pSetDDrawPalette)(pPal); }
	HRESULT GetDDrawPalette(LPDIRECTDRAWPALETTE *ppPal) { return (this->*pGetDDrawPalette)(ppPal); }
	HRESULT SetColorKey(DWORD dwColorKeyFlags, DWORD *pdwColorKey) { return (this->*pSetColorKey)(dwColorKeyFlags,pdwColorKey); }

        //Helpful texture functions
        virtual HRESULT LoadImage(UINT uLevel, CImage **pImg, DWORD dwTexLoad);
        virtual HRESULT LoadImage(UINT uLevel, CImage *pImg, DWORD dwTexLoad);
        virtual HRESULT LoadTexture(UINT uLevel, LPCTSTR pFilename, float fAlphaOverride, DWORD dwTexLoad);
        virtual HRESULT LoadStripes(UINT uLevel, DWORD dwW, DWORD dwH, DWORD dwStripes, DWORD dwColorArray[], bool  bHorizontal, bool  bDiagonal, DWORD dwTexLoad);
        virtual HRESULT LoadGradient(UINT uLevel, DWORD dwW,DWORD dwH,DWORD dwColors[4], DWORD dwTexLoad);
        virtual HRESULT LoadTextureFromTexture(UINT uLevel, CTexture *pTex);

        // filter must be one of FILTER_NONE, FILTER_SRGB,
        // FILTER_SRGB_IN or FILTER_SRGB_OUT.
        virtual HRESULT LoadTextureFromTexture(UINT uLevel, CTexture *pTex, DWORD filter);
};

//-------------------------------------------------------------------------------------
//

class CCubeTexture : public CBaseTexture
{
    friend class CDevice;
	friend class CEffect;
private:
	CSurface	  ***m_pSurfaces;	// DX7 only

	//Version dependent interfaces
	DWORD (CCubeTexture::*pGetLOD)();
	DWORD GetLOD7();
	DWORD GetLOD8();
    DWORD GetLOD9();

	DWORD (CCubeTexture::*pSetLOD)(DWORD dwLODNew);
	DWORD SetLOD7(DWORD dwLODNew);
	DWORD SetLOD8(DWORD dwLODNew);
    DWORD SetLOD9(DWORD dwLODNew);

	DWORD (CCubeTexture::*pSetPriority)(DWORD dwNewPriority);
    DWORD SetPriority9(DWORD dwNewPriority);
	DWORD SetPriority8(DWORD dwNewPriority);
	DWORD SetPriority7(DWORD dwNewPriority);

	void  (CCubeTexture::*pPreLoad)();
	void  PreLoad7();
	void  PreLoad8();
    void  PreLoad9();

	DWORD (CCubeTexture::*pGetPriority)(void);
    DWORD GetPriority9(void);
	DWORD GetPriority8(void);
	DWORD GetPriority7(void);

	bool Init7(LPVOID pTexture);
	bool Init8(LPVOID pTexture);
    bool Init9(LPVOID pTexture);

	void ReleaseD3D() { (this->*pReleaseD3D)(); }
    void (CCubeTexture::*pReleaseD3D)();
    void ReleaseD3D9();
	void ReleaseD3D8();
	void ReleaseD3D7();

    HRESULT (CCubeTexture::*pSetPalette)(PALETTEENTRY* pPal);
    HRESULT SetPalette9(PALETTEENTRY *pPal);
    HRESULT SetPalette8(PALETTEENTRY *pPal);
    HRESULT SetPalette7(PALETTEENTRY *pPal);

	HRESULT (CCubeTexture::*pGetCubeMapSurface)(CUBEMAP_FACES FaceType, UINT uLevel, CSurface** ppCubeMapSurface);
	HRESULT GetCubeMapSurfaceN(CUBEMAP_FACES FaceType, UINT uLevel, CSurface** ppCubeMapSurface);
	HRESULT GetCubeMapSurface9(CUBEMAP_FACES FaceType, UINT uLevel, CSurface** ppCubeMapSurface);
    HRESULT GetCubeMapSurface8(CUBEMAP_FACES FaceType, UINT uLevel, CSurface** ppCubeMapSurface);

    HRESULT (CCubeTexture::*pGetLevelDesc)(UINT Level, PSURFACEDESC pDesc);
	HRESULT GetLevelDescN(UINT Level, PSURFACEDESC pDesc);
    HRESULT GetLevelDesc9(UINT Level, PSURFACEDESC pDesc);
	HRESULT GetLevelDesc8(UINT Level, PSURFACEDESC pDesc);

    HRESULT (CCubeTexture::*pAddDirtyRect)(CUBEMAP_FACES FaceType, CONST RECT* pDirtyRect);
	HRESULT AddDirtyRect7(CUBEMAP_FACES FaceType, CONST RECT* pDirtyRect);
	HRESULT AddDirtyRect8(CUBEMAP_FACES FaceType, CONST RECT* pDirtyRect);
    HRESULT AddDirtyRect9(CUBEMAP_FACES FaceType, CONST RECT* pDirtyRect);

    HRESULT (CCubeTexture::*pLockRect)(CUBEMAP_FACES FaceType, UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD dwFlags);
	HRESULT LockRectN(CUBEMAP_FACES FaceType, UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD dwFlags);
    HRESULT LockRect9(CUBEMAP_FACES FaceType, UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD dwFlags);
	HRESULT LockRect8(CUBEMAP_FACES FaceType, UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD dwFlags);

    HRESULT (CCubeTexture::*pUnlockRect)(CUBEMAP_FACES FaceType, UINT uLevel);
	HRESULT UnlockRectN(CUBEMAP_FACES FaceType, UINT uLevel);
    HRESULT UnlockRect9(CUBEMAP_FACES FaceType, UINT uLevel);
	HRESULT UnlockRect8(CUBEMAP_FACES FaceType, UINT uLevel);

	HRESULT (CCubeTexture::*pGetDevice)(CDevice** ppDevice);
    HRESULT GetDeviceN(CDevice** ppDevice);
    HRESULT GetDevice9(CDevice** ppDevice);
	HRESULT GetDevice8(CDevice** ppDevice);

	HRESULT (CCubeTexture::*pFreePrivateData)(REFGUID refguid);
    HRESULT FreePrivateDataN(REFGUID refguid);
    HRESULT FreePrivateData9(REFGUID refguid);
	HRESULT FreePrivateData8(REFGUID refguid);

	HRESULT (CCubeTexture::*pGetPrivateData)(REFGUID refguid, void* pData, DWORD* pSizeOfData);
    HRESULT GetPrivateDataN(REFGUID refguid, void* pData, DWORD* pSizeOfData);
    HRESULT GetPrivateData9(REFGUID refguid, void* pData, DWORD* pSizeOfData);
	HRESULT GetPrivateData8(REFGUID refguid, void* pData, DWORD* pSizeOfData);

	HRESULT (CCubeTexture::*pSetPrivateData)(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);
    HRESULT SetPrivateDataN(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);
    HRESULT SetPrivateData9(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);
	HRESULT SetPrivateData8(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);

	HRESULT (CCubeTexture::*pSetAutoGenFilterType)(DWORD FilterType);
	HRESULT SetAutoGenFilterType9(DWORD FilterType);
	HRESULT SetAutoGenFilterTypeUnsup(DWORD FilterType);

	DWORD (CCubeTexture::*pGetAutoGenFilterType)();
	DWORD GetAutoGenFilterType9();
	DWORD GetAutoGenFilterTypeUnsup();

	void (CCubeTexture::*pGenerateMipSubLevels)();
	void GenerateMipSubLevels9();
	void GenerateMipSubLevelsUnsup();

    static HRESULT WINAPI EnumAttachedCube7(LPDIRECTDRAWSURFACE7 pSurf, LPDDSURFACEDESC2 pDesc, LPVOID pContext);

    virtual HRESULT UpdateTexture(UINT uLevel, CImage **pImage, DWORD dwTexLoad);
    virtual HRESULT UpdateTexture(CImage ***pImage, DWORD dwTexLoad);
    virtual HRESULT UpdateTexture(UINT uLevel, CUBEMAP_FACES Face, CImage *pImg, DWORD dwTexLoad);
    virtual HRESULT CopyRects(CCubeTexture *pSrcTexture);
    virtual HRESULT LockCopy(CCubeTexture *pSrcTexture);
    virtual HRESULT UpdateSurface(CCubeTexture *pSrcTexture);

protected:
    virtual bool 	DescToFace(LPDDSURFACEDESC2 pDesc, CUBEMAP_FACES &Face);

public:

	CCubeTexture();
    ~CCubeTexture();

    virtual ULONG   WINAPI AddRef(void);
    virtual ULONG   WINAPI Release(void);

	//Initialize the texture version
	bool Init(LPVOID pD3DTexture, CDevice *pDevice);

	//Version Independent interface
	HRESULT FreePrivateData(REFGUID refguid) { return (this->*pFreePrivateData)(refguid); }
	HRESULT GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) { return (this->*pGetPrivateData)(refguid,pData,pSizeOfData); }
	HRESULT SetPrivateData(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags) { return (this->*pSetPrivateData)(refguid,pData,dwSizeOfData,dwFlags); }
    virtual HRESULT GetDevice(CDevice** ppDevice) { return (this->*pGetDevice)(ppDevice); }

	DWORD   GetLevelCount() { return m_dwLevelCount; }
	void    PreLoad() {(this->*pPreLoad)();}
	DWORD   GetLOD() { return (this->*pGetLOD)(); }
	DWORD   SetLOD(DWORD dwLODNew) { return (this->*pSetLOD)(dwLODNew); }	
    DWORD   SetPriority(DWORD dwNewPriority){ return (this->*pSetPriority)(dwNewPriority); } 
	DWORD   GetPriority(void){ return (this->*pGetPriority)(); } 
	HRESULT SetAutoGenFilterType(DWORD FilterType) {return (this->*pSetAutoGenFilterType)(FilterType);}
	DWORD	GetAutoGenFilterType() {return (this->*pGetAutoGenFilterType)();}
	void	GenerateMipSubLevels() {(this->*pGenerateMipSubLevels)();}
	HRESULT AddDirtyRect(CUBEMAP_FACES FaceType, CONST RECT* pDirtyRect) { return (this->*pAddDirtyRect)(FaceType, pDirtyRect); }
	HRESULT GetCubeMapSurface(CUBEMAP_FACES FaceType, UINT uLevel, CSurface** ppCubeMapSurface) { return (this->*pGetCubeMapSurface)(FaceType, uLevel, ppCubeMapSurface); }
	HRESULT GetLevelDesc(UINT Level, PSURFACEDESC pDesc) { return (this->*pGetLevelDesc)(Level, pDesc); }
	HRESULT LockRect(CUBEMAP_FACES FaceType, UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD dwFlags)
		{ return (this->*pLockRect)(FaceType, uLevel, pLockedRect, pRect, dwFlags); }
	HRESULT UnlockRect(CUBEMAP_FACES FaceType, UINT uLevel) { return (this->*pUnlockRect)(FaceType, uLevel); }
	HRESULT SetPalette(PALETTEENTRY *pPal) { return (this->*pSetPalette)(pPal); }

	// Utility functions
    virtual HRESULT LoadImage(UINT uLevel, CImage **pImg, DWORD dwTexLoad);\
    virtual HRESULT LoadImage(CImage ***pImg, DWORD dwTexLoad);
	virtual HRESULT LoadTexture(UINT uLevel, CUBEMAP_FACES Face, LPCTSTR pFilename, float fAlphaOverride, DWORD dwTexLoad);
	virtual HRESULT LoadStripes(UINT uLevel, CUBEMAP_FACES Face, DWORD dwW, DWORD dwH, DWORD dwStripes, DWORD dwColorArray[], bool  bHorizontal, bool  bDiagonal, DWORD dwTexLoad);
	virtual HRESULT	LoadGradient(UINT uLevel, CUBEMAP_FACES Face, DWORD dwW,DWORD dwH,DWORD dwColors[4], DWORD dwTexLoad);

};

//-------------------------------------------------------------------------------------
//

class CVolTexture : public CBaseTexture
{
    friend class CDevice;
	friend class CEffect;
private:

    bool Init9(PVOID pTex);
	bool Init8(PVOID pTex);

	void ReleaseD3D(){ if(pReleaseD3D)(this->*pReleaseD3D)(); }
    void (CVolTexture::*pReleaseD3D)();
	void ReleaseD3D9();
    void ReleaseD3D8();

	// private dx wrappers
	DWORD (CVolTexture::*pSetPriority)(DWORD PriorityNew);
    DWORD SetPriority9(DWORD PriorityNew);
	DWORD SetPriority8(DWORD PriorityNew);
	
	DWORD (CVolTexture::*pGetPriority)();
    DWORD GetPriority9();
	DWORD GetPriority8();
	
	void  (CVolTexture::*pPreLoad)();
    void  PreLoad9();
	void  PreLoad8();
	
	DWORD (CVolTexture::*pSetLOD)(DWORD LODNew);
    DWORD SetLOD9(DWORD LODNew);
	DWORD SetLOD8(DWORD LODNew);
	
	DWORD (CVolTexture::*pGetLOD)();
    DWORD GetLOD9();
	DWORD GetLOD8();
	
	HRESULT (CVolTexture::*pGetLevelDesc)(UINT Level, VOLUME_DESC *pDesc);
    HRESULT GetLevelDesc9(UINT Level, VOLUME_DESC *pDesc);
	HRESULT GetLevelDesc8(UINT Level, VOLUME_DESC *pDesc);
	
	HRESULT (CVolTexture::*pGetVolumeLevel)(UINT Level, LPVOLUME* ppVolumeLevel);
    HRESULT GetVolumeLevel9(UINT Level, LPVOLUME* ppVolumeLevel);
	HRESULT GetVolumeLevel8(UINT Level, LPVOLUME* ppVolumeLevel);
	
	HRESULT (CVolTexture::*pLockBox)(UINT Level, LOCKED_BOX* pLockedVolume, CONST BOX* pBox, DWORD Flags);
    HRESULT LockBox9(UINT Level, LOCKED_BOX* pLockedVolume, CONST BOX* pBox, DWORD Flags);
	HRESULT LockBox8(UINT Level, LOCKED_BOX* pLockedVolume, CONST BOX* pBox, DWORD Flags);
	
	HRESULT (CVolTexture::*pUnlockBox)(UINT Level);
    HRESULT UnlockBox9(UINT Level);
	HRESULT UnlockBox8(UINT Level);
	
	HRESULT (CVolTexture::*pAddDirtyBox)(CONST BOX* pDirtyBox);
    HRESULT AddDirtyBox9(CONST BOX* pDirtyBox);
	HRESULT AddDirtyBox8(CONST BOX* pDirtyBox);

    HRESULT (CVolTexture::*pSetPalette)(PALETTEENTRY* pPal);
    HRESULT SetPalette9(PALETTEENTRY *pPal);
    HRESULT SetPalette8(PALETTEENTRY *pPal);

	HRESULT (CVolTexture::*pGetDevice)(CDevice** ppDevice);
    HRESULT GetDevice9(CDevice** ppDevice);
	HRESULT GetDevice8(CDevice** ppDevice);

	HRESULT (CVolTexture::*pFreePrivateData)(REFGUID refguid);
    HRESULT FreePrivateData9(REFGUID refguid);
	HRESULT FreePrivateData8(REFGUID refguid);

	HRESULT (CVolTexture::*pGetPrivateData)(REFGUID refguid, void* pData, DWORD* pSizeOfData);
    HRESULT GetPrivateData9(REFGUID refguid, void* pData, DWORD* pSizeOfData);
	HRESULT GetPrivateData8(REFGUID refguid, void* pData, DWORD* pSizeOfData);

	HRESULT (CVolTexture::*pSetPrivateData)(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);
    HRESULT SetPrivateData9(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);
	HRESULT SetPrivateData8(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags);

	HRESULT (CVolTexture::*pSetAutoGenFilterType)(DWORD FilterType);
	HRESULT SetAutoGenFilterType9(DWORD FilterType);
	HRESULT SetAutoGenFilterTypeUnsup(DWORD FilterType);

	DWORD (CVolTexture::*pGetAutoGenFilterType)();
	DWORD GetAutoGenFilterType9();
	DWORD GetAutoGenFilterTypeUnsup();

	void (CVolTexture::*pGenerateMipSubLevels)();
	void GenerateMipSubLevels9();
	void GenerateMipSubLevelsUnsup();

protected:
    virtual HRESULT UpdateVolume(UINT uLevel, UINT uSlice, CImage *pImg, DWORD dwTexLoad);
    virtual HRESULT UpdateVolume(UINT uLevel, CImage **pImg, DWORD dwTexLoad);
    virtual HRESULT UpdateVolume(CImage ***pImg, DWORD dwTexLoad);
    virtual HRESULT LockCopy(CVolTexture* pSrcTex);

public:
	CVolTexture();
	~CVolTexture();

    virtual ULONG   WINAPI AddRef(void);
    virtual ULONG   WINAPI Release(void);

	bool Init(LPVOID pD3DTexture, CDevice *pDevice);

	//Real Dx8 interfaces
	virtual HRESULT FreePrivateData(REFGUID refguid) { return (this->*pFreePrivateData)(refguid); }
	virtual HRESULT GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) { return (this->*pGetPrivateData)(refguid,pData,pSizeOfData); }
	virtual HRESULT SetPrivateData(REFGUID refguid, void* pData, DWORD dwSizeOfData, DWORD dwFlags) { return (this->*pSetPrivateData)(refguid,pData,dwSizeOfData,dwFlags); }
    virtual HRESULT GetDevice(CDevice** ppDevice) { return (this->*pGetDevice)(ppDevice); }

    virtual DWORD   SetPriority(DWORD PriorityNew) {return (this->*pSetPriority)(PriorityNew);}
	virtual DWORD   GetPriority() {return (this->*pGetPriority)();}
	virtual void    PreLoad() {(this->*pPreLoad)();}
	virtual DWORD   SetLOD(DWORD LODNew) {return (this->*pSetLOD)(LODNew);}
	virtual DWORD   GetLOD() {return (this->*pGetLOD)();}
	virtual DWORD   GetLevelCount() {return m_dwLevelCount;}
	virtual HRESULT SetAutoGenFilterType(DWORD FilterType) {return (this->*pSetAutoGenFilterType)(FilterType);}
	virtual DWORD	GetAutoGenFilterType() {return (this->*pGetAutoGenFilterType)();}
	virtual void	GenerateMipSubLevels() {(this->*pGenerateMipSubLevels)();}
	virtual HRESULT GetLevelDesc(UINT Level, VOLUME_DESC *pDesc) {return (this->*pGetLevelDesc)(Level,pDesc);}
	virtual HRESULT GetVolumeLevel(UINT Level, LPVOLUME* ppVolumeLevel) {return (this->*pGetVolumeLevel)(Level,ppVolumeLevel);}
	virtual HRESULT LockBox(UINT Level, LOCKED_BOX* pLockedVolume, CONST BOX* pBox, DWORD Flags) {return (this->*pLockBox)(Level,pLockedVolume,pBox,Flags);}
	virtual HRESULT UnlockBox(UINT Level) {return (this->*pUnlockBox)(Level);}
	virtual HRESULT AddDirtyBox(CONST BOX* pDirtyBox) {return (this->*pAddDirtyBox)(pDirtyBox);}
    virtual inline bool	IsPalettized() { return m_bPalettized; }
    virtual HRESULT SetPalette(PALETTEENTRY *pPal) { return (this->*pSetPalette)(pPal); }

	// Utility functions
    virtual HRESULT LoadImage(UINT uLevel, CImage **pImg, DWORD dwTexLoad);
    virtual HRESULT LoadImage(CImage ***pImg, DWORD dwTexLoad);
	virtual HRESULT LoadTexture(UINT uLevel, UINT uSlice, LPCTSTR pFilename, float fAlphaOverride, DWORD dwTexLoad);
	virtual HRESULT LoadStripes(UINT uLevel, UINT uSlice, DWORD dwW, DWORD dwH, DWORD dwStripes, DWORD dwColorArray[], bool  bHorizontal, bool  bDiagonal, DWORD dwTexLoad);
	virtual HRESULT	LoadGradient(UINT uLevel, UINT uSlice, DWORD dwW,DWORD dwH,DWORD dwColors[4], DWORD dwTexLoad);
};

// -------------------------------------------------------------------------------------

class CnBaseTexture : public CObject
{
protected:
	RESOURCETYPE	m_ResourceType;
	CTexture**		m_pTextures;
	CVolTexture**	m_pVolTextures;
	CCubeTexture**	m_pCubeTextures;
	LPLOCK_LIST*	m_pHeadLock;
	UINT			m_uHeadLockSize;
	UINT			m_uDevices;
	bool			SkipDevice(UINT uDevice, DWORD dwFlags);
	LOCK_LIST**		GetHead(UINT uLevel);
public:
    CnBaseTexture();
	~CnBaseTexture();
    inline	RESOURCETYPE	GetType() { return m_ResourceType;}
	bool	GetBaseTexture(CBaseTexture **ppTexture, UINT n);
	bool	SetBaseTexture(CBaseTexture *pTexture, UINT n);
	bool	Init(UINT uDevices);

	bool	SetAutoGenFilterType(DWORD FilterType, DWORD dwFlags=CD3D_ALL);
	DWORD	GetAutoGenFilterType(DWORD dwFlags=CD3D_ALL);
	void	GenerateMipSubLevels(DWORD dwFlags=CD3D_ALL);
};

// -------------------------------------------------------------------------------------

class CnTexture : public CnBaseTexture
{
public:
	CnTexture()		{m_ResourceType = RTYPE_TEXTURE;}
	~CnTexture();
	virtual bool	GetTexture(CTexture **ppTexture, UINT n);
	virtual bool	SetTexture(CTexture *pTexture, UINT n);
	
	virtual DWORD   GetLevelCount(DWORD dwFlags=CD3D_ALL);
	virtual DWORD   GetLOD(DWORD dwFlags=CD3D_ALL);
	virtual DWORD   SetLOD(DWORD dwLODNew, DWORD dwFlags=CD3D_ALL);
	virtual	void    PreLoad(DWORD dwFlags=CD3D_ALL);
	virtual DWORD   GetPriority(DWORD dwFlags=CD3D_ALL);
	virtual DWORD   SetPriority(DWORD dwNewPriority, DWORD dwFlags=CD3D_ALL);
	virtual bool    AddDirtyRect(CONST RECT* pDirtyRect, DWORD dwFlags=CD3D_ALL);
	virtual bool    GetLevelDesc(UINT uLevel, PSURFACEDESC pDesc, DWORD dwFlags=CD3D_SRC);
	virtual bool    GetSurfaceLevel(UINT uLevel, LPSURFACES* ppSurfaceLevel, DWORD dwFlags=CD3D_ALL);
	virtual bool    LockRect(UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect,DWORD Flags, DWORD dwFlags=CD3D_ALL);
	virtual bool    UnlockRect(UINT uLevel, DWORD dwFlags=CD3D_ALL);
	virtual bool 	IsPalettized(DWORD dwFlags=CD3D_ALL);
    virtual	bool 	SetPalette(PALETTEENTRY *pPal, DWORD dwFlags = CD3D_ALL);
    virtual	bool 	SetDDrawPalette(LPDIRECTDRAWPALETTE pPal, DWORD dwFlags = CD3D_ALL);
    virtual	bool 	SetColorKey(DWORD dwColorKeyFlags, DWORD *pdwColorKey, DWORD dwFlags = CD3D_ALL);

    // For convienience
	virtual bool    LoadTexture(UINT uLevel, LPCTSTR pFilename, float fAlphaOverride=1.f, DWORD dwTexLoad=TEXLOAD_DEFAULT, DWORD dwFlags=CD3D_ALL);
	virtual bool    LoadStripes(UINT uLevel, DWORD dwW, DWORD dwH, DWORD dwStripes, DWORD dwColorArray[], bool  bHorizontal, bool  bDiagonal=false, DWORD dwTexLoad=TEXLOAD_DEFAULT, DWORD dwFlags=CD3D_ALL);
	virtual bool    LoadGradient(UINT uLevel, DWORD dwW,DWORD dwH,DWORD dwColors[4], DWORD dwTexLoad=TEXLOAD_DEFAULT, DWORD dwFlags=CD3D_ALL);
    virtual bool	LoadImage(UINT uLevel, CImage **pImg, DWORD dwTexLoad=TEXLOAD_DEFAULT, DWORD dwFlags=CD3D_ALL);
    virtual bool	LoadImage(UINT uLevel, CImage *pImg, DWORD dwTexLoad=TEXLOAD_DEFAULT, DWORD dwFlags=CD3D_ALL);
};

// -------------------------------------------------------------------------------------

class CnCubeTexture : public CnBaseTexture
{
private:
public:
	CnCubeTexture()	{m_ResourceType = RTYPE_CUBETEXTURE;}
	~CnCubeTexture();
	virtual bool	GetCubeTexture(CCubeTexture **ppCubeTexture, UINT n);
	virtual bool	SetCubeTexture(CCubeTexture *pCubeTexture, UINT n);

	virtual DWORD   GetLevelCount(DWORD dwFlags=CD3D_ALL);
	virtual DWORD   GetLOD(DWORD dwFlags=CD3D_ALL);
	virtual DWORD   SetLOD(DWORD dwLODNew, DWORD dwFlags=CD3D_ALL);
	virtual	void    PreLoad(DWORD dwFlags=CD3D_ALL);
	virtual DWORD   GetPriority(DWORD dwFlags=CD3D_ALL);
	virtual DWORD   SetPriority(DWORD dwNewPriority, DWORD dwFlags=CD3D_ALL);
	virtual bool    AddDirtyRect(CUBEMAP_FACES FaceType, CONST RECT* pDirtyRect, DWORD dwFlags=CD3D_ALL);
	virtual bool    GetLevelDesc(UINT uLevel, PSURFACEDESC pDesc, DWORD dwFlags=CD3D_SRC);
	virtual bool    GetCubeMapSurface(CUBEMAP_FACES FaceType, UINT uLevel, LPSURFACES* ppSurfaceLevel, DWORD dwFlags=CD3D_ALL);
	virtual bool    LockRect(CUBEMAP_FACES FaceType, UINT uLevel, LOCKED_RECT* pLockedRect, CONST RECT* pRect,DWORD Flags, DWORD dwFlags=CD3D_ALL);
	virtual bool    UnlockRect(CUBEMAP_FACES FaceType, UINT uLevel, DWORD dwFlags=CD3D_ALL);
	virtual bool 	IsPalettized(DWORD dwFlags=CD3D_ALL);
    virtual	bool 	SetPalette(PALETTEENTRY *pPal, DWORD dwFlags = CD3D_ALL);

    virtual bool	LoadImage(UINT uLevel, CImage **pImg, DWORD dwTexLoad=TEXLOAD_DEFAULT, DWORD dwFlags=CD3D_ALL);
    virtual bool	LoadImage(CImage ***pImg, DWORD dwTexLoad=TEXLOAD_DEFAULT, DWORD dwFlags=CD3D_ALL);
    virtual bool	LoadTexture(UINT uLevel, CUBEMAP_FACES Face, LPCTSTR pFilename, float fAlphaOverride=1.f, DWORD dwTexLoad=TEXLOAD_DEFAULT, DWORD dwFlags=CD3D_ALL);
	virtual bool	LoadStripes(UINT uLevel, CUBEMAP_FACES Face, DWORD dwW, DWORD dwH, DWORD dwStripes, DWORD dwColorArray[], bool  bHorizontal, bool  bDiagonal=false, DWORD dwTexLoad=TEXLOAD_DEFAULT, DWORD dwFlags=CD3D_ALL);
	virtual bool	LoadGradient(UINT uLevel, CUBEMAP_FACES Face, DWORD dwW,DWORD dwH,DWORD dwColors[4], DWORD dwTexLoad=TEXLOAD_DEFAULT, DWORD dwFlags=CD3D_ALL);
};

// -------------------------------------------------------------------------------------

class CnVolTexture : public CnBaseTexture
{
private:
public:
	CnVolTexture()	{m_ResourceType = RTYPE_VOLUMETEXTURE;}
	~CnVolTexture();
	virtual bool	GetVolumeTexture(CVolTexture **ppVolumeTexture, UINT n);
	virtual bool	SetVolumeTexture(CVolTexture *pVolumeTexture, UINT n);

	virtual DWORD   GetLevelCount(DWORD dwFlags=CD3D_ALL);
	virtual DWORD   GetLOD(DWORD dwFlags=CD3D_ALL);
	virtual DWORD   SetLOD(DWORD dwLODNew, DWORD dwFlags=CD3D_ALL);
	virtual	void    PreLoad(DWORD dwFlags=CD3D_ALL);
	virtual DWORD   GetPriority(DWORD dwFlags=CD3D_ALL);
	virtual DWORD   SetPriority(DWORD dwNewPriority, DWORD dwFlags=CD3D_ALL);
	virtual bool    AddDirtyBox(CONST BOX* pDirtyBox, DWORD dwFlags=CD3D_ALL);
	virtual bool    GetLevelDesc(UINT uLevel, LPVOLUME_DESC pDesc, DWORD dwFlags=CD3D_SRC);
	virtual bool    GetVolumeLevel(UINT uLevel, LPVOLUME* ppVolume, DWORD dwFlags=CD3D_ALL);
	virtual bool    LockBox(UINT uLevel, LOCKED_BOX* pLockedBox, CONST BOX* pBox,DWORD Flags, DWORD dwFlags=CD3D_ALL);
	virtual bool    UnlockBox(UINT uLevel, DWORD dwFlags=CD3D_ALL);
	virtual bool 	IsPalettized(DWORD dwFlags=CD3D_ALL);
    virtual	bool 	SetPalette(PALETTEENTRY *pPal, DWORD dwFlags = CD3D_ALL);

    virtual bool	LoadImage(UINT uLevel, CImage **pImg, DWORD dwTexLoad=TEXLOAD_DEFAULT, DWORD dwFlags=CD3D_ALL);
    virtual bool	LoadImage(CImage ***pImg, DWORD dwTexLoad=TEXLOAD_DEFAULT, DWORD dwFlags=CD3D_ALL);
    virtual bool	LoadTexture(UINT uLevel, UINT uSlice, LPCTSTR pFilename, float fAlphaOverride=1.f, DWORD dwTexLoad=TEXLOAD_DEFAULT, DWORD dwFlags=CD3D_ALL);
	virtual bool	LoadStripes(UINT uLevel, UINT uSlice, DWORD dwW, DWORD dwH, DWORD dwStripes, DWORD dwColorArray[], bool  bHorizontal, bool  bDiagonal=false, DWORD dwTexLoad=TEXLOAD_DEFAULT, DWORD dwFlags=CD3D_ALL);
	virtual bool	LoadGradient(UINT uLevel, UINT uSlice, DWORD dwW,DWORD dwH,DWORD dwColors[4], DWORD dwTexLoad=TEXLOAD_DEFAULT, DWORD dwFlags=CD3D_ALL);
};

// -------------------------------------------------------------------------------------

//
// can be used by cd3dtest descendants to cycle through each texture in a list of textures
// (provided mostly for readability of test code)
/**************************************************************************************
    Example Client Code

    LPTEXTURES pTextures;
	GetTexture(&pTextures);
	
	LPTEXTURE pTex;
	BeginTextureMap( pTextures, pTex );
	
	pTex->LockRect( 0, pTex, etc etc );
	if( memcmp(pLockedRectData, pExpectedData, nScanlineSize) )
		bFail = TRUE;

	EndTextureMap( pTextures, pTex );

**************************************************************************************/
#define BeginTextureMap(lpNTexture, lpTexture) \
	for( UINT texture_index_i=0; texture_index_i<m_uDeviceList; texture_index_i++ ) \
	{ \
		if( lpNTexture->GetTexture( &lpTexture, surface_index_i  ) )\
		{

#define EndTextureMap( lpNTexture, lpTexture) \
		} \
		RELEASE( lpTexture ); \
	}

// -------------------------------------------------------------------------------------

#endif
