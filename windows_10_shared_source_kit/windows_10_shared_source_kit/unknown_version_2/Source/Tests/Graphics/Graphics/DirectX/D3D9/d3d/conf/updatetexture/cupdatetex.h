//
//  CUpdateTex.h
//
//  Class specification for CMipMap.
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 1997, Microsoft Corporation.
//
//  Original Author: xfu
//

#ifndef __CUPDATETEX__
#define __CUPDATETEX__


#define TESTDXVER 0x0800

#include "CD3DTest.h"

#include "CTextures.h"

#include "TestUtils.h"

#include "UtilFormats.h"

#include <initguid.h>
#include <d3d9.h>
#include <assert.h>

#define SafeRelease(a)    {if(a) {(a)->Release(); a = NULL;};}

enum SURFACEVALUE{ZERO,SEQ};

class CUpdateTexBase;
class UpdateTex;

typedef struct _TESTCASE 
{
	LPCTSTR						szTitle;
	UINT						uSrcSize;
	UINT						uSrcLevels;
	UINT						uDestSize;
	UINT						uDestLevels;
	BOOL						(UpdateTex::*PreUpdateFunc)();
	BOOL						(UpdateTex::*VerifyFunc)();
	BOOL						bExpectSucceed;
	BOOL						bSkipForManagedTextures;
} TESTCASE, *PTESTCASE;

typedef HRESULT (WINAPI * PD3D8GetSWInfo)(LPVOID pCaps, LPVOID pCallbacks);

//
//	Declare the base class CBaseTex
//

class CBaseTex
{
protected:
	CBaseTex(CUpdateTexBase *, CDevice*,D3DRESOURCETYPE);
	CUpdateTexBase *				m_pTestApp;
	SURFACEVALUE					m_OriginalValue;
	//BOOL							SetSurfaceValue (LPSURFACES,float,SURFACEVALUE, UINT uiByteValStart = 0);
	BOOL							SetSurfaceValue (LPSURFACES pDestSurface, LPVOID lpFileInMemory);
	BOOL							CheckVolumeValue (CVolume *,float,RECT *,SURFACEVALUE);
	BOOL							bIsDXTnFormat(D3DFORMAT);
	BOOL							bIsUYVYFormat(D3DFORMAT);
	DWORD							FindMask (D3DFORMAT);
	D3DRESOURCETYPE					m_ResType;

	CTexture						*m_pAuxTexture; // Data source for "dirty" surface content.

	static LPVOID BackResources[];
	static LPVOID ForeResources[];
	static const UINT cuiNoOfBackResources;
	static const UINT cuiNoOfForeResources;
	static bool bResourcesLoaded;

public:

	static bool LoadFileResources();

	virtual							~CBaseTex();
	virtual							operator CBaseTexture* ()=0;
	virtual LPBASETEXTURES			GetLPTEXTURES() = 0;
	virtual BOOL					SetTexValue (SURFACEVALUE )=0;
	virtual	BOOL					ValidateTexture()=0;
	virtual BOOL					LockWithNULL(DWORD flag=0)=0;
	virtual BOOL					LockManyAreas(UINT)=0;
	virtual BOOL					LockSubLevel()=0;
	virtual BOOL					AddDirtyWithNULL()=0;
	virtual BOOL					AddDirtyManyAreas(UINT)=0;
	virtual BOOL					CopyRects(UINT,D3DPOOL)=0;
	virtual BOOL					MakeNonSquare()=0;
	virtual BOOL					LockWithoutUnlock()=0;
	virtual BOOL					Unlock()=0;
	virtual BOOL					SetSrcTex(CBaseTex *)=0;
	virtual HRESULT					Render(UINT uiMipLevel) = 0;
	virtual void					DirtyEntireTexture() = 0;

	D3DRESOURCETYPE					GetResType();
	
};


//
// Declare three derived classes, wrapper of Mip Texture, cube texture and vol texture
//
	
