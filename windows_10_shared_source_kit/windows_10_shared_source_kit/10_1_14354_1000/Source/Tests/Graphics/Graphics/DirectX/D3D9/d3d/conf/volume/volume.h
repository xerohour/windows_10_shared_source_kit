/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	Volume.h
 *
 ***************************************************************************/

#ifndef __VOLUME_H__
#define __VOLUME_H__

#include "CD3DTest.h"
#include "CTextures.h"

const int MAX_TEXTURE_TESTS = 12;
const int MAX_MIP_TESTS = 8;
const int MAX_TEXGEN_TESTS = 12;
const int MAX_FILTER_TESTS = 12;
const int MAX_BLEND_TESTS = 5;
const int MAX_ADDRESS_TESTS = 132;

struct TLVertex {                   // Volume TL vertex with 3D texture coords
    D3DVALUE     x;             
    D3DVALUE     y;
    D3DVALUE     z;
    D3DVALUE     rhw;           
    D3DCOLOR     color;        
    D3DVALUE     tu;         
    D3DVALUE     tv;
    D3DVALUE     tw;
};

struct TVertex {				    // Volume vertex with 3D texture coords
	D3DVECTOR v;
	D3DVECTOR n;
	D3DVECTOR t;
};

struct TGVertex {				    // Volume vertex without 3D texture coords
	D3DVECTOR v;
	D3DVECTOR n;
};

// Class definition
class CVolumeTest: public CD3DTest
{
    // Data
	private:
	TVertex *       pTVertex;		// Sphere vertices with texture vectors
	TGVertex *      pTGVertex;		// Sphere vertices without texture vectors
 	MATERIAL *      pMaterial;
	int             nVertexMax;

    // Data
	public:
	TLVertex        VertexList[6];  // Grid vertices with texture vectors
	CnVolTexture *  pVolTexture;
	bool			bMipMap, bTexGen;
    TCHAR	        msgString[80];
	TCHAR			szStatus[80];

    public:
	CVolumeTest();
	~CVolumeTest();

	// Framework functions
    virtual bool SetDefaultMaterials(void);
    virtual bool SetDefaultRenderStates(void);
    virtual bool SetDefaultMatrices(void);
	virtual bool ClearFrame(void);
    virtual void SceneRefresh(void);
    virtual void UpdateStatus(void);
    virtual bool TestTerminate(void);

    // Helper functions
    void LoadSphere(void);
    void LoadGrid(float u0, float v0, float w0, float u1, float v1, float w1);
};

// Texture Class definitions
class CVolTexTest: public CVolumeTest
{
    // Data
	public:
	DWORD       dwLoadOption;

	// Framework functions
    bool SetDefaultMaterials(void) {return true;};
    bool SetDefaultMatrices(void) {return true;};
    bool SetDefaultRenderStates(void) {return true;};
	UINT TestInitialize(void);
	bool ExecuteTest(UINT);
};

// VolUpdateTex Class definitions
class CVolUpdateTexTest: public CVolTexTest
{
	public:
	CVolUpdateTexTest();
};

// VolLockCopy Class definitions
class CVolLockCopyTest: public CVolTexTest
{
	public:
	CVolLockCopyTest();
};

// MipMap Class definitions
class CMipVolTest: public CVolumeTest
{
    // Data
	public:
	DWORD       dwLoadOption;
	bool		bPointFilter;

	// Framework functions
	UINT TestInitialize(void);
	bool ExecuteTest(UINT);
};

// MipVolUT Class definitions
class CMipVolUTTest: public CMipVolTest
{
	public:
	CMipVolUTTest();
};

// MipVolLC Class definitions
class CMipVolLCTest: public CMipVolTest
{
	public:
	CMipVolLCTest();
};

// TexGen Class definitions
class CVolTexGenTest: public CVolumeTest
{
    // Data
	private:
	CnVolTexture *  pSingleVolTexture;

    public:
	CVolTexGenTest();
	~CVolTexGenTest();

	// Framework functions
	bool SetDefaultMatrices(void);
	UINT TestInitialize(void);
	bool ExecuteTest(UINT);
	bool TestTerminate(void);
};

// Filter Class definitions
class CVolFilterTest: public CVolumeTest
{
    // Data
	private:
	CnVolTexture *  pMagVolTexture;
	CnVolTexture *  pMipVolTexture;

    public:
	CVolFilterTest();
	~CVolFilterTest();

	// Framework functions
	UINT TestInitialize(void);
	bool ExecuteTest(UINT);
	bool TestTerminate(void);

    // Helper functions
    UINT SetupFilterMode(int);
};

// Blend Class definitions
class CVolBlendTest: public CVolumeTest
{
    // Data
	private:
	CnVolTexture *  pAlphaVolTexture;

    public:
	CVolBlendTest();
	~CVolBlendTest();

	// Framework functions
    bool SetDefaultMaterials(void) {return true;};
    bool SetDefaultMatrices(void) {return true;};
    bool SetDefaultRenderStates(void);
	UINT TestInitialize(void);
	bool ExecuteTest(UINT);
	bool TestTerminate(void);

    // Helper functions
    bool SetupBlendMode(int nMode);
    void LoadBlendingGrid(int nMesh, bool bBlack);
};

// Address Class definitions
class CVolAddressTest: public CVolumeTest
{
    // Data
	public:
	DWORD       dwAddressMode;

 	// Framework functions
    bool SetDefaultMaterials(void) {return true;};
    bool SetDefaultMatrices(void) {return true;};
    bool SetDefaultRenderStates(void) {return true;};
	UINT TestInitialize(void);
	bool ExecuteTest(UINT);
};

// VolAddressWrap Class definitions
class CVolAddressWrapTest: public CVolAddressTest
{
	public:
	CVolAddressWrapTest();
};

// VolAddressMirror Class definitions
class CVolAddressMirrorTest: public CVolAddressTest
{
	public:
	CVolAddressMirrorTest();
};

// VolAddressClamp Class definitions
class CVolAddressClampTest: public CVolAddressTest
{
	public:
	CVolAddressClampTest();
};

// VolAddressBorder Class definitions
class CVolAddressBorderTest: public CVolAddressTest
{
	public:
	CVolAddressBorderTest();
};

// VolAddressMirrorOnce Class definitions
class CVolAddressMirrorOnceTest: public CVolAddressTest
{
	public:
	CVolAddressMirrorOnceTest();
};

#endif
