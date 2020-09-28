/******************************Module*Header*******************************
* Module Name: LockConfirm.h
*
* Declaration of the test component that will lock, unlock, confirm contents
*
* Author: Marina Meleshenko [marmel]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#ifndef __LOCK_CONFIRM__
#define __LOCK_CONFIRM__

#define ALLOC_COUNT 16

enum eALLOC_TYPE
{
    Texture = 0,
    RenderTarget = 1,
};

typedef struct _SURFACE
{
    DWORD dwFillPattern;
    CI3DSurface2Ptr pSurface;
}SURFACE;

typedef struct _DYNAMIC_TEXTURE : _SURFACE
{
    CI3DTexturePtr pVidMemTexture;
    UINT dwMipCount;
    DWORD dwPitch;
}DYNAMIC_TEXTURE;

typedef struct _PINNED_SURFACE : _SURFACE
{
    CI3DNonMipped2DResourcePtr pPrimary;
}PINNED_SURFACE;

typedef struct _RENDER_TARGET : _SURFACE
{
    CI3DNonMipped2DResourcePtr pRT;	
}RENDER_TARGET;
    


class CLockConfirmTypeMap : public CCoreTypeMap
{
public:
    CLockConfirmTypeMap();
};

/*
    Name: CLockConfirm
    Desc: Test component that will will lock, unlock, confirm contents of allocations
*/
class CLockConfirm : public CGenericI3DComponent
{
public:
    CLockConfirm();

protected:
    bool m_bBVT;
    bool m_bIntegrated;
    eALLOC_TYPE m_Type;
    UINT m_Width;
    UINT m_Height;
    CLockConfirmTypeMap m_TypeMap;

    //
    //	Gets backbuffer numS from swap chain, fills it with color to use it
    //	then fills it with pattern
    //
    HRESULT CreatePrimary(PINNED_SURFACE* pS, UINT numS);

    //
    //	Creates dynamic texture, uses it by blitting to get it allocated in video memory
    //	then fills it with pattern
    //
    HRESULT CreateDynamicTexture(DYNAMIC_TEXTURE* pT, DWORD dwWidth = 0, DWORD dwHeight = 0, DWORD dwMipCount = 1);


    //
    //	Creates render target and uses it by filling it with color to get it allocation in video memory
    //	then fills it with pattern
    //
    HRESULT CreateRenderTarget(CI3DDevicePtr pDevice, RENDER_TARGET* pRT, DWORD dwWidth = 0, DWORD dwHeight = 0, bool bLockable = true);

    //
    //	ResourcePattern will either fill range Start+size with pattern or check that range for pattern
    //
    HRESULT ResourcePattern(UINT size, BYTE* Start, DWORD dwFillPattern, DWORD Mask, BOOL bFill, BOOL bCheck);

    //
    //	LockSurface will lock a surface and check its pattern (if dwOldPattern != 0) or fill it with pattern 
    //	(if dwNewPattern != 0)
    //
    HRESULT LockSurface(CI3DSurface2Ptr pS, DWORD dwOldPattern, DWORD dwNewPattern, DWORD* pPitch = NULL, DWORD dwFlags = 0);

    //
    //	Unlocks surface
    //
    HRESULT UnlockSurface(CI3DSurface2Ptr pS);


    //
    //	Will create and use many allocations causing evictions
    //
    HRESULT CauseEvictions(CI3DDevicePtr pDevice);

    //
    //	Will render quad with texture causing said texture to be paged into video memory
    //
    void RenderTexturedQuad(CI3DTexturePtr pTexture);

    //
    //	Will blit from surface to render target causing said surface to be paged into video memory
    //
    void UseSurface(CI3DDevicePtr pDevice, CI3DSurface2Ptr pSurface);


    void Init();


    //
    //	Will create Count number of allocations of type and dimensions determined by 
    //  current test parameters
    //
    HRESULT CreateAllocations(SURFACE*** ppSurf, UINT Count);

    //
    //	Will destroy allocations previously created by CreateAllocations
    //
    void DestroyAllocations(SURFACE*** ppSurf, UINT Count);