class CMipTex: public CBaseTex 
{
protected:
	// AndrewLu; 2003/10/13: Use instances of the CMipTex class to wrap instances of
	// the framework's LPTEXTURES pointers.
	LPTEXTURES				m_pTexture;

public:
									CMipTex(CUpdateTexBase *,CDevice*,D3DFORMAT , D3DPOOL ,UINT ,UINT, DWORD);
									~CMipTex();
									operator CBaseTexture* ();

	LPBASETEXTURES					GetLPTEXTURES();
	BOOL							ModifyDirtyRect(RECT *pRectDest);
									
	BOOL							SetTexValue (SURFACEVALUE );
	BOOL							ValidateTexture();
	BOOL							LockWithNULL(DWORD flag=0);
	BOOL							LockManyAreas(UINT);
	BOOL							LockSubLevel();
	BOOL							AddDirtyWithNULL();
	BOOL							AddDirtyManyAreas(UINT);
	BOOL							CopyRects(UINT,D3DPOOL);
	BOOL							MakeNonSquare();
	BOOL							LockWithoutUnlock();
	BOOL							Unlock();
	BOOL							SetSrcTex(CBaseTex *);
	HRESULT							Render(UINT uiMipLevel);
	void							DirtyEntireTexture();
};


class CCubeTex: public CBaseTex
{
protected:
	LPCUBETEXTURES			m_pTexture;
	static CUBEMAP_FACES			m_CubemapFaces[6];
	const UINT						m_TotalFaces;
	
public:
									CCubeTex(CUpdateTexBase *,CDevice*,D3DFORMAT , D3DPOOL ,UINT ,UINT, DWORD);
									~CCubeTex();
									operator CBaseTexture* ();

	LPBASETEXTURES					GetLPTEXTURES();

	BOOL							ModifyDirtyRect(RECT *pRectDest, CUBEMAP_FACES cmFace);
	BOOL							SetTexValue (SURFACEVALUE );
	BOOL							ValidateTexture();
	BOOL							LockWithNULL(DWORD flag=0);
	BOOL							LockManyAreas(UINT);
	BOOL							LockSubLevel();
	BOOL							AddDirtyWithNULL();
	BOOL							AddDirtyManyAreas(UINT);
	BOOL							CopyRects(UINT,D3DPOOL);
	BOOL							MakeNonSquare();
	BOOL							LockWithoutUnlock();
	BOOL							Unlock();
	BOOL							SetSrcTex(CBaseTex *);
	HRESULT							Render(UINT uiMipLevel);
	void							DirtyEntireTexture();
};

class CVolTex: public CBaseTex 
{
protected:
	
	LPVOLUMETEXTURES	m_pTexture;
	LPVOLUMETEXTURES	m_pSrcTexture;
	
public:
									CVolTex(CUpdateTexBase *,CDevice*,D3DFORMAT , D3DPOOL ,UINT ,UINT );
									~CVolTex();
									operator CBaseTexture* ();

	LPBASETEXTURES					GetLPTEXTURES();

	BOOL							SetTexValue (SURFACEVALUE );
	BOOL							ValidateTexture();
	BOOL							LockWithNULL(DWORD flag=0);
	BOOL							LockManyAreas(UINT);
	BOOL							LockSubLevel();
	BOOL							AddDirtyWithNULL();
	BOOL							AddDirtyManyAreas(UINT);
	BOOL							CopyRects(UINT,D3DPOOL);
	BOOL							MakeNonSquare();
	BOOL							LockWithoutUnlock();
	BOOL							Unlock();
	BOOL							SetSrcTex(CBaseTex *);
	HRESULT							Render(UINT uiMipLevel);
	void							DirtyEntireTexture();
};

//
//  The CMipMap specification
//

// Members common to multiple test groups.

