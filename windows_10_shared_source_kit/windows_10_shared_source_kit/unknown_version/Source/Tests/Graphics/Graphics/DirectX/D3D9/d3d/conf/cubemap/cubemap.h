//////////////////////////////////////////////////////////////////////
// Cubemap.h: interface for the CCubemap class.
//////////////////////////////////////////////////////////////////////

#ifndef __CUBEMAP_H__
#define __CUBEMAP_H__

#include "CD3DTest.h"
#include "CTextures.h"

struct TVertex {								// Cubemap vertex with 3D texture coords
	D3DVECTOR v;
	D3DVECTOR n;
	D3DVECTOR t;
};

struct TGVertex {								// Cubemap vertex without 3D texture coords
	D3DVECTOR v;
	D3DVECTOR n;
};

class CCubeTest: public CD3DTest
{
    // Data
	private:
	TVertex *       pTVertex;		// Sphere vertices with texture vectors
	TGVertex *      pTGVertex;		// Sphere vertices without texture vectors
	int             nVertexMax;

    // Data
	public:
	float			fDegInc;
	float			fZDepthMin;		
	float			fZDepthMax;	
 	MATERIAL *      pMaterial;
	CnCubeTexture * pCubeTexture;
    bool			bTexGen;
    TCHAR	        msgString[80];
	TCHAR			szStatus[80];

	CCubeTest();
	~CCubeTest();

	// Framework functions
	virtual bool SetDefaultMatrices(void);
	virtual bool SetDefaultMaterials(void);
	virtual bool SetDefaultRenderStates(void);
	virtual bool ClearFrame(void);
	virtual void SceneRefresh(void);
    virtual void UpdateStatus(void);
	virtual bool TestTerminate(void);

    // Helper functions
	void LoadSphere();
};

// Texture Class definitions
class CCubeTexTest: public CCubeTest
{
    // Data
	private:
	CImage * pImage[6];

	public:
	DWORD dwLoadOption;

	// Framework functions
	UINT TestInitialize(void);
	bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);
};

// CubeUpdateTex Class definitions
class CCubeUpdateTexTest: public CCubeTexTest
{
	public:
	CCubeUpdateTexTest();
};

// CubeCopyRects Class definitions
class CCubeCopyRectsTest: public CCubeTexTest
{
	public:
	CCubeCopyRectsTest();
};

// CubeLockCopy Class definitions
class CCubeLockCopyTest: public CCubeTexTest
{
	public:
	CCubeLockCopyTest();
};

// CubeUpdateSurf Class definitions
class CCubeUpdateSurfTest: public CCubeTexTest
{
	public:
	CCubeUpdateSurfTest();
};

// MipMap Class definitions
class CMipCubeTest: public CCubeTest
{
    // Data
	private:
	CImage *** pppImages;

	public:
	int nTestMax;
	int nLevelMax;
	bool bPointFilter;
	DWORD dwLoadOption;

	// Framework functions
	UINT TestInitialize(void);
	bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);
};

// MipCubeUT Class definitions
class CMipCubeUTTest: public CMipCubeTest
{
	public:
	CMipCubeUTTest();
};

// MipCubeCR Class definitions
class CMipCubeCRTest: public CMipCubeTest
{
	public:
	CMipCubeCRTest();
};

// MipCubeLC Class definitions
class CMipCubeLCTest: public CMipCubeTest
{
	public:
	CMipCubeLCTest();
};

// MipCubeUS Class definitions
class CMipCubeUSTest: public CMipCubeTest
{
	public:
	CMipCubeUSTest();
};

// CubeTexGen Class definitions
class CCubeTexGenTest: public CCubeTest
{
    // Data
	private:
	CImage * pImage[6];

    public:
	CCubeTexGenTest();

	// Framework functions
	virtual bool SetDefaultMatrices(void);
	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);
};

// CubeFilter Class definitions
class CCubeFilterTest: public CCubeTest
{
    // Data
	private:
    bool bMipDelay, bMipSkip;
    CImage * pMinImage[6];
	CImage * pMagImage[6];
	CImage *** pppMipImages;
	CnCubeTexture *pMagCubeTexture;
	CnCubeTexture *pMipCubeTexture;

    public:
	CCubeFilterTest();

	// Framework functions
	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);

    // Helper functions
    UINT SetupFilterMode(int);
};

// CubeBlend Class definitions
class CCubeBlendTest: public CCubeTest
{
    // Data
	private:
	CImage *pImage[6];
	CImage *pAlphaImage[6];

    public:
	CCubeBlendTest();

	// Framework functions
	virtual bool SetDefaultRenderStates(void);
	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);

    // Helper functions
    bool ApplyMaterial(int, bool);
    bool SetupBlendMode(int);
};

#endif