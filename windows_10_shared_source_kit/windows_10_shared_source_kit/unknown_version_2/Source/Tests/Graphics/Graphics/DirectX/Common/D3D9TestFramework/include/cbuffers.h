// ======================================================================================
//
//  CBuffers.h
//
//  Header file for the CVertexBuffer and CIndexBuffer classes.
//
//  This is a wrapper for all versions of vertex and index buffer interfaces.
//
//  Copyright (C) 2001 Microsoft Corporation
//
// ======================================================================================

#ifndef __CBuffers_h__
#define __CBuffers_h__

#include "Framework.h"


// --------------------------------------------------------------------------------------

class CVertexBuffer : public CObject
{
protected:

    friend class CDevice;

    // d3d version specific data and accessors
    LPDIRECT3DVERTEXBUFFER		m_pVB6;
    LPDIRECT3DVERTEXBUFFER7     m_pVB7;
    LPDIRECT3DVERTEXBUFFER8     m_pVB8;
    LPDIRECT3DVERTEXBUFFER9     m_pVB9;
    CDevice*                    m_pDevice;
    DWORD                       m_dwVertexFormat;

    // Inialization
    bool        Initialize6(LPVOID pVB);
    bool        Initialize7(LPVOID pVB);
    bool        Initialize8(LPVOID pVB);
    bool        Initialize9(LPVOID pVB);

    void        ReleaseD3D(void)                { (this->*pReleaseD3D)(); }
    void        (CVertexBuffer::*pReleaseD3D)(void);
    void        ReleaseD3D6(void);
    void        ReleaseD3D7(void);
    void        ReleaseD3D8(void);
    void        ReleaseD3D9(void);

	virtual void*	GetInternalPointer(void);

    HRESULT             (CVertexBuffer::*pLock)(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags);
    virtual HRESULT     Lock6(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags);
    virtual HRESULT     Lock7(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags);
    virtual HRESULT     Lock8(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags);
    virtual HRESULT     Lock9(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags);

    HRESULT             (CVertexBuffer::*pUnlock)(void);
    virtual HRESULT     Unlock6(void);
    virtual HRESULT     Unlock7(void);
    virtual HRESULT     Unlock8(void);
    virtual HRESULT     Unlock9(void);

    HRESULT             (CVertexBuffer::*pOptimize)(void);
    virtual HRESULT     Optimize6(void);
    virtual HRESULT     Optimize7(void);
    virtual HRESULT     OptimizeNotSupported(void);

    HRESULT             (CVertexBuffer::*pProcessVertices)(DWORD VertexOp, DWORD DestIndex, DWORD Count, LPVERTEXBUFFER SrcBuffer, DWORD SrcIndex, DWORD dwFlags);
    virtual HRESULT     ProcessVertices6(DWORD VertexOp, DWORD DestIndex, DWORD Count, LPVERTEXBUFFER SrcBuffer, DWORD SrcIndex, DWORD dwFlags);
    virtual HRESULT     ProcessVertices7(DWORD VertexOp, DWORD DestIndex, DWORD Count, LPVERTEXBUFFER SrcBuffer, DWORD SrcIndex, DWORD dwFlags);
    virtual HRESULT     ProcessVerticesNotSupported(DWORD VertexOp, DWORD DestIndex, DWORD Count, LPVERTEXBUFFER SrcBuffer, DWORD SrcIndex, DWORD dwFlags);

    HRESULT             (CVertexBuffer::*pProcessVerticesStrided)(DWORD VertexOp, DWORD DestIndex, DWORD Count, LPDRAWPRIMITIVESTRIDEDDATA SrcVertexArray, DWORD SrcIndex, DWORD dwFlags);
    virtual HRESULT     ProcessVerticesStrided7(DWORD VertexOp, DWORD DestIndex, DWORD Count, LPDRAWPRIMITIVESTRIDEDDATA SrcVertexArray, DWORD SrcIndex, DWORD dwFlags);
    virtual HRESULT     ProcessVerticesStridedNotSupported(DWORD VertexOp, DWORD DestIndex, DWORD Count, LPDRAWPRIMITIVESTRIDEDDATA SrcVertexArray, DWORD SrcIndex, DWORD dwFlags);

    DWORD               (CVertexBuffer::*pGetPriority)(void);
    virtual DWORD	    GetPriorityNotSupported(void);
    virtual DWORD	    GetPriority8(void);
    virtual DWORD	    GetPriority9(void);

