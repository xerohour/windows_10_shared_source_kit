//////////////////////////////////////////////////////////////////////
// File:  LostDev.h
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for the D3D LostDev conformance test.
//
// History:
// ?? ??? ????  ChrisLav  Created.
//////////////////////////////////////////////////////////////////////

#ifndef _LOSTDEV_H_
#define _LOSTDEV_H_


/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

#define D3D_OVERLOADS

//#define USE_FUS

#include "CD3DTest.h"
#include "CasesManager.h"
#include "CFlagCombinations.h"
#include "CTestFactor.h"
#include "cd3d.h"
#include "d3d9.h"
#include <windows.h>
#include <vector>


/////////////////////////////////////////////////////////////////
//
// Type definitions
//
/////////////////////////////////////////////////////////////////

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw; // The transformed position for the vertex
	DWORD color;        // The vertex color
};

// structures for our textures cases
struct TEXTURE_USAGE
{
	float fMinDX;
	float fMaxDX;
	DWORD dwUsage;
	TCHAR szUsage[255];
};

struct TEXTURE_LOCKFLAG
{
	float fMinDX;
	float fMaxDX;
	DWORD dwLockFlag;
	TCHAR szLockFlag[255];
};

struct TEXTURE_FORMAT
{
	float fMinDX;
	float fMaxDX;
	FMT format;
	TCHAR szFormat[20];
};

struct TEXTURE_CASE
{
	DWORD dwUsage;
	TCHAR szUsage[255];
	DWORD dwLockFlag;
	TCHAR szLockFlag[255];
	FMT format;
	TCHAR szFormat[20];
	bool bUseCopyRects;
	bool bUseMipLevels;
	bool bUseSubRectLock;
};


int ExceptionFilter
(
	unsigned int ExceptionCode,
	_EXCEPTION_POINTERS * pExceptionDetails
);


/////////////////////////////////////////////////////////////////
//
// Constants
//
/////////////////////////////////////////////////////////////////

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

// A structure for our volume textures vertex type
struct VOLUMEVERTEX
{
	FLOAT x, y, z;		// The transformed position for the vertex
	DWORD color;		// The vertex color
	FLOAT tu, tv, tw;	// The texture coordinates
};

#define D3DFVF_VOLUMEVERTEX		( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 |D3DFVF_TEXCOORDSIZE3(0))

//Pools
#define TEST_SYSTEMMEM      0x00000001
#define TEST_MANAGED        0x00000002
#define TEST_DEFAULT        0x00000004
#define TEST_SCRATCH        0x00000008
#define TEST_ALLPOOLS       0x0000000F

//Flags
const UINT FLAG_READONLY                = 0x00010000;
const UINT FLAG_READWRITE               = 0x00020000;
const UINT FLAG_ALLREADFLAGS            = FLAG_READONLY | FLAG_READWRITE;
const UINT FLAG_ALLFLAGS                = FLAG_ALLREADFLAGS;

//Ways to lose the device
const UINT LOST_CHANGEDISPLAY           = 0x00000001;
const UINT LOST_FUS                     = 0x00000002;
const UINT LOST_CDSBYOTHERPROCESS       = 0x00000004;
const UINT LOST_STANDBY                 = 0x00000010;
const UINT LOST_HIBERNATE               = 0x00000020;
const UINT LOST_ALLLOSTWAYS             = 0x00000037;
const UINT NBLOSTWAYS                   = 5;

//Test scenarios
const UINT TEST_CREATIONWHILELOST               = 0x00000001;
const UINT TEST_CREATIONWHILELOSTFILLAFTER      = 0x00000002;
const UINT TEST_LOCKWHILELOST                   = 0x00000004;
const UINT TEST_LOSTWHILELOCKED                 = 0x00000008;
const UINT TEST_LOSTWHILERENDERING              = 0x00000010;

const UINT TEST_CREATEAANDBTHENLOSE             = 0x00010000;
const UINT TEST_CREATEALOSECREATEB              = 0x00020000;
const UINT TEST_LOSETHENCREATEAANDB             = 0x00040000;

const UINT TEST_FUSCREATEFUSBACK                = 0x00100000;
const UINT TEST_CREATEFUSDELETEFUSBACK          = 0x00200000;

