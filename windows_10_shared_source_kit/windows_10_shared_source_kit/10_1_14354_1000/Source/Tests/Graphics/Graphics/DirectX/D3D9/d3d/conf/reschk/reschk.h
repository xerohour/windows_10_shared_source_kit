//-----------------------------------------------------------------------------
// File: reschk.h
//
// Desc: 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __RESCHK_H__
#define __RESCHK_H__

#include "d3d9.h"
#include "CD3DTest.h"
#include "CBuffers.h"
#include "CTextures.h"
#include "CSurface.h"
#include "CResource.h"

#include "CPControl.h"

#include <list>

using namespace std;

#define RESOURCE_PRIORITY_DISCARD        0x28000000
#define RESOURCE_PRIORITY_LOW            0x50000000
#define RESOURCE_PRIORITY_NORMAL         0x78000000
#define RESOURCE_PRIORITY_HIGH           0xa0000000
#define RESOURCE_PRIORITY_MAXIMUM        0xc8000000

#define PAGEOUTRESOURCE_FAIL        0x0000
#define PAGEOUTRESOURCE_PASS        0x0001
#define PAGEOUTRESOURCE_SKIP        0x0002

struct DIFUSEVERTEX
{
    D3DVALUE x, y, z, rhw;
    D3DCOLOR color;

    DIFUSEVERTEX() { }
    DIFUSEVERTEX(D3DVALUE _x, D3DVALUE _y, D3DVALUE _z, D3DVALUE _rhw, D3DCOLOR _color)
	{
		x = _x;  y = _y;  z = _z; rhw = _rhw;
        color = _color;
    }
    DIFUSEVERTEX operator=(DIFUSEVERTEX n) 
    {
        x = n.x; y = n.y; z = n.z; rhw = n.rhw;
        color = n.color;
        return *this;
    }
};
#define D3DFVF_DIFUSEVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

struct TEX1VERTEX
{
    DIFUSEVERTEX vertex;
    D3DVALUE tu, tv;

    TEX1VERTEX() { }
    TEX1VERTEX(DIFUSEVERTEX v, D3DVALUE _tu, D3DVALUE _tv)
	{
		vertex = v;
        tu = _tu; tv = _tv;
    }
};
#define D3DFVF_TEX1VERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

struct VOLVERTEX
{
    DIFUSEVERTEX vertex;
	D3DVALUE    tu, tv, tw;

	VOLVERTEX() { }
    VOLVERTEX(DIFUSEVERTEX v, D3DVALUE _tu, D3DVALUE _tv, D3DVALUE _tw)
	{
		vertex = v;
	    tu = _tu; tv = _tv; tw = _tw;
    }
};
#define FVF_VOLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE3(0))


struct RESCHK_TESTCASES
{
    DWORD dwPriority;
    BOOL  bUseContentProtection;
    D3DAUTHENTICATEDCHANNELTYPE AuthenticatedChannelType;
};

//----------------------------------------------------------------------------
// Name: CReschk
// Desc: 
//-----------------------------------------------------------------------------
class CReschk: public CD3DTest
{
    // Private Data
	private:
        LPRESOURCE m_pTestResource;
        LPRESOURCE m_pCopyResource;

        UINT m_nMinPageOutTries;
        UINT m_nMaxSkipPageOutTries;
        UINT m_nMaxFailPageOutTries;

        DIFUSEVERTEX m_Vertices[4];

        double        m_dQPCFrequency;
        LARGE_INTEGER m_liStartTime;
        LARGE_INTEGER m_liQPCEnd;
        double        m_dLoadTime;

        // Content Protection helper object
        CPControl* m_pCPControl;

        UINT m_nMaxTestCases;

    // Public Data
	public:
        UINT m_nMaxPageOutResources;
        UINT m_nMaxPageOutRender;
        
    // Private Test Functions
	private:
        double TimedLoadResource(LPRESOURCE pResource);
        bool   TestSetPriority(LPRESOURCE pResource, DWORD dwPriority);
        bool   TestGetPriority(LPRESOURCE pResource, DWORD dwPriority);
        UINT   TestPageOutResource(void);
        UINT   TestResourceResidency(void);
        HRESULT CreatePageOutTexture(CTexture** ppTexture);
        bool   CreateCPControl(D3DAUTHENTICATEDCHANNELTYPE Channel, BOOL bEnable);
        bool   ReleaseCPControl();
        bool   SetEncryptionGuidById(UINT nEncryptionGuidID);
        bool   TestEncryptionGuids();

    // Public Test Functions
    public:     
        CReschk();
        ~CReschk();
        HRESULT       SetTestResources(LPRESOURCE pTest, LPRESOURCE pCopy);
        HRESULT       CreateVBTEX1(CVertexBuffer** ppVertexBuffer);
        DIFUSEVERTEX* GetVertices(void) {return m_Vertices;}
        void          StartTime(void);
        void          EndTime(void);

        virtual bool SetupResource(void)                {return false;} // Create any resources need for test
        virtual bool LoadResource(LPRESOURCE pResource) {return false;} // Try to page in resource
        
	// D3D Framework Derived Functions
	public:
	    virtual bool CapsCheck(void);       // Check caps requred by test. 
	    virtual UINT TestInitialize(void);  // Setup non-device specific settings, variables, ect.
	    virtual bool Setup(void);           // Setup device specific settings, variables, ect.
        virtual void Cleanup(void);         // Cleanup device specific settings, variables, ect. Tied with Setup()
	    virtual bool ExecuteTest(UINT);     // Called once for each test defined by SetTestRange() call
        virtual bool ClearFrame(void);      // Clear our buffers
        virtual void UpdateStatus(void);    // Called once each frame to update status window text

};