class CUpdateTexBase : public CD3DTest
{
protected:
	CDevice*				m_pDevice;
	CD3D*							m_pD3Dxfu;
	D3DDEVTYPE						m_DeviceType;
	UINT							m_uGenericMipLevel;
	BOOL							m_bCanDoCubeMap;
	BOOL							m_bCanDoVolMap;
	BOOL							m_bCanDoMipmap;
	BOOL							m_bCanDoCubeMipmap;
	BOOL							m_bCanDoVolMipmap;
	UINT							m_uGenericCubeLevel;
	FORMAT							m_AdapterFormat;
	BOOL							m_SkipInTheMiddle;
	BOOL							m_bCanDoNonSquare;
	DEVICEDESC						m_caps;
	BOOL							m_bDebugMode;
	CBaseTex						*m_pSrcTex;
	CBaseTex						*m_pDestTex;
	CBaseTex						*m_pBogusTex;
	HWND							m_hWnd;

	BOOL							IsDebugMode();

	BOOL							CreateTex (D3DRESOURCETYPE,D3DFORMAT , D3DPOOL ,UINT ,UINT , DWORD, SURFACEVALUE,CBaseTex **,BOOL bInit=TRUE);

public:
	UINT uiWidthAdjustment, uiHeightAdjustment, uiCubeFaceIndex;
	RECT rectBB;
	CUpdateTexBase();
	~CUpdateTexBase();
    virtual UINT					TestInitialize();

private:

};

//
// Declare all groups here
//

// Members common to test groups for mip- and cube-texture test cases.

class UpdateTex:public CUpdateTexBase
{
public:

	FORMAT formatCurrent;

	static CONST UINT cuiLevelDisplaySeparation;
	static CONST UINT cuiFaceTopLeftDisplacement;

							UpdateTex();

	bool ExecuteTest
	(
		UINT nTest,
		UINT uiMipLevelIndex,
		UINT uiDimensionAdjustmentIndex,
		UINT uiDestUsageIndex,
		UINT uiSourceUsageIndex,
		UINT iTestCase
	);

	virtual bool			TestTerminate(void);

	virtual bool			Setup();
	virtual void			Cleanup();

	void					ProcessError(HRESULT hr);

	void					SetTextureStageStates(LPTEXTURES pTestTexture, UINT uiTextureLevel);

	HRESULT RenderTextureSurface(LPSURFACES pDestinationRT, LPTEXTURES pRenderTexture, UINT uiTextureLevel);

	HRESULT RenderEntireTexture(LPTEXTURES pRenderTexture);
	HRESULT RenderTextureLevelToRect(LPTEXTURES pRenderTexture, UINT uiTextureLevel, RECT rectDest);

	HRESULT RenderEntireFace(LPCUBETEXTURES pRenderTexture, CUBEMAP_FACES cmFace);
	HRESULT RenderFaceLevelToRect(LPCUBETEXTURES pRenderTexture, UINT uiTextureLevel, RECT rectDest, CUBEMAP_FACES cmFace);

#if FAST_TEST
    //Override the format modes to pick just a subset
    bool AddFormatModes(DISPLAYMODE *pMode, bool bWindowed);
#endif //FAST_TEST
	
	BOOL					DoNothing();
	BOOL					Case1();
	BOOL					Case2();
	BOOL					Case3();
	BOOL					Case4();
	BOOL					Case5();
	BOOL					Case6();
	BOOL					Case7();
	BOOL					Case7_2();
	BOOL					Case8();
	BOOL					Case9();
	BOOL					Case10();
	BOOL					Case11();
	BOOL					Case12();
	BOOL					Case13();
	BOOL					Case13_2();
	BOOL					Case13_3();
	BOOL					Case14();
	BOOL					Case14_2();
	BOOL					Case14_3();
	BOOL					CheckEntireTexOK();
	BOOL					CheckManySubTexOK();
	BOOL					CheckSubTexOK();
	BOOL					CheckEntireTexFail();
	
protected:
	BOOL					LoadMipTex();
	bool					InitializeAuxiliaryResources();
	bool					InitializeCubeMapRenderVBs();
	bool					InitializeBumpLuminanceSupport();
	void					ClearAuxiliaryResources();
	void					ClearCubeMapVBs();
	void					ClearBumpLuminanceSupport();

	// "Auxiliary" resources.

	// Vertex buffer for rendering mipmap levels.
	LPVERTEXBUFFERS			pvbAuxiliaryVertexBuffer;