const UINT TEST_DEFAULTPOOLTEST                 = 0x01000000;
const UINT TEST_RELEASEAFTERRESETFAILED         = 0x02000000;
const UINT TEST_CREATEAANDBTHENSUSPENDSYSTEM    = 0x04000000;

const UINT TEST_ALLCASES                        = 0x0737001F;


const UINT NB1RESSCENARIO = 5;
const UINT NB2RESSCENARIO = 3;


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CLostDev
//
// Purpose:
//
///////////////////////////////////////////////////////
class CLostDev
:
	public CD3DTest
{
	protected:
		PRESENT_PARAMETERS m_PresParams;

		DWORD			m_Pool;
		LPSURFACES		m_pBackBufferCopy;

		LPTSTR			m_szPoolType;
		LPTSTR			m_TestName;
		LPTSTR			m_szSubTest;
		char			m_szFullName[255];

		UINT			m_uTestType;

		bool			m_bTestSystemMem;
		bool			m_bTestManaged;
		bool			m_bTestDefault;
		bool			m_bTestScratch;

		//Command Line options to drive the tests
		UINT			m_uPoolsToTest;
		UINT			m_uCases;
		//UINT			m_uFlags;
		bool			m_bSkipGroup;

		bool			m_bLockReadOnly;
		bool			m_bTestLockReadOnly;

		// Wait 2 seconds after each frame.
		bool			m_bSeeResult;

		// Wait 2 seconds after failures.
		bool			m_bSeeFailures;

		//Ways to Lose the device
		LPTSTR			m_szLostType;
		UINT			m_uLostType;
		UINT			m_uLostFlags;

		RFactor			m_rCasesManager;

		RFactor			m_rPoolManaged;
		RFactor			m_rPoolManaged1Res;
		RFactor			m_rPoolManaged2Res;
		RFactor			m_rPoolSystemMem;
		RFactor			m_rPoolSystemMem1Res;
		RFactor			m_rPoolSystemMem2Res;
		RFactor			m_rPoolScratch;
		RFactor			m_rPoolScratch1Res;
		RFactor			m_rPoolScratch2Res;
		RFactor			m_rPoolDefault;

		RFactor			m_rLostWays;

		CCasesList		m_casesTestType1Res;
		CCasesList		m_casesTestType2Res;
		CCasesList		m_casesTestTypeDefault;
		//CCasesList		m_casesTestTypeScratch;
		CCasesList		m_casesPools;
		CCasesList		m_casesLostWays;

		void ValidateTestType();
		void ValidatePools();
		void ValidateLostWays();
		bool CreateTestFactors();


		//Utility functions
		bool AreTherePossibleGDILeaks();
		bool CopyFromAToB(LOCKED_RECT *pLockRectA, LOCKED_RECT *pLockRectB, SURFACEDESC	*pDescA);

		// Method to lose the device
	//	bool ChangeDisplayMode();
	//	bool ChangeDisplayModeByOtherProcess();
		bool FastUserSwitch();

		// The different tests
		virtual bool NormalResult();
		virtual bool CreationWhileLost();
		virtual bool CreationWhileLostFillAfter();
		virtual bool LockWhileLost();
		virtual bool LockWhileLostLOOP();
		virtual bool LostWhileLocked();
		virtual bool LostWhileRendering();
		virtual bool DefaultPoolTest();
		virtual bool CreateAAndBThenLose();
		virtual bool CreateAThenLoseThenCreateB();
		virtual bool LoseThenCreateAAndB();
		virtual bool FUSCreateFUSBack();
		virtual bool CreateFUSDeleteFUSBack();
		virtual bool ReleaseAfterResetFailed();
		virtual bool CreateAAndBThenSuspendSystem();

		// Methods used by the tests
		bool LoseDevice();
		bool ResetDevice(ULONG lDev = CD3D_SRC);
		bool SaveBackBuffer();
		bool CompareBackBuffers();
		bool SetTestCase(UINT uTestNumber);
		void SetTestsToRun();

		FMT GetValidFormat(RESOURCETYPE rtype);

		// Methods that needs to be overriden in each tests
		virtual bool CreateResource(ULONG lDev = CD3D_SRC) = 0;
		virtual bool CreateResourceB(ULONG lDev = CD3D_SRC) { return true; };
		virtual bool LockData(ULONG lDev = CD3D_SRC) = 0;
		virtual bool FillAndUnlockData(ULONG lDev = CD3D_SRC) = 0;
		virtual bool CopyResAToResB(ULONG lDev = CD3D_SRC) { return true; };
		virtual bool DrawResource(ULONG lDev = CD3D_SRC) = 0;
		virtual bool DrawResourceB(ULONG lDev = CD3D_SRC) { return true; };
		virtual bool ReleaseResource(ULONG lDev = CD3D_SRC) = 0;
		virtual void SetFlags() {};
		virtual void SetTestFactors();
		virtual void DumpTestDetails();

		bool FillSurface(SURFACEDESC *pDesc, LOCKED_RECT *pLockedRect, bool bUseSubRectLock);

		void FailTest();

	public: // class functions
		CLostDev();
		~CLostDev();

		// Framework overloads
		virtual UINT TestInitialize(void);
		virtual bool ExecuteTest(UINT);
		virtual bool CapsCheck();
		virtual void EndTestCase(void);
		virtual bool TestTerminate(void);
		bool HandleDeviceLost() { return true; };
		void CommandLineHelp(void);
		void ReleaseTest();
		HRESULT	ValidateDevices(DEVICEMODE *pMode);
};


