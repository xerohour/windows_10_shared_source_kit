/*Header for a non power of two texture test.
This is an abstract class.
Stress and conformance tests will be derived from this class*/

#ifndef __NONPOWTWOTEST_H_
#define __NONPOWTWOTEST_H_

#include "CD3DTest.h"
#include "TestMatrix.h"
#include "testcasestruct.h"

struct TLVERTEX
{
	float	Position  [4];
	D3DCOLOR color;
	float	_2DCoords [2];
	float	CubeCoords[3];
	float	_3DCoords [3];

	TLVERTEX() { }
    TLVERTEX(const D3DXVECTOR3& v, D3DCOLOR c, const D3DXVECTOR2& t0, const D3DXVECTOR3& t1, const D3DXVECTOR3& t2)
	{
		Position[0] = v.x;  Position[1] = v.y;  Position[2] = v.z; Position[3] = 1.f;
        color = c;
        _2DCoords[0]  = t0.x; _2DCoords[1]  = t0.y;
		CubeCoords[0] = t1.x; CubeCoords[1] = t1.y; CubeCoords[2] = t1.z;
		_3DCoords[0]  = t2.x; _3DCoords[1]  = t2.y; _3DCoords[2]  = t2.z;
    }
};
const int TLFVF = D3DFVF_XYZRHW  | D3DFVF_DIFFUSE | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE3(1) | D3DFVF_TEXCOORDSIZE3(2);

struct VERTEX
{
	float	Position  [3];
	float	Normal    [3];
	
	float	_2DCoords [2];
	float	CubeCoords[3];
	float	_3DCoords [3];

	VERTEX() { }
    VERTEX(const D3DXVECTOR3& v, const D3DXVECTOR3& n, const D3DXVECTOR2& t0, const D3DXVECTOR3& t1, const D3DXVECTOR3& t2)
	{
		Position[0] = v.x;  Position[1] = v.y;  Position[2] = v.z;
		Normal[0] = n.x;  Normal[1] = n.y;  Normal[2] = n.z;
        _2DCoords[0]  = t0.x; _2DCoords[1]  = t0.y;
		CubeCoords[0] = t1.x; CubeCoords[1] = t1.y; CubeCoords[2] = t1.z;
		_3DCoords[0]  = t2.x; _3DCoords[1]  = t2.y; _3DCoords[2]  = t2.z;
    }
};
const int FVF = D3DFVF_XYZ  | D3DFVF_NORMAL | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE3(1) | D3DFVF_TEXCOORDSIZE3(2);

class CNonPowTwoTest : public CD3DTest
{
public:
	CNonPowTwoTest();
	virtual ~CNonPowTwoTest();

	virtual bool    SetupTest(void);
    virtual void    ConstructTestCaption(LPTSTR, int);
	virtual bool	CapsCheck();
    virtual bool    Setup();
	virtual void    Cleanup();
	virtual UINT	TestInitialize();
	virtual bool	ExecuteTest(UINT nTestNum);   
	virtual void	UpdateStatus();
	virtual bool    TestTerminate();
	virtual void	CommandLineHelp();

protected:

	bool           InitializeTextures();
	void           CleanupTextures();
	bool           FilterTextureInfo(const DEVICEDESC& Caps, STextureInfo& TexInfo);

	CnBaseTexture* Create2DTexture(const UINT index, STextureInfo& State);
	bool           FillTextureLevelGradient(const UINT index, const UINT nLevel, CnTexture* pTexture);
	bool           FillTextureLevelStripes (const UINT index, const UINT nLevel, CnTexture* pTexture);
	bool           FillTextureLevelImage   (                  const UINT nLevel, CnTexture* pTexture, const TCHAR* szFileName);

	CnBaseTexture* Create3DTexture(const UINT index, STextureInfo& State);
	bool           FillVolumeWithGradient(const UINT index, const UINT nLevel, CnVolTexture* pTexture);
	bool           FillVolumeWithStripes (const UINT index, const UINT nLevel, CnVolTexture* pTexture);
	bool           FillVolumeWithImage   (                  const UINT nLevel, CnVolTexture* pTexture, const TCHAR* szFileName);

	CnBaseTexture* CreateCubeMap(const UINT index, STextureInfo& State);
	bool           FillCubeWithGradient(const UINT index, const UINT nLevel, CnCubeTexture* pTexture);
	bool           FillCubeWithStripes (const UINT index, const UINT nLevel, CnCubeTexture* pTexture);
	bool           FillCubeWithImage   (                  const UINT nLevel, CnCubeTexture* pTexture, const TCHAR* szFileName);

	UINT           ForcePOW2(UINT nDimension);//returns the next highest lowest pow2 dimension (i.e. ForcePOW2(258) = 256)
	bool           IsDXTN(const FORMAT& TextureFormat);
	UINT           ForceMultipleFour(UINT nDimension);//outputs the next highest power of 4
	UINT           ForceDWORDMultiple(UINT nDimension);//outputs the next highest power of 32

	//Setup default world, view and projection matrices
	virtual bool SetupMatrices();

	//setup default render and texture states
	virtual bool SetDefaultStates();
	
	//adds all tests cases to the test matrix returned by GetTestMatrix().  
	//Then it initializes all test cases with the return value from ShouldCapsCheck().
	virtual bool AddTestCases(CTestMatrix* pTestMatrix);

	//To be written by derived classes.

	//Returns true if only those tests which the device caps supports should be run
	virtual bool ShouldCapsCheck() = 0;

	//Returns a pointer to the test matrix to be used by this test
	virtual CTestMatrix* GetTestMatrix() = 0;

	//Call pTestMatrix->SetupCombination() based on nTestNum
	virtual bool SetupTestState(TCHAR *testName, CTestMatrix* pTestMatrix, const UINT nTestNum) = 0;

	//Allocate all geometry related resources
	virtual bool InitGeometry() = 0;

	//Render the scene.  This function will be called in the middle of a BeginScene() / EndScene() block.
	virtual bool RenderScene() = 0;

	//create group name. This function will be called in Setuptest() to be set m_szGroupName which will be used in ConstructTestCaption
	virtual void CreateGroupName();

	//Return the number of test cases to be run using pTestMatrix.
	virtual void SetTestCaseRange(CTestMatrix* pTestMatrix) = 0;

private:
	bool        *m_bUsedFmt [TEXTURE_COUNT];
	TCHAR        m_szGroupName[NAME_SIZE];
	UINT         m_nFmt;

protected:
	UINT         m_uRectHeight, m_uRectWidth;
	D3DXMATRIX   m_mView, m_mProj, m_mWorld[2], m_mTransform[2];
	bool         m_bCubePow2;
	bool         m_bVolumePow2;
	bool         m_bTexPow2;
	bool		 m_bNonPow2Cond;
	bool		 m_bSkipAll;			//only used for StressTest
};

void DeletionCallBack(CTestVariable* pDelete);


#endif
