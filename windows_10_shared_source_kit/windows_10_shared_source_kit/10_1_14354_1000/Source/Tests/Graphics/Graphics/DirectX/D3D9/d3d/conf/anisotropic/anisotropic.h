/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	anisotropic.h
 *
 ***************************************************************************/

#ifndef __ANISOTROPIC_H__
#define __ANISOTROPIC_H__

#include "CD3DTest.h"
#include "CTextures.h"

// Defines
#ifdef FAST_TEST
#define MAX_TESTS	1
#define MAX_GRIDS   2
#else
#define MAX_TESTS	6
#define MAX_GRIDS   4
#endif

// Base Class definitions
class CAnisoTest: public CD3DTest
{
    // Data
	private:
    D3DTLVERTEX     VertexList[6];
    DWORD           dwVertexCount;
	CnTexture *		pTexture;
	float			SizeArray[6];
	TCHAR			msgString[100];
	TCHAR			szStatus[100];

    // Data
	public:
	DWORD			dwMinFilterCap, dwMinFilter;
	DWORD			dwMagFilterCap, dwMagFilter;

	public:
	CAnisoTest();
	~CAnisoTest();

	// Framework functions
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual void UpdateStatus(void);
	virtual bool TestTerminate(void);

	// Helper functions
	void DrawGrid(float fMesh, int nMap);
};

// PointAniso Class definitions
class CPointAnisoTest: public CAnisoTest
{
	public:
	CPointAnisoTest();
};

// LinearAniso Class definitions
class CLinearAnisoTest: public CAnisoTest
{
	public:
	CLinearAnisoTest();
};

// AnisoPoint Class definitions
class CAnisoPointTest: public CAnisoTest
{
	public:
	CAnisoPointTest();
};

// AnisoLinear Class definitions
class CAnisoLinearTest: public CAnisoTest
{
	public:
	CAnisoLinearTest();
};

// AnisoAniso Class definitions
class CAnisoAnisoTest: public CAnisoTest
{
	public:
	CAnisoAnisoTest();
};

#endif