///////////////////////////////////////////////////////
// Class:  CLostVertexBuffer
//
// Purpose:
//
///////////////////////////////////////////////////////
class CLostVertexBuffer
:
	public CLostDev
{
	protected:
		LPVERTEXBUFFERS m_pVBA;
		LPVERTEXBUFFERS m_pVBB;
		CUSTOMVERTEX	m_Vertices[3];
		VOID			*m_pVertices;

		LPINDEXBUFFERS  m_pIBA;
		LPINDEXBUFFERS  m_pIBB;
		WORD			m_Indices[3];
		VOID			*m_pIndices;

		LPTEXTURES		m_pTexture;

		bool			m_bUseIndexBuffer;
		bool			m_bWriteOnly;

		CFlagCombinations	m_flagsUsage;
		CFlagCombinations	m_flagsUsageNoneLock;
		CFlagCombinations	m_flagsUsageDynamicLock;
		CFlagCombinations	m_flagsUsageWriteonlyLock;
		CFlagCombinations	m_flagsUsageDynamicWriteonlyLock;

		RFactor				m_rUsageNoneLock;
		RFactor				m_rUsageDynamicLock;
		RFactor				m_rUsageWriteonlyLock;
		RFactor				m_rUsageDynamicWriteonlyLock;

		RFactor				m_rIndexBuffer;

		void SetFlags();
		void SetTestFactors();
		void DumpTestDetails();

		char	m_szLockFlags[120];
		char	m_szUsageFlags[120];

		DWORD	m_dwUsageFlags;
		DWORD	m_dwLockFlags;

		bool CreateResource(LPVERTEXBUFFERS *pVB, LPINDEXBUFFERS *pIB, ULONG lDev = CD3D_SRC);
		bool DrawResource(LPVERTEXBUFFERS pVB, LPINDEXBUFFERS pIB, ULONG lDev = CD3D_SRC);

		bool CreateResource(ULONG lDev = CD3D_SRC);
		bool LockData(ULONG lDev = CD3D_SRC);
		bool FillAndUnlockData(ULONG lDev = CD3D_SRC);
		bool DrawResource(ULONG lDev = CD3D_SRC);
		bool ReleaseResource(ULONG lDev = CD3D_SRC);

		bool CreateResourceB(ULONG lDev = CD3D_SRC);
		bool CopyResAToResB(ULONG lDev = CD3D_SRC);
		bool DrawResourceB(ULONG lDev = CD3D_SRC);

	public:
        
		CLostVertexBuffer();
		UINT TestInitialize();
		bool TestTerminate(void);
		bool CapsCheck();
};


