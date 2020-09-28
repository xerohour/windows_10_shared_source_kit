/******************************Module*Header*******************************
* Module Name: billboard.h
*
* Header for the D3D samples "billboard" ported to the base video test infrastructure
*
* Author: Blake Pelton [blakep]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/

#ifndef __SCH_BILLBOARD_H_
#define __SCH_BILLBOARD_H_

#ifndef D3DX_TEST_ONLY
	#define D3DX_TEST_ONLY
#endif

#include "d3d9.h"
#include "d3dx9.h"
#include "Testcomponent.h"
#include "ComPtr.h"
#include "tchar.h"
#include "sharedmemory.h"
#include <Quadrant.h>

#define NUMTREETEXTURES 9
#define NUM_TREES 500

class CD3DMesh;



// Custom vertex type for the trees
struct TREEVERTEX
{
    D3DXVECTOR3 p;      // Vertex position
    DWORD       color;  // Vertex color
    FLOAT       tu, tv; // Vertex texture coordinates

    static const DWORD FVF;
};

//-----------------------------------------------------------------------------
// Name: Tree
// Desc: Simple structure to hold data for rendering a tree
//-----------------------------------------------------------------------------
struct Tree
{
    TREEVERTEX  v[4];           // Four corners of billboard quad
    D3DXVECTOR3 vPos;           // Origin of tree
    DWORD       dwTreeTexture;  // Which texture map to use
    DWORD       dwOffset;       // Offset into vertex buffer of tree's vertices
};


/*
  Name: CTextureCreator
  Desc: Interface to a class that creates textures
  This abstracts if the texture was loaded from disk
  or a x-proc texture was opened
*/
class CTextureCreator
{
public:
    virtual ~CTextureCreator()
    {
    }
    
    /*
      Name: 
      Desc: Creates a D3DPOOL_DEFAULT texture with the same properties
      that are on the texture save to disk
    */
    virtual HRESULT CreateTexture(const wchar_t* szFileName,
                                  IDirect3DTexture9** ppOut) = 0;
};



class CBillBoard : public CTestComponent, public CTextureCreator
{
public:
    CBillBoard();

    virtual HRESULT CreateTexture(const wchar_t* szFileName,
                                  IDirect3DTexture9** ppOut);

    void OnEnumWindow(HWND hWnd);
    
protected:
    virtual void PreWalk(CLogWriter* pLog);
    virtual void PostWalk(CLogWriter* pLog);

private:
    void LoadResources();
    void Render(bool bExpectingDeviceLost);//renders 1 frame
    bool CheckCaps();
    void InitTrees();

    bool IsTreePositionValid(DWORD nTree);
    
    void DrawTrees();
    void FrameMove();

    void PumpMessages();

    //makes all windows visible on the screen
    //this ensures that their present operations correctly work
    void ArrangeWindows();
    

    //destroys all resources owned by this guy so that 
    //it can recover from a lost device
    void CleanupResources();

    //recovers the device from the lost state
    //or changes between windowed and full-screen modes
    void ResetDevice(bool bWindowed);
    
    /*
      Name: CreateATexture
      Desc: Actually creates a texture,
      This is called if a texture is not opened via cross-process resource
      sharing
    */
    HRESULT CreateATexture(const wchar_t* szFileName,
                           HANDLE* pSharedOut,//if non-NULL, then it is filled with the global handle
                           IDirect3DTexture9** ppOut);

private:
    CComPtr<IDirect3DDevice9Ex> m_pDevice;

    std::auto_ptr<CD3DMesh> m_pTerrain;
    std::auto_ptr<CD3DMesh> m_pSkyBox;

    CComPtr<IDirect3DTexture9> m_pTreeTextures[NUMTREETEXTURES];
    CComPtr<IDirect3DVertexBuffer9> m_pTreeVB;
    D3DXMATRIXA16    m_matBillboardMatrix; // Used for billboard orientation
    Tree             m_Trees[NUM_TREES];   // Array of tree info
    D3DXVECTOR3      m_vEyePt;

    D3DCAPS9 m_Caps;

    float m_fTime;

    ULONGLONG m_QPCFreq;//counts per second

    /*
      Name: m_SharedResources
      Desc: A table allows for interprocess communication
      to support x-proc resource sharing
    */
    CSharedMemory m_SharedResources;

    /*
      Name: m_SharedMutex
      Desc: A mutex that is taken whenever m_SharedResources is read or written
    */
    CNamedMutex m_SharedMutex;

    /*
      Name: m_bUseCrossProc
      Desc: If this is true then texture are shared across processes
      otherwise, they are not
    */
    bool m_bUseCrossProc;

    /*
      Name: m_nSleepTime
      Desc: The number of miliseconds to wait between each frame,
      if 0, then sleep is not called
    */
    UINT m_nSleepTime;

    /*
      Name: m_fTextureScale
      Desc: The amount scale the width and height of each texture by
    */
    float m_fTextureScale;

    /*
      Name: m_hRenderWnd
      Desc: The rendering window
    */
    HWND  m_hRenderWnd;

    /*
      Name: m_Siblings
      Desc: The list of all windows with the same name as m_hRenderWnd
      This is used to tile the windows
    */
    std::vector<HWND> m_Siblings;

    /*
      Name: m_nBackBuffer***
      Desc: The user-requested windowed mode back buffer info
    */
    D3DSURFACE_DESC m_BackBufferDesc;

    /*
      Name: m_nAdapterOridnal
      Desc: The D3D adapter ordinal that this device was created on
    */
    UINT m_nAdapterOrdinal;

    /*
      Name: m_bWindowed
      Desc: Keeps track of the current rendering mode (windowed vs. fullscreen)
    */
    bool m_bWindowed;

    CQuadrant* m_pWindowLoc;
};



//-----------------------------------------------------------------------------
// Name: class CD3DMesh
// Desc: Class for loading and rendering file-based meshes
//-----------------------------------------------------------------------------
class CD3DMesh
{
public:
    TCHAR               m_strName[512];

    LPD3DXMESH          m_pSysMemMesh;    // SysMem mesh, lives through resize
    LPD3DXMESH          m_pLocalMesh;     // Local mesh, rebuilt on resize
    
    DWORD               m_dwNumMaterials; // Materials for the mesh
    D3DMATERIAL9*       m_pMaterials;
    LPDIRECT3DTEXTURE9* m_pTextures;
    bool                m_bUseMaterials;

public:
    // Rendering
    HRESULT Render( LPDIRECT3DDEVICE9 pd3dDevice, 
                    bool bDrawOpaqueSubsets = true,
                    bool bDrawAlphaSubsets = true );

    // Mesh access
    LPD3DXMESH GetSysMemMesh() { return m_pSysMemMesh; }
    LPD3DXMESH GetLocalMesh()  { return m_pLocalMesh; }

    // Rendering options
    void    UseMeshMaterials( bool bFlag ) { m_bUseMaterials = bFlag; }
    HRESULT SetFVF( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwFVF );
    HRESULT SetVertexDecl( LPDIRECT3DDEVICE9 pd3dDevice, D3DVERTEXELEMENT9 *pDecl );

    // Initializing
    HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT InvalidateDeviceObjects();

    // Creation/destruction
    HRESULT Create( LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR strFilename, CTextureCreator* pTexCreator );
    HRESULT Destroy();

    CD3DMesh( LPCTSTR strName = _T("CD3DFile_Mesh") );
    virtual ~CD3DMesh();
};


#endif
