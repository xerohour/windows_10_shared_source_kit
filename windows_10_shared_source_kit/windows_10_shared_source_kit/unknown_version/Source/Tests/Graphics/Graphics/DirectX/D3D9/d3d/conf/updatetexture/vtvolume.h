/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	Volume.h
 *
 ***************************************************************************/

#ifndef __VTVOLUME_H__
#define __VTVOLUME_H__

#include "CD3DTest.h"

#include "CTextures.h"

#include <d3d9.h>
#include <assert.h>

// AndrewLu:
#define D3DTSS_MIPMAPLODBIAS           19
#define D3DTSS_MIPFILTER               18

const int MAX_TESTS = 50;
const int MAX_MIP_TESTS = 100;



struct TVertex {									// Cubemap vertex with 3D texture coords
	D3DVECTOR v;
	D3DVECTOR n;
	D3DVECTOR t;
};

struct TGVertex {									// Cubemap vertex without 3D texture coords
	D3DVECTOR v;
	D3DVECTOR n;
};


// Class definition
class CVolumeTest : public CD3DTest
{
    // Data
	public:

	TVertex *       pTVertex;							// Sphere vertices with texture vectors
	int             nVertexMax;
    DWORD           dwVertexCount;

	CnVolTexture * pVolTexture;
	CnVolTexture * pSysTex;
	CnVolTexture * pBogusTex;

	// AndrewLu: Try to exploit new framework's "Cn compound textures".
	CnVolTexture *	m_pSrcTex;
	CnVolTexture *	m_pDestTex;

    TCHAR	        msgString[255];
	TCHAR			szStatus[255];
	BOOL			m_bMipVolTex;
	BOOL			m_bSkipInTheMiddle;
	UINT			m_nTotalLevels;

	public:
	CVolumeTest();
	~CVolumeTest();

#if FAST_TEST
    bool AddFormatModes(DISPLAYMODE *pMode, bool bWindowed);
#endif

	// Framework functions
    virtual bool SetDefaultMaterials();
    virtual bool SetDefaultRenderStates();
    virtual bool SetDefaultMatrices();
    virtual void SceneRefresh(void);
    virtual bool TestTerminate(void);

    // Helper functions
    void LoadSphere(void);
    void UpdateStatus(bool);
};


// MipMap Class definitions
class CMipVolTest: public CVolumeTest
{
    protected:

	static bool IsSubString(LPTSTR szSearchString, LPTSTR szSourceString);

	public:

	FORMAT formatCurrent;

	CMipVolTest();
	~CMipVolTest();

	BOOL						DoNothing();
	BOOL						Case1();
	BOOL						Case2();
	BOOL						Case3();
	BOOL						Case4();
	BOOL						Case5();
	BOOL						Case6();
	BOOL						Case7();
	BOOL						Case7_2();
	BOOL						Case8();
	BOOL						Case9();

	BOOL						ModifySysMemBoxes(CnVolTexture * pTex, BOX *pBox);

	BOOL						LockWithNULL(CnVolTexture * ,DWORD flag=0);
	BOOL						LockManyAreas(CnVolTexture * ,UINT );
	BOOL						AddDirtyWithNULL(CnVolTexture *);
	BOOL						AddDirtyManyAreas(CnVolTexture * ,UINT );
	BOOL						LockSubLevel (CnVolTexture *);

	virtual bool CapsCheck();

	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);
	BOOL IsDXTnFormat (D3DFORMAT);

	bool						VTCreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,FORMAT Format,DWORD Pool,CnVolTexture ** ppVolumeTexture, bool bStripes, DWORD dwFlags = CD3D_ALL);
};


typedef struct _VTTESTCASE 
{
	LPCTSTR						szTitle;
	UINT						uSrcSize;
	UINT						uSrcLevels;
	UINT						uDestSize;
	UINT						uDestLevels;
	BOOL						(CMipVolTest::*PreUpdateFunc)();
	BOOL						bExpectSucceed;
} VTTESTCASE, *PVTTESTCASE;


extern const UINT VTGENERICTEXSIZE;
extern const UINT VTGENERICTEXLEVEL;
extern const BOX VTCMULTIBOXES[];
extern const BOX VTCDXTnMULTIBOXES[];
extern const UINT VTTOTALBOXES;
extern const VTTESTCASE tc[];
extern const UINT VTTOTALCASES;

#endif