///////////////////////////////////////////////////////
// Class:  CLostDev
//
// Purpose:
//
///////////////////////////////////////////////////////
class CLostBaseTexture
:
	public CLostDev
{
	protected:
		bool			m_bUseCopyRects;		//Used to determine if we use (CopyRects or UpdateTexture for DrawResource) and (CopyRects or memcpy for CopyResAToResB) 
		bool			m_bUseMipMaps;			//Used to determine if we use MipMaps on the texture
		bool			m_bUseSubRectLock;		//Used to determine if we use a SubRect for the lock
		DWORD			m_dwUsage;
		DWORD			m_dwLockFlags;
		UINT			m_nResForCase;			//Number of resources for this case (1 or 2)

		FMT				m_TexFormat;
		bool			m_bMipMapSupported;

		TEXTURE_CASE *	m_pCurrentTexCase;

		std::vector <TEXTURE_CASE *> m_listCasesPool[4];
		std::vector <TEXTURE_CASE *> m_listCasesDefault;
		std::vector <TEXTURE_CASE *> m_listCasesSystemMem1Res;
		std::vector <TEXTURE_CASE *> m_listCasesSystemMem2Res;
		std::vector <TEXTURE_CASE *> m_listCasesManaged1Res;
		std::vector <TEXTURE_CASE *> m_listCasesManaged2Res;
		std::vector <TEXTURE_CASE *> m_listCasesScratch1Res;
		std::vector <TEXTURE_CASE *> m_listCasesScratch2Res;

		RFactor			m_rCasesDefault;
		RFactor			m_rCasesManaged1Res;
		RFactor			m_rCasesManaged2Res;
		RFactor			m_rCasesSystemMem1Res;
		RFactor			m_rCasesSystemMem2Res;
		RFactor			m_rCasesScratch1Res;
		RFactor			m_rCasesScratch2Res;

		UINT			m_nMipLevels;

		void SetFlags();
		void SetTestFactors();
		void DumpTestDetails();

		void GenerateCaseList();
	public:
        
		virtual bool CapsCheck();
		virtual bool TestTerminate();
};


///////////////////////////////////////////////////////
// Class:  CLostTexture
//
// Purpose:
//
///////////////////////////////////////////////////////
class CLostTexture
:
	public CLostBaseTexture
{
	protected:
		LPTEXTURES		m_pTexA;
		LPTEXTURES		m_pTexB;

		LOCKED_RECT		m_LockedRect[9];

		bool CreateResource(LPTEXTURES &pTex, ULONG lDev = CD3D_SRC);
		bool DrawResource(LPTEXTURES pTex, ULONG lDev = CD3D_SRC);

		bool RenderToBackbuffer(LPTEXTURES pTex, ULONG lDev = CD3D_SRC);

		bool CreateResource(ULONG lDev = CD3D_SRC);
		bool LockData(ULONG lDev = CD3D_SRC);
		bool FillAndUnlockData(ULONG lDev = CD3D_SRC);
		bool DrawResource(ULONG lDev = CD3D_SRC);
		bool ReleaseResource(ULONG lDev = CD3D_SRC);

		bool CreateResourceB(ULONG lDev = CD3D_SRC);
		bool CopyResAToResB(ULONG lDev = CD3D_SRC);
		bool DrawResourceB(ULONG lDev = CD3D_SRC);

	public:
		CLostTexture();

		bool CapsCheck();
};


///////////////////////////////////////////////////////
// Class:  CLostVolumeTexture
//
// Purpose:
//
///////////////////////////////////////////////////////
class CLostVolumeTexture
:
	public CLostDev
{
	protected:
		LPVOLUMETEXTURES	m_pVolumeTexA;
		LPVOLUMETEXTURES    m_pVolumeTexB;

		VOLUMEVERTEX		m_Vertices[4];
		VOID				*m_pVertices;

		LOCKED_BOX			m_LockedBox;

		FMT					m_TexFormat;
		bool				m_bMipMapSupported;

		bool CreateResource(ULONG lDev = CD3D_SRC);
		bool LockData(ULONG lDev = CD3D_SRC);
		bool FillAndUnlockData(ULONG lDev = CD3D_SRC);
		bool DrawResource(ULONG lDev = CD3D_SRC);
		bool ReleaseResource(ULONG lDev = CD3D_SRC);
		//t-blakep, added support for **B functions
		bool CopyResAToResB(ULONG lDev = CD3D_SRC);
		bool CreateResourceB(ULONG lDev = CD3D_SRC);
		bool DrawResourceB(ULONG lDev = CD3D_SRC);

		bool CreateResource(LPVOLUMETEXTURES& pTexture, ULONG lDev = CD3D_SRC);
		bool DrawResource(LPVOLUMETEXTURES pTexture, ULONG lDev = CD3D_SRC);

		void SetTestFactors();//t-blakep no pool_default textures allowed because we can't copy volume textures in pool_default


	public:
        
		CLostVolumeTexture();
		UINT TestInitialize();
		bool CapsCheck();
};