    //
    //	Will check driver caps to determine if non-square non-pow-of-2 textures are supported
    //
    bool CapsCheckForTextureSupport(UINT MipCount = 1);


    //
    //	Will create a new window and new device. This will be useful when rendering needs to be 
    //  done on another thread in parallel with rendering done on main thread.
    //
    void CreateAdditionalDevice(CI3DDevice** ppDevice);


    //
    //	Will set VidMM fault
    //
    HRESULT SetVidMmFault(UINT Fault);
};


class CLockConfirmLockPinned : public CLockConfirm
{
public:
    CLockConfirmLockPinned();

protected:
    bool CapsCheck();
    void RunTest();
    void EndTest();
};


class CLockConfirmLockNonPinned : public CLockConfirm
{
public:
    CLockConfirmLockNonPinned();

protected:
    bool CapsCheck();
    void RunTest();
    void EndTest();
};


class CLockConfirmLockEvict : public CLockConfirm
{
public:
    CLockConfirmLockEvict();

protected:
    bool CapsCheck();
    void RunTest();
    void EndTest();
};



class CLockConfirmEvictUnderLock : public CLockConfirm
{
public: 
    CLockConfirmEvictUnderLock();

protected:
    bool CapsCheck();
    void RunTest();
    void EndTest();
};


class CLockConfirmLockEachMipLevelNoSwizzlingRange : public CLockConfirm
{
public:
    CLockConfirmLockEachMipLevelNoSwizzlingRange();

protected:
    bool CapsCheck();
    void RunTest();
    void EndTest();
};


class CLockConfirmRunOutOfSwizzlingRanges_nonpinned : public CLockConfirm
{
public:
    CLockConfirmRunOutOfSwizzlingRanges_nonpinned();

protected:
    bool CapsCheck();
    void RunTest();
    void EndTest();
};


class CLockConfirmRunOutOfSwizzlingRanges_pinned : public CLockConfirm
{
public:
    CLockConfirmRunOutOfSwizzlingRanges_pinned();

protected:
    bool CapsCheck();
    void RunTest();
    void EndTest();
};


class CLockConfirmLockEachMipLevel : public CLockConfirm
{
public:
    CLockConfirmLockEachMipLevel();

protected:
    bool CapsCheck();
    void RunTest();
    void EndTest();
};

class CLockConfirmLockWithFaults : public CLockConfirm
{
public:
    CLockConfirmLockWithFaults();

protected:
    bool CapsCheck();
    void RunTest();
    void EndTest();	
};


class CLockConfirmEvictWhileAccessed : public CLockConfirm
{
public:
    CLockConfirmEvictWhileAccessed();

protected:
    bool CapsCheck();
    void RunTest();
    void EndTest();

    static DWORD SuspendCPUAccessThreadProc(void* context);

    bool m_bThreadStarted;
};


class CLockConfirmSwizzlingRangeContentionAndPaging : public CLockConfirm
{
public:
    CLockConfirmSwizzlingRangeContentionAndPaging();

protected:
    bool CapsCheck();
    void RunTest();
    void EndTest();

    //
    //	For this test we want natural eviction rather then forcing eviction with SuspendCPUAccess
    //  because in natural eviction paging algorithms will apply and make it so that not all of
    //  our test allocations get evicted right away, rather the older ones will get evicted and newer 
    //  ones will still hold swizzling ranges
    //
    static DWORD EvictionThreadProc(void* context);

    bool m_bThreadRunning;
};


class CLockConfirmLockWhenOutOfMemory : public CLockConfirm
{
public:
    CLockConfirmLockWhenOutOfMemory();

protected:
    bool CapsCheck();
    void RunTest();
    void EndTest();

    void ConsumeVirtualMemory();
    void FreeVirtualMemory();

private:
    static const UINT m_cMaxAllocations = 0x10000;
    void* m_Allocs[m_cMaxAllocations];
    UINT  m_numAllocations; 
};

#endif // __LOCK_CONFIRM__