    DWORD               (CVertexBuffer::*pSetPriority)(DWORD PriorityNew);
    virtual DWORD	    SetPriorityNotSupported(DWORD PriorityNew);
    virtual DWORD	    SetPriority8(DWORD PriorityNew);
    virtual DWORD	    SetPriority9(DWORD PriorityNew);

	void                (CVertexBuffer::*pPreLoad)();
    virtual void	    PreLoadNotSupported();
    virtual void	    PreLoad8();
    virtual void	    PreLoad9();
	
    UINT                (CVertexBuffer::*pGetSize)(void);
    virtual UINT        GetSize6(void);
    virtual UINT        GetSize7(void);
    virtual UINT        GetSize8(void);
    virtual UINT        GetSize9(void);

    HRESULT (CVertexBuffer::*pGetDesc)(PVERTEXBUFFERDESC pDesc);
    virtual HRESULT GetDesc6(PVERTEXBUFFERDESC pDesc);
	virtual HRESULT GetDesc7(PVERTEXBUFFERDESC pDesc);
	virtual HRESULT GetDesc8(PVERTEXBUFFERDESC pDesc);
    virtual HRESULT GetDesc9(PVERTEXBUFFERDESC pDesc);
    
    DWORD               GetVertexFormat(void)           { return m_dwVertexFormat; }
    void                SetVertexFormat(DWORD dwFVF)    { m_dwVertexFormat = dwFVF; }

    LPDIRECT3DVERTEXBUFFER		GetVertexBufferPointer6(void) { return m_pVB6; }
    LPDIRECT3DVERTEXBUFFER7     GetVertexBufferPointer7(void) { return m_pVB7; }
    LPDIRECT3DVERTEXBUFFER8     GetVertexBufferPointer8(void) { return m_pVB8; }
    LPDIRECT3DVERTEXBUFFER9     GetVertexBufferPointer9(void) { return m_pVB9; }

public:
	CVertexBuffer();
    ~CVertexBuffer();

    virtual ULONG WINAPI    AddRef(void);
    virtual ULONG WINAPI    Release(void);

    VERTEXBUFFERDESC    	m_Desc;

    virtual bool        Initialize(PVOID pVB, CDevice* pDevice);

    virtual HRESULT     Lock(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags)
                            { return (this->*pLock)(OffsetToLock, SizeToLock, ppbData, Flags); }
    virtual HRESULT     Lock(UINT OffsetToLock, UINT SizeToLock, BYTE** ppbData, DWORD Flags)
                            { return (this->*pLock)(OffsetToLock, SizeToLock, (VOID**)ppbData, Flags); }
	virtual HRESULT     Unlock(void) { return (this->*pUnlock)(); }
	virtual HRESULT     Optimize(void) { return (this->*pOptimize)(); }
	virtual HRESULT     ProcessVertices(DWORD VertexOp, DWORD DestIndex, DWORD Count, LPVERTEXBUFFER SrcBuffer, DWORD SrcIndex, DWORD dwFlags)
							{ return (this->*pProcessVertices)(VertexOp, DestIndex, Count, SrcBuffer, SrcIndex, dwFlags); }	
	virtual HRESULT     ProcessVerticesStrided(DWORD VertexOp, DWORD DestIndex, DWORD Count, LPDRAWPRIMITIVESTRIDEDDATA SrcVertexArray, DWORD SrcIndex, DWORD dwFlags) 
							{ return (this->*pProcessVerticesStrided)(VertexOp, DestIndex, Count, SrcVertexArray, SrcIndex, dwFlags); }	
	virtual DWORD	    GetPriority(void) { return (this->*pGetPriority)(); }
	virtual DWORD	    SetPriority(DWORD PriorityNew) { return (this->*pSetPriority)(PriorityNew); }
    virtual	HRESULT 	GetDesc(PVERTEXBUFFERDESC pDesc) { return (this->*pGetDesc)(pDesc); }
    virtual UINT        GetSize(void){ return (this->*pGetSize)(); }
	virtual void	    PreLoad(void) { (this->*pPreLoad)(); }
	
};


// --------------------------------------------------------------------------------------

class CIndexBuffer : public CObject
{
protected:
    friend class CDevice;

    // d3d version specific data and accessor
    LPDIRECT3DINDEXBUFFER8      m_pIB8;
    LPDIRECT3DINDEXBUFFER9      m_pIB9;
    
