#ifndef __CSWAPCHAIN_H__
#define __CSWAPCHAIN_H__

#include "Framework.h"

class CSurface;

class CSwapChain : public CObject
{
    LPDIRECT3DSWAPCHAIN9	m_pSwapChain9;
	LPDIRECT3DSWAPCHAIN8	m_pSwapChain8;

	CSurface				**m_pBackBuffers;
	int						m_nBackBuffers;

	CDevice					*m_pDevice;

    HRESULT				(CSwapChain::*pPresent)(RECT *pSrc, RECT *pDst, HWND hDstWndOverride, RGNDATA *pDirtyRegion);
    virtual HRESULT     Present8To9(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion);
    virtual HRESULT     Present8(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion);
    virtual HRESULT     Present7(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion);
    virtual HRESULT     Present6(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion);

    HRESULT				(CSwapChain::*pPresent2)(RECT *pSrc, RECT *pDst, HWND hDstWndOverride, RGNDATA *pDirtyRegion, DWORD dwFlags);
    virtual HRESULT     Present9(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion, DWORD dwFlags);
    virtual HRESULT     Present9To8(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion, DWORD dwFlags);
    virtual HRESULT     Present9To7(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion, DWORD dwFlags);
    virtual HRESULT     Present9To6(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion, DWORD dwFlags);

	HRESULT             (CSwapChain::*pGetBackBuffer)(UINT BackBuffer,DWORD Type,CSurface** ppBackBuffer);
    virtual HRESULT     GetBackBuffer9(UINT BackBuffer,DWORD Type,CSurface** ppBackBuffer);
    virtual HRESULT     GetBackBuffer8(UINT BackBuffer,DWORD Type,CSurface** ppBackBuffer);
    virtual HRESULT     GetBackBuffer7(UINT BackBuffer,DWORD Type,CSurface** ppBackBuffer);
    virtual HRESULT     GetBackBuffer6(UINT BackBuffer,DWORD Type,CSurface** ppBackBuffer);

	HRESULT             (CSwapChain::*pGetFrontBuffer)(CSurface* pDestSurface);
    virtual HRESULT     GetFrontBuffer9(CSurface* pDestSurface);
    virtual HRESULT     GetFrontBuffer8(CSurface* pDestSurface);
    virtual HRESULT     GetFrontBuffer7(CSurface* pDestSurface);
    virtual HRESULT     GetFrontBuffer6(CSurface* pDestSurface);

	HRESULT             (CSwapChain::*pGetDisplayMode)(DISPLAYMODE* pMode);
    virtual HRESULT     GetDisplayMode9(DISPLAYMODE* pMode);
    virtual HRESULT     GetDisplayMode8(DISPLAYMODE* pMode);
    virtual HRESULT     GetDisplayMode7(DISPLAYMODE* pMode);
    virtual HRESULT     GetDisplayMode6(DISPLAYMODE* pMode);

	HRESULT             (CSwapChain::*pGetRasterStatus)(RASTER_STATUS* pRasterStatus);
    virtual HRESULT     GetRasterStatus9(RASTER_STATUS* pRasterStatus);
    virtual HRESULT     GetRasterStatus8(RASTER_STATUS* pRasterStatus);
    virtual HRESULT     GetRasterStatus7(RASTER_STATUS* pRasterStatus);
    virtual HRESULT     GetRasterStatus6(RASTER_STATUS* pRasterStatus);

	void				ReleaseAll();

    void     			ReleaseD3D(){ (this->*pReleaseD3D)();}
    void             	(CSwapChain::*pReleaseD3D)();
	void				ReleaseD3D9();
	void				ReleaseD3D8();
    void				ReleaseD3D7();
    void				ReleaseD3D6();
    
protected:
	virtual void*	GetInternalPointer(void);

public:

    CSwapChain();
    ~CSwapChain();

    virtual ULONG   WINAPI AddRef(void);
    virtual ULONG   WINAPI Release(void);