///////////////////////////////////////////////////////
// Class:  CLostCubeTexture
//
// Purpose:
//
///////////////////////////////////////////////////////
class CLostCubeTexture
:
	public CLostBaseTexture
{
	protected:
		LPCUBETEXTURES		m_pCubeTexA;
		LPCUBETEXTURES		m_pCubeTexB;

		LPVERTEXBUFFERS		m_pVB;
		static VOLUMEVERTEX	m_Vertices[18];
		VOID				*m_pVertices;

		LOCKED_RECT			m_LockedRect[6][5];

		bool CreateResource(LPCUBETEXTURES &pTex, ULONG lDev = CD3D_SRC);
		bool DrawResource(LPCUBETEXTURES pTex, ULONG lDev = CD3D_SRC);

		bool CreateResource(ULONG lDev = CD3D_SRC);
		bool LockData(ULONG lDev = CD3D_SRC);
		bool FillAndUnlockData(ULONG lDev = CD3D_SRC);
		bool DrawResource(ULONG lDev = CD3D_SRC);
		bool ReleaseResource(ULONG lDev = CD3D_SRC);

		bool CreateResourceB(ULONG lDev = CD3D_SRC);
		bool CopyResAToResB(ULONG lDev = CD3D_SRC);
		bool DrawResourceB(ULONG lDev = CD3D_SRC);


	public:
		CLostCubeTexture();
		UINT TestInitialize();
		bool CapsCheck();
};


///////////////////////////////////////////////////////
// Class:  CLostSurface
//
// Purpose:
//
///////////////////////////////////////////////////////
class CLostSurface
:
	public CLostDev
{
	protected:
		LPSURFACES		m_pSurfacesA;
		LPSURFACES		m_pSurfacesB;
		LPSURFACE		m_pSurfA;
		LPSURFACE		m_pSurfB;

		LOCKED_RECT		m_LockedRect;

		bool			m_bUseAToBCopyRects;	//Used to determine if we use CopyRects or memcpy for CopyResAToResB
		bool			m_bUseSubRectLock;		//Used to determine if we use a SubRect for the lock

		FMT				m_TexFormat;

		RFactor			m_rCopyFromAToB;		//Used to determine if we use CopyRects or memcpy for CopyResAToResB
		RFactor			m_rSubRectLock;			//Used to determine if we use a SubRect for the lock

		void SetFlags();
		virtual void SetTestFactors() = 0;
		void DumpTestDetails();

		virtual bool CreateResource(LPSURFACES &pSurfaces, LPSURFACE &pSurf, ULONG lDev = CD3D_SRC) = 0;
		bool DrawResource(LPSURFACES pSurfaces, LPSURFACE pSurf, ULONG lDev = CD3D_SRC);

		bool CreateResource(ULONG lDev = CD3D_SRC);
		bool LockData(ULONG lDev = CD3D_SRC);
		bool FillAndUnlockData(ULONG lDev = CD3D_SRC);
		bool DrawResource(ULONG lDev = CD3D_SRC);
		bool ReleaseResource(ULONG lDev = CD3D_SRC);

		bool CreateResourceB(ULONG lDev = CD3D_SRC);
		bool CopyResAToResB(ULONG lDev = CD3D_SRC);
		bool DrawResourceB(ULONG lDev = CD3D_SRC);


	public:
		CLostSurface();

		bool CapsCheck();
};


///////////////////////////////////////////////////////
// Class:  CLostImageSurface
//
// Purpose:
//
///////////////////////////////////////////////////////
class CLostImageSurface
:
	public CLostSurface
{
	protected:
		void SetTestFactors();
		bool CreateResource(LPSURFACES &pSurfaces, LPSURFACE &pSurf, ULONG lDev = CD3D_SRC);

	public:
		CLostImageSurface();
		bool CapsCheck();
};


///////////////////////////////////////////////////////
// Class:  CLostOffscreenPlainSurface
//
// Purpose:
//
///////////////////////////////////////////////////////
class CLostOffscreenPlainSurface
:
	public CLostSurface
{
	protected:
		void SetTestFactors();
		bool CreateResource(LPSURFACES &pSurfaces, LPSURFACE &pSurf, ULONG lDev = CD3D_SRC);

	public:
		CLostOffscreenPlainSurface();
		bool CapsCheck();
};


