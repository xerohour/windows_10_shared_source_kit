//
//  VBSwap.h
//

#ifndef __VBSwap_H__
#define __VBSwap_H__

#define TESTDXVER 0x0800

#include <d3d8.h>
#include <CShapes.h>
// temporary
//class CMesh;

typedef struct tagFVFVERTEX
{
	float x, y, z;
	float nx, ny, nz;
	float tu, tv;
}FVFVERTEX, *PFVFVERTEX;

#define FVFVERTEXHANDLE	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

//-----------------------------------------------------------------------------

typedef struct _DEV_CONTEXT
{
	CDevice* pDevice;
	CTexture* pTexture;
	CTexture* pDynamicTexture;
	CVertexBuffer* pVB;
	CIndexBuffer* pIB;
}DEV_CONTEXT;

//-----------------------------------------------------------------------------
//
//  The VBSwap specification
//
//-----------------------------------------------------------------------------

class CVBSwapTest : public CD3DTest
{
public:
	CVBSwapTest();
    bool SortVertexBuffer(PVOID pVBDataDest,  PVOID pVBDataSrc, WORD *pwIBDataSrc, UINT uIndexCount, UINT uVertexSize);
	bool SetDefaultRenderStates();
	bool SetDefaultMaterials();
	bool SetDefaultMatrices();
	bool SetDefaultLights();
	UINT TestInitialize();
	bool ExecuteTest(UINT nTest);
	bool TestTerminate();
private:
//	CMD2Mesh*       m_pMesh;
//	CMesh*          m_pMesh;
    CShapes         m_Shape[10];
    PFVFVERTEX      m_pfvfTriSorted[10];

    
	bool            m_bDynamicTextures;
	bool			m_bHandleExceptions;
	DEV_CONTEXT		m_SrcContext;
	bool			Draw(UINT n, DEV_CONTEXT* pContext);
	UINT			InitResources(DEV_CONTEXT* pContext);
	bool            CopyTexture( CTexture* pDst, CTexture* pSrc );

};

#if 0
//-----------------------------------------------------------------------------
// Some mesh loading stuff...
//-----------------------------------------------------------------------------


#define MESH_KEYFRAMES 32

#define MESH_FVF D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0)

typedef struct _MESH_VERTEX
{
	float x,y,z;
	float nx,ny,nz;
	float u,v;
}MESH_VERTEX;


class CMesh
{
protected:
	int m_iNumKeyFrames;
	int m_iNumIndexedVertices;
	int m_iNumVertices;
	int m_iNumIndices;
	int m_iNumPrims;
	int m_iVertexStride;
	MESH_VERTEX** m_pIndexedVertices;
	MESH_VERTEX** m_pVertices;
	WORD* m_pIndices;
	bool LoadFromXFile( CDevice* pDevice, LPSTR szFileName );
	void  ReleaseTest();
public:
	CMesh();
	static CMesh* CreateMeshFromXFile(CDevice* pDevice, LPSTR szFileName);
	ULONG Release();

	inline int GetNumFrames() { return m_iNumKeyFrames; }
	inline int GetNumIndexedVertices() { return m_iNumIndexedVertices; }
	inline int GetNumVertices() { return m_iNumVertices; }
	inline int GetNumIndices() { return m_iNumIndices; }
	inline int GetNumTriangles() { return m_iNumPrims; }
	inline DWORD GetFVF() { return (DWORD)(MESH_FVF); }
	inline int GetVertexStride() { return m_iVertexStride; }
	inline D3DPRIMITIVETYPE GetPrimitiveType() { return D3DPT_TRIANGLELIST; }
	inline WORD* GetIndices() { return m_pIndices; }
	inline MESH_VERTEX* GetIndexedVertexData(int iKeyFrame) { return m_pIndexedVertices[iKeyFrame]; }
	inline MESH_VERTEX* GetVertexData(int iKeyFrame) { return m_pVertices[iKeyFrame]; }
};
#endif

#endif