    virtual bool Init(IDirectDrawSurface4 **pSurface, PRESENT_PARAMETERS *pParams, CDevice *pDevice);
    virtual bool Init(IDirectDrawSurface7 **pSurface, PRESENT_PARAMETERS *pParams, CDevice *pDevice);
	virtual bool Init(IDirect3DSwapChain8* pSurface, PRESENT_PARAMETERS *pParams, CDevice *pDevice);
    virtual bool Init(IDirect3DSwapChain9* pSurface, PRESENT_PARAMETERS *pParams, CDevice *pDevice);

	virtual HRESULT     Present(RECT *pSrc, RECT *pDst, HWND hDstWndOverride, RGNDATA *pDirtyRegion)
						   { return (this->*pPresent)(pSrc, pDst, hDstWndOverride, pDirtyRegion);}
    virtual HRESULT     Present(RECT *pSrc, RECT *pDst, HWND hDstWndOverride, RGNDATA *pDirtyRegion, DWORD dwFlags)
						   { return (this->*pPresent2)(pSrc, pDst, hDstWndOverride, pDirtyRegion, dwFlags);}
	virtual HRESULT     GetBackBuffer(UINT BackBuffer,DWORD Type,CSurface** ppBackBuffer) 
							{ return (this->*pGetBackBuffer)(BackBuffer,Type,ppBackBuffer); }
	inline	HRESULT		GetFrontBufferData(CSurface* pDestSurface) { return GetFrontBuffer(pDestSurface); }
	virtual HRESULT     GetFrontBuffer(CSurface* pDestSurface) 
							{ return (this->*pGetFrontBuffer)(pDestSurface); }
	virtual HRESULT     GetDisplayMode(DISPLAYMODE* pMode) 
							{ return (this->*pGetDisplayMode)(pMode); }
	virtual HRESULT     GetRasterStatus(RASTER_STATUS* pRasterStatus) 
							{ return (this->*pGetRasterStatus)(pRasterStatus); }
	virtual HRESULT		GetBackBufferCached(UINT BackBuffer, DWORD Type, CSurface** ppBackBuffer);
};

// -------------------------------------------------------------------------------------


//
// CnSwapChain - used to tell framework to handle lists of swapchains into wrapper calls
//
class CnSwapChain : public CObject
{
private:
	CSwapChain** m_pSwapChains;
	UINT		 m_uDevices;
public:
	~CnSwapChain();
	virtual bool Init(UINT uDevices);
	virtual bool GetSwapChain(CSwapChain **ppSwapChain, UINT n);
	virtual bool SetSwapChain(CSwapChain *pSwapChain, UINT n);
    virtual bool Present(RECT *pSrc, RECT **pDst, HWND hDstWndOverride, RGNDATA **pDirtyRegion);
    virtual bool Present(RECT *pSrc, RECT **pDst, HWND hDstWndOverride, RGNDATA **pDirtyRegion, DWORD dwFlags);
	virtual bool GetBackBuffer(UINT BackBuffer,DWORD Type,CnSurface** ppBackBuffer);
	virtual bool GetBackBufferCached(UINT BackBuffer, DWORD Type, CnSurface** ppBackBuffer);
    virtual bool GetFrontBuffer(CnSurface* pDestSurface);
    virtual bool GetDisplayMode(DISPLAYMODE* pMode);
    virtual bool GetRasterStatus(RASTER_STATUS* pRasterStatus);
};

//
// can be used by cd3dtest descendants to cycle through each swapchain in a list of swapchains
//
#define BeginSwapChainMap(lpNSwapChain, lpSwapChain) \
	for( UINT swapchain_index_i=0; swapchain_index_i<m_uDeviceList; swapchain_index_i++ ) \
	{ \
		if( lpNSwapChain->GetSwapChain( &lpSwapChain, swapchain_index_i  ) )\
		{

#define EndSwapChainMap( lpNSwapChain, lpSwapChain) \
		} \
		RELEASE( lpSwapChain ); \
	}
 
#endif