//----------------------------------------------------------------------------
// Name: CReschk_VB
// Desc: 
//-----------------------------------------------------------------------------
class CReschk_VB: public CReschk
{
    // Private Data
	private:
        CVertexBuffer* pVBTest;
        CVertexBuffer* pVBCopy;

        CResource      TestResource;
        CResource      CopyResource;
        
    // Public Data
	public:
    
    // Private Test Functions
	private:
        HRESULT CreateVB(CVertexBuffer** ppVertexBuffer);

    // Public Test Functions
    public:     
        CReschk_VB();
        ~CReschk_VB();
        virtual bool SetupResource(void);
        virtual bool LoadResource(LPRESOURCE pResource);

	// D3D Framework Derived Functions
	public:
        virtual void Cleanup(void);
};

//----------------------------------------------------------------------------
// Name: CReschk_IB
// Desc: 
//-----------------------------------------------------------------------------
class CReschk_IB: public CReschk
{
    // Private Data
	private:
        CVertexBuffer* pVB;

        CIndexBuffer* pIBTest;
        CIndexBuffer* pIBCopy;

        CResource      TestResource;
        CResource      CopyResource;
        
    // Public Data
	public:
        HRESULT CreateVB(CVertexBuffer** ppVertexBuffer);
    
    // Private Test Functions
	private:
        HRESULT CreateIB(CIndexBuffer** ppIndexBuffer);

    // Public Test Functions
    public:     
        CReschk_IB();
        ~CReschk_IB();
        virtual bool SetupResource(void);
        virtual bool LoadResource(LPRESOURCE pResource);

	// D3D Framework Derived Functions
	public:
        virtual void Cleanup(void);
};

//----------------------------------------------------------------------------
// Name: CReschk_Texture
// Desc: 
//-----------------------------------------------------------------------------
class CReschk_Texture: public CReschk
{
    // Private Data
	private:
        CVertexBuffer* pVB;
        CTexture*  pTestTexture;
        CTexture*  pCopyTexture;

        CResource      TestResource;
        CResource      CopyResource;
        
    // Public Data
	public:
    
    // Private Test Functions
	private:
        HRESULT CreateTexture(CTexture** ppTexture);

    // Public Test Functions
    public:     
        CReschk_Texture();
        ~CReschk_Texture();
        virtual bool SetupResource(void);
        virtual bool LoadResource(LPRESOURCE pResource);

	// D3D Framework Derived Functions
	public:
        virtual void Cleanup(void);
};

//----------------------------------------------------------------------------
// Name: CReschk_CubeTexture
// Desc: 
//-----------------------------------------------------------------------------
class CReschk_CubeTexture: public CReschk
{
    // Private Data
	private:
        CVertexBuffer* pVB;
        CCubeTexture*  pTestTexture;
        CCubeTexture*  pCopyTexture;

        CResource      TestResource;
        CResource      CopyResource;
        
    // Public Data
	public:
    
    // Private Test Functions
	private:
        HRESULT CreateCubeVB(CVertexBuffer** ppVertexBuffer);
        HRESULT CreateCubeTexture(CCubeTexture** ppTexture);

    // Public Test Functions
    public:     
        CReschk_CubeTexture();
        ~CReschk_CubeTexture();
        virtual bool SetupResource(void);
        virtual bool LoadResource(LPRESOURCE pResource);

	// D3D Framework Derived Functions
	public:
        virtual bool CapsCheck(void); 
        virtual void Cleanup(void);
};

//----------------------------------------------------------------------------
// Name: CReschk_CubeTexture
// Desc: 
//-----------------------------------------------------------------------------
class CReschk_VolumeTexture: public CReschk
{
    // Private Data
	private:
        CVertexBuffer* pVB;
        CVolTexture*  pTestTexture;
        CVolTexture*  pCopyTexture;

        CResource      TestResource;
        CResource      CopyResource;
        
    // Public Data
	public:
    
    // Private Test Functions
	private:
        HRESULT CreateVolVB(CVertexBuffer** ppVertexBuffer);
        HRESULT CreateVolTexture(CVolTexture** ppTexture);

    // Public Test Functions
    public:     
        CReschk_VolumeTexture();
        ~CReschk_VolumeTexture();
        virtual bool SetupResource(void);
        virtual bool LoadResource(LPRESOURCE pResource);

	// D3D Framework Derived Functions
    public:
        virtual bool CapsCheck(void);
        virtual void Cleanup(void);
};

//----------------------------------------------------------------------------
// Name: CReschk_Surface
// Desc: 
//-----------------------------------------------------------------------------
class CReschk_Surface: public CReschk
{
    // Private Data
	private:
        CSurface*  pTestSurface;
        CSurface*  pCopySurface;

        CResource      TestResource;
        CResource      CopyResource;
        
    // Public Data
	public:
    
    // Private Test Functions
	private:
        HRESULT CreateSurface(CSurface** ppSourface);

    // Public Test Functions
    public:     
        CReschk_Surface();
        ~CReschk_Surface();
        virtual bool SetupResource(void);
        virtual bool LoadResource(LPRESOURCE pResource);

	// D3D Framework Derived Functions
	public:
        virtual void Cleanup(void); 
};

#endif