	LPDIRECT3DINDEXBUFFER8      GetIndexBufferPointer8(void) { return m_pIB8; }
    LPDIRECT3DINDEXBUFFER9      GetIndexBufferPointer9(void) { return m_pIB9; }

    CDevice*                    m_pDevice;


    // Inialization
	virtual bool		Init8(PVOID pIB);
    virtual bool		Init9(PVOID pIB);
    
	void				ReleaseD3D(void) { (this->*pReleaseD3D)(); }
    void             	(CIndexBuffer::*pReleaseD3D)();
    void     			ReleaseD3D8();
    void     			ReleaseD3D9();

	inline LPDIRECT3DINDEXBUFFER8	GetInternalPointer8(void){ return m_pIB8; }
    inline LPDIRECT3DINDEXBUFFER9	GetInternalPointer9(void){ return m_pIB9; }

    HRESULT             (CIndexBuffer::*pLock)(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags);
    virtual HRESULT     LockNotImplemented(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags);
    virtual HRESULT     Lock8(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags);
    virtual HRESULT     Lock9(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags);

    HRESULT             (CIndexBuffer::*pUnlock)(void);
    virtual HRESULT     UnlockNotImplemented(void);
    virtual HRESULT     Unlock8(void);
    virtual HRESULT     Unlock9(void);

	DWORD               (CIndexBuffer::*pGetPriority)(void);
    virtual DWORD	    GetPriorityNotImplemented(void);
    virtual DWORD	    GetPriority8(void);
    virtual DWORD	    GetPriority9(void);

	DWORD               (CIndexBuffer::*pSetPriority)(DWORD dwNewPriority);
    virtual DWORD	    SetPriorityNotImplemented(DWORD dwNewPriority);
    virtual DWORD	    SetPriority8(DWORD PriorityNew);
    virtual DWORD	    SetPriority9(DWORD PriorityNew);

	void                (CIndexBuffer::*pPreLoad)();
    virtual void	    PreLoadNotImplemented();
    virtual void	    PreLoad8();
    virtual void	    PreLoad9();

    UINT                (CIndexBuffer::*pGetSize)(void);
    virtual UINT        GetSize8(void);
    virtual UINT        GetSize9(void);
    virtual UINT        GetSizeNotImplemented(void);

	HRESULT (CIndexBuffer::*pGetDesc)(PINDEXBUFFERDESC pDesc);
	virtual HRESULT GetDesc8(PINDEXBUFFERDESC pDesc);
	virtual HRESULT GetDesc9(PINDEXBUFFERDESC pDesc);
	virtual HRESULT GetDescNotImplemented(PINDEXBUFFERDESC pDesc);
	
public:
    
	INDEXBUFFERDESC    	m_Desc;

	CIndexBuffer();
    ~CIndexBuffer();

    virtual bool        Initialize(PVOID pIB, CDevice* pDevice);

    virtual ULONG WINAPI    AddRef(void);
    virtual ULONG WINAPI    Release(void);
    virtual HRESULT     	Lock(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags)
								{ return (this->*pLock)(OffsetToLock, SizeToLock, ppbData, Flags); }
    virtual HRESULT     	Lock(UINT OffsetToLock, UINT SizeToLock, BYTE** ppbData, DWORD Flags)
								{ return (this->*pLock)(OffsetToLock, SizeToLock, (VOID**)ppbData, Flags); }
    virtual HRESULT     	Unlock(void) { return (this->*pUnlock)(); }
    virtual UINT        	GetSize(void) { return (this->*pGetSize)(); }
    virtual	HRESULT 		GetDesc(PINDEXBUFFERDESC pDesc) { return (this->*pGetDesc)(pDesc); }
	virtual DWORD	    	SetPriority(DWORD dwNewPriority) { return (this->*pSetPriority)(dwNewPriority); }
	virtual DWORD	    	GetPriority(void) { return (this->*pGetPriority)(); }
	virtual void	    	PreLoad(void) { (this->*pPreLoad)(); }
};

// -------------------------------------------------------------------------------------
// CnVertexBuffer - used to tell framework to handle lists of VBs into wrapper calls