///////////////////////////////////////////////////////
// Class:  CLostDepthStencil
//
// Purpose:
//
///////////////////////////////////////////////////////
class CLostDepthStencil
:
	public CLostDev
{
	protected:
		LPSURFACES			m_pDepth;
		FMT					m_DSFormat;

		void SetTestFactors();

		bool CreateResource(ULONG lDev = CD3D_SRC);
		bool LockData(ULONG lDev = CD3D_SRC);
		bool FillAndUnlockData(ULONG lDev = CD3D_SRC);
		bool DrawResource(ULONG lDev = CD3D_SRC);
		bool ReleaseResource(ULONG lDev = CD3D_SRC);


	public:
		CLostDepthStencil();

		bool CapsCheck();
};


///////////////////////////////////////////////////////
// Class:  CLostRenderTarget
//
// Purpose:
//
///////////////////////////////////////////////////////
class CLostRenderTarget
:
	public CLostDev
{
	protected:
		LPSURFACES			m_pRender;

		void SetTestFactors();

		bool CreateResource(ULONG lDev = CD3D_SRC);
		bool LockData(ULONG lDev = CD3D_SRC);
		bool FillAndUnlockData(ULONG lDev = CD3D_SRC);
		bool DrawResource(ULONG lDev = CD3D_SRC);
		bool ReleaseResource(ULONG lDev = CD3D_SRC);


	public:
		CLostRenderTarget();

		bool CapsCheck();
};


///////////////////////////////////////////////////////
// Class:  CLostStateBlock
//
// Purpose:
//
///////////////////////////////////////////////////////
class CLostStateBlock
:
	public CLostDev
{
	protected:
		DWORD		m_dwStateBlock;

		void SetTestFactors();

		bool CreateResource(ULONG lDev = CD3D_SRC);
		bool LockData(ULONG lDev = CD3D_SRC);
		bool FillAndUnlockData(ULONG lDev = CD3D_SRC);
		bool DrawResource(ULONG lDev = CD3D_SRC);
		bool ReleaseResource(ULONG lDev = CD3D_SRC);

		bool CreateResourceB(ULONG lDev = CD3D_SRC);
		bool CopyResAToResB(ULONG lDev = CD3D_SRC);
		bool DrawResourceB(ULONG lDev = CD3D_SRC);


	public:
		CLostStateBlock();	
};


///////////////////////////////////////////////////////
// Class:  CLostVertexShader
//
// Purpose:
//
///////////////////////////////////////////////////////
class CLostVertexShader
:
	public CLostDev
{
	protected:
		CnVertexShader*		m_pVShader;

		void SetTestFactors();

		bool CreateResource(ULONG lDev = CD3D_SRC);
		bool LockData(ULONG lDev = CD3D_SRC);
		bool FillAndUnlockData(ULONG lDev = CD3D_SRC);
		bool DrawResource(ULONG lDev = CD3D_SRC);
		bool ReleaseResource(ULONG lDev = CD3D_SRC);

		bool CreateResourceB(ULONG lDev = CD3D_SRC);
		bool CopyResAToResB(ULONG lDev = CD3D_SRC);
		bool DrawResourceB(ULONG lDev = CD3D_SRC);


	public:
		CLostVertexShader();	
};


///////////////////////////////////////////////////////
// Class:  CLostPixelShader
//
// Purpose:
//
///////////////////////////////////////////////////////
class CLostPixelShader
:
	public CLostDev
{
	protected:
		CnPixelShader*		m_pPShader;

		void SetTestFactors();

		bool CreateResource(ULONG lDev = CD3D_SRC);
		bool LockData(ULONG lDev = CD3D_SRC);
		bool FillAndUnlockData(ULONG lDev = CD3D_SRC);
		bool DrawResource(ULONG lDev = CD3D_SRC);
		bool ReleaseResource(ULONG lDev = CD3D_SRC);

		bool CreateResourceB(ULONG lDev = CD3D_SRC);
		bool CopyResAToResB(ULONG lDev = CD3D_SRC);
		bool DrawResourceB(ULONG lDev = CD3D_SRC);


	public:
		CLostPixelShader();	
};

#endif
