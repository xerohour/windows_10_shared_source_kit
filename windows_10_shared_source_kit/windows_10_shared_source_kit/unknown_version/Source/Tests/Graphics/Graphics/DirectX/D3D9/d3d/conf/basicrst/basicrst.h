/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	BasicRst.h
 *
 ***************************************************************************/

#ifndef __BASICRST_H__
#define __BASICRST_H__

#include "CD3DTest.h"
#include "CSurface.h"

// Defines
#define CULLING_TESTS	     3
#define DITHER_TESTS	     8
#define FILL_TESTS		    20
#define SHADING_TESTS	    14
#define ZWRITE_TESTS	     4
#define MESH_TL_TESTS	    20
#define MESH_L_TESTS	    19
#define MESH_D3D_TESTS	    22
#define COLORWRITE_TESTS	16

// Base Class definitions
class CBasicRstTest: public CD3DTest
{
    // Data
	public:
    D3DTLVERTEX     VertexList[400];
    DWORD           dwVertexCount;
	TCHAR			msgString[80];

	public:
	CBasicRstTest();
	~CBasicRstTest();

	// Framework functions
	virtual bool ClearFrame(void);
	virtual void SceneRefresh(void);
};

// Culling Class definitions
class CCullingTest: public CBasicRstTest
{
	public:
	CCullingTest();
	~CCullingTest();

	// Framework functions
	UINT TestInitialize(void);
	bool ExecuteTest(UINT uTestNum);

	// Helper functions
	void DrawCullTests(void);
};

// Dither Class definitions
class CDitherTest: public CBasicRstTest
{
	public:
	CDitherTest();
	~CDitherTest();

	// Framework functions
	UINT TestInitialize(void);
	bool ExecuteTest(UINT uTestNum);

	// Helper functions
	void DrawDitherTests(int nTest);
};

// Fill Class definitions
class CFillTest: public CBasicRstTest
{
	public:
	CFillTest();
	~CFillTest();

	// Framework functions
	UINT TestInitialize(void);
	bool ExecuteTest(UINT uTestNum);

	// Helper functions
	void DrawFillTests(int nTest);
};

// Shading Class definitions
class CShadingTest: public CBasicRstTest
{
    // Data
	private:
	D3DPRIMITIVETYPE PrimType;
    D3DTLVERTEX      Single1List[3];
    D3DTLVERTEX      Single2List[3];
    D3DTLVERTEX      Single3List[3];

	public:
	CShadingTest();
	~CShadingTest();

	// Framework functions
	UINT TestInitialize(void);
	bool ExecuteTest(UINT uTestNum);
	void SceneRefresh(void);

	// Helper functions
	void DrawShadeListTests(int nTest);
	void DrawShadeStripTests(void);
	void DrawShadeFanTests(void);
};

// ZWrite Class definitions
class CZWriteTest: public CBasicRstTest
{
	public:
	CZWriteTest();
	~CZWriteTest();

	// Framework functions
	UINT TestInitialize(void);
	bool ExecuteTest(UINT uTestNum);

	// Helper functions
	void DrawZWriteTests(void);
};

// MeshTL Class definitions
class CMeshTLTest: public CBasicRstTest
{
    // Data
	private:
    DWORD	dwWidth;
    DWORD	dwHeight;

	public:
	CMeshTLTest();
	~CMeshTLTest();

	// Framework functions
	UINT TestInitialize(void);
	bool ExecuteTest(UINT uTestNum);
	void SceneRefresh(void);

	// Helper functions
	void DrawCircle(LPD3DVECTOR pCenter, float Radius, int nDivisions, D3DCOLOR Color);
	void DrawMeshTests(int nTest);
};

// MeshL Class definitions
class CMeshLTest: public CBasicRstTest
{
    // Data
	private:
    struct LVERTEX {
        D3DVALUE     x;             // Homogeneous coordinates 
        D3DVALUE     y;
        D3DVALUE     z;
        D3DCOLOR     color;         // Vertex color 
        D3DCOLOR     specular;      // Specular component of vertex 
        D3DVALUE     tu;            // Texture coordinates 
        D3DVALUE     tv;

        LVERTEX() { }
        LVERTEX(const D3DVECTOR& v,
                    D3DCOLOR _color, D3DCOLOR _specular,
                    float _tu, float _tv)
            { x = v.x; y = v.y; z = v.z; 
              color = _color; specular = _specular;
              tu = _tu; tv = _tv;
            }
    };

	LVERTEX	LVertexList[100];

	public:
	CMeshLTest();
	~CMeshLTest();

	// Framework functions
	bool SetDefaultRenderStates(void);
	bool SetDefaultMatrices(void);
	UINT TestInitialize(void);
	bool ExecuteTest(UINT uTestNum);
	void SceneRefresh(void);

	// Helper functions
	void DrawMeshTests(int nTest);
};

// MeshD3D Class definitions
class CMeshD3DTest: public CBasicRstTest
{
    // Data
	private:
	D3DVERTEX	D3DVertexList[100];
 	MATERIAL    *pMaterial;

	public:
	CMeshD3DTest();
	~CMeshD3DTest();

	// Framework functions
	bool SetDefaultMaterials(void);
	bool SetDefaultMatrices(void);
	UINT TestInitialize(void);
	bool ExecuteTest(UINT uTestNum);
	void SceneRefresh(void);
	bool TestTerminate();

	// Helper functions
	D3DCOLOR DrawMeshTests(int nTest);
};

// ColorWrite Class definitions
class CColorWriteTest: public CBasicRstTest
{
    // Data
	private:
    bool    bAlphaMask;
    bool    bDestBuffer;
	DWORD	dwDefault;
    DWORD   dwClearAlpha;
    CSurface * pSrcTarget;
    CSurface * pSrcZBuffer;
	CSurface * pTempSurface;

	public:
	CColorWriteTest();
	~CColorWriteTest();

	// Framework functions
	UINT TestInitialize(void);
	bool ClearFrame(void);
	bool ExecuteTest(UINT uTestNum);
	void SceneRefresh(void);
	bool ProcessFrame(void);
    bool TestTerminate();

	// Helper functions
    bool CreateDestBuffer(void);
	void DrawColorWriteTests(void);
};

// NoDiffuse Class definitions
class CNoDiffuseTest: public CBasicRstTest
{
    // Data
	private:
    struct ND_VERTEX {
        D3DVALUE     x;     // Homogeneous coordinates 
        D3DVALUE     y;
        D3DVALUE     z;
        D3DVALUE     rhw;   // Reciprocal of homogeneous w 
        D3DVALUE     tu;    // Texture coordinates 
        D3DVALUE     tv;

        ND_VERTEX() { }
        ND_VERTEX(const D3DVECTOR& v,
                    D3DVALUE _rhw,
                    float _tu, float _tv)
            { x = v.x; y = v.y; z = v.z; 
              rhw = _rhw;
              tu = _tu; tv = _tv;
            }
    };

	ND_VERTEX	    NoDiffuseList[6];

	public:
	CNoDiffuseTest();
	~CNoDiffuseTest();

	// Framework functions
	UINT TestInitialize(void);
	bool ExecuteTest(UINT uTestNum);
	void SceneRefresh(void);

	// Helper functions
	void DrawNonDiffuseGrid(void);
};

#endif