class CnVertexBuffer : public CObject
{
private:
    CVertexBuffer** m_pVertexBuffers;
    UINT            m_uDevices;
    LOCK_LIST*      m_pHeadLock;
    bool            SkipDevice(UINT uDevice, DWORD dwFlags);
public:
    CnVertexBuffer();
    ~CnVertexBuffer();
    virtual bool  Init(UINT uDevices);
    virtual bool  GetVertexBuffer(CVertexBuffer **ppVertexBuffer, UINT n);
    virtual bool  SetVertexBuffer(CVertexBuffer *pVertexBuffer, UINT n);
    virtual bool  Lock(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags, DWORD dwFlags =CD3D_ALL);
    virtual bool  Lock(UINT OffsetToLock, UINT SizeToLock, BYTE** ppbData, DWORD Flags, DWORD dwFlags =CD3D_ALL)
		{ return Lock(OffsetToLock, SizeToLock, (VOID**)ppbData, Flags, dwFlags); }
    virtual bool  Unlock(DWORD dwFlags = CD3D_ALL);
	virtual bool  Optimize(DWORD dwFlags = CD3D_ALL);
    virtual bool  ProcessVertices(DWORD VertexOp, DWORD DestIndex, DWORD Count, LPVERTEXBUFFERS SrcBuffer, DWORD SrcIndex, DWORD dwFlags, DWORD dwDeviceFlags = CD3D_ALL);
    virtual bool  ProcessVerticesStrided(DWORD VertexOp, DWORD DestIndex, DWORD Count, LPDRAWPRIMITIVESTRIDEDDATA SrcVertexArray, DWORD SrcIndex, DWORD dwFlags, DWORD dwDeviceFlags = CD3D_ALL);
    virtual DWORD GetPriority(DWORD dwFlags = CD3D_SRC);
    virtual DWORD SetPriority(DWORD PriorityNew, DWORD dwFlags = CD3D_ALL);
	virtual void  PreLoad(DWORD dwFlags = CD3D_ALL);
	virtual bool  GetDesc(PVERTEXBUFFERDESC pDesc, DWORD dwFlags = CD3D_ALL);
};

// -------------------------------------------------------------------------------------
// CnIndexBuffer

class CnIndexBuffer : public CObject
{
private:
    CIndexBuffer**  m_pIndexBuffers;
    UINT            m_uDevices;
    LOCK_LIST*      m_pHeadLock;
    bool            SkipDevice(UINT uDevice, DWORD dwFlags);
public:
    CnIndexBuffer();
    ~CnIndexBuffer();
    virtual bool Init(UINT uDevices);
    virtual bool GetIndexBuffer(CIndexBuffer **ppIndexBuffer, UINT n);
    virtual bool SetIndexBuffer(CIndexBuffer *pIndexBuffer, UINT n);

    virtual bool  Lock(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags, DWORD dwFlags =CD3D_ALL);
    virtual bool  Lock(UINT OffsetToLock, UINT SizeToLock, BYTE** ppbData, DWORD Flags, DWORD dwFlags =CD3D_ALL)
		{ return Lock(OffsetToLock, SizeToLock, (VOID**)ppbData, Flags, dwFlags); }
    virtual bool  Unlock(DWORD dwFlags =CD3D_ALL);
	virtual bool  GetDesc(PINDEXBUFFERDESC pDesc, DWORD dwFlags = CD3D_ALL);
	virtual DWORD GetPriority(DWORD dwFlags = CD3D_SRC);
    virtual DWORD SetPriority(DWORD PriorityNew, DWORD dwFlags = CD3D_ALL);
    virtual void  PreLoad(DWORD dwFlags = CD3D_ALL);
};

#define BeginVertexBufferMap(lpNVertexBuffer, lpVertexBuffer) \
    for( UINT vb_index_i=0; vb_index_i<m_uDeviceList; vb_index_i++ ) \
    { \
        if( lpNVertexBuffer->GetVertexBuffer( &lpVertexBuffer, vb_index_i  ) ) \
        {

#define EndVertexBufferMap( lpNVertexBuffer, lpVertexBuffer) \
        } \
        RELEASE( lpVertexBuffer ); \
    }

#define BeginIndexBufferMap(lpNIndexBuffer, lpIndexBuffer) \
    for( UINT ib_index_i=0; ib_index_i<m_uDeviceList; ib_index_i++ ) \
    { \
        if( lpNIndexBuffer->GetIndexBuffer( &lpIndexBuffer, ib_index_i  ) ) \
        {

#define EndIndexBufferMap( lpNIndexBuffer, lpIndexBuffer) \
        } \
        RELEASE( lpIndexBuffer ); \
    }


#endif //__CBuffers_h__