	// Base and environment map textures for rendering bump map effects.
	CMipTex					*pBumpMapAuxBase;
	CMipTex					*pBumpMapAuxEnv;

	VIEWPORT				vpEntireBackBuffer;

	// AndrewLu: Ad hoc addition to add managed texture testing to the test.
	UINT					uiUpdateTextureTestCases;
	UINT					uiManagedTextureTestCases;
	UINT					uiTotalTestCases;
	bool					bTestUpdateTexture;	// Test cases for which this is 'false' will
												// apply to a single managed texture, rather
												// than to a pair of textures.

	UINT					iResType;

	CAPS					capsSrcDevice;

	typedef
	enum _RENDERPROCEDURE
	{
		rpMipMap,
		rpLuminance,
		rpBumpMap,
		rpBumpLuminance
	}
	RENDERPROCEDURE, *PRENDERPROCEDURE;

	RENDERPROCEDURE TestTextureRenderProcedure;

	DWORD MipFilteringType;
};

// Test group for conventional mipmap textures.
class UpdateTex_MipTextures: public UpdateTex
{
private:
public:
	UpdateTex_MipTextures();
	~UpdateTex_MipTextures();
	virtual bool CapsCheck();
	virtual	UINT TestInitialize();
	virtual bool ExecuteTest(UINT nTest);
};

// Test group for cube textures.
class UpdateTex_CubeTextures: public UpdateTex
{
private:
public:
	UpdateTex_CubeTextures();
	~UpdateTex_CubeTextures();
	virtual bool CapsCheck();
	virtual	UINT TestInitialize();
	virtual bool ExecuteTest(UINT nTest);
};

class Misc:public CUpdateTexBase
{

	
public:
							Misc();
	bool					ExecuteTest(UINT);
	virtual	UINT			TestInitialize();
};


// This test use MY_GROUP_BEGIN, MY_GROUP_END, MY_TEST_BEGIN, MY_TEST_END  instead of the normal GROUP_BEGIN....
// because the the user reuqested that each individual test can be run seperately. For example, they want
// to use "UpdateTexture -Update:52" to run one test. To achieve this, my CUpdateTexBase class has to dervied
// from CD3DTest instead of CMiniFrameWork. Those GROUP_XXX, TEST_XXX are CMiniFrameWork's macro. So I have
// to define my own macro like MY_XXX_XXX. Now, each test case in UpdateTex group can run seperately. There's
// one hole here. The Misc test group is not structured properly. All the test cases are run as one test.
// However, since Misc group is not run in DCT test, so it's mostly used internally, and doesn't matter too much.
// If in the future, each Misc test case need to be run seperately, it need to be written in the similar way
// as UpdateTex group.

#ifdef MY_GROUP_BEGIN
#undef MY_GROUP_BEGIN
#endif

#define MY_GROUP_BEGIN(classname,groupname,commandkey)\
    classname::classname()\
    {\
        m_szTestName = groupname;\
		m_szLogName = commandkey; \
	    m_szCommandKey = commandkey;\
    }\
    bool classname::ExecuteTest(UINT nTest)\
    {\
        ClearFrame();

#ifdef MY_GROUP_END
#undef MY_GROUP_END
#endif

#define MY_GROUP_END\
    		return false; \
	}


#define MY_TEST_BEGIN(testdesc, runcondition)  \
        BeginTestCase(testdesc); \
        if(runcondition) \
        {

#ifdef MY_TEST_END
#undef MY_TEST_END
#endif
#define MY_TEST_END \
		} \
		else \
			SetResult(SKIP); \
		EndTestCase(); \


#define MY_DEBUG_BUILD_TEST_BEGIN(a,b) if (m_bDebugMode) \
									{				 \
										MY_TEST_BEGIN(a,b)
#define MY_DEBUG_BUILD_TEST_END		MY_TEST_END	\
									}



extern const	UINT				GENERICTEXSIZE;
extern const	UINT				GENERICTEXLEVEL;

#endif
