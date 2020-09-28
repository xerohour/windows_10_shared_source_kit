/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	ColorKey.h
 *
 ***************************************************************************/

#ifndef __COLORKEY_H__
#define __COLORKEY_H__

#include "CD3DTest.h"
#include "CTextures.h"

// Defines
#define MAX_TESTS		8

// Class definition
class CColorKeyTest: public CD3DTest
{
    // Data
	private:
    D3DTLVERTEX     VertexList[6];
    DWORD           dwVertexCount;
	CnTexture *		pTexture;
	TCHAR			szStatus[80];
	TCHAR			msgString[80];

    // Data
    public:
	D3DCOLOR		Stripes[8];
	DWORD			dwColorKey;
	bool			bDefault;

	public:
	CColorKeyTest();
	~CColorKeyTest();

	// Framework functions
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
    virtual void UpdateStatus(void);
	virtual bool TestTerminate(void);

	// Helper functions
	void DrawGrid(int nTest);
};

// Default Class definitions
class CDefaultTest: public CColorKeyTest
{
	public:
	CDefaultTest();
	~CDefaultTest();
};

// White Class definitions
class CWhiteTest: public CColorKeyTest
{
	public:
	CWhiteTest();
	~CWhiteTest();
};

// Red Class definitions
class CRedTest: public CColorKeyTest
{
	public:
	CRedTest();
	~CRedTest();
};

// Green Class definitions
class CGreenTest: public CColorKeyTest
{
	public:
	CGreenTest();
	~CGreenTest();
};

// Blue Class definitions
class CBlueTest: public CColorKeyTest
{
	public:
	CBlueTest();
	~CBlueTest();
};

// Yellow Class definitions
class CYellowTest: public CColorKeyTest
{
	public:
	CYellowTest();
	~CYellowTest();
};

// Magenta Class definitions
class CMagentaTest: public CColorKeyTest
{
	public:
	CMagentaTest();
	~CMagentaTest();
};

// Cyan Class definitions
class CCyanTest: public CColorKeyTest
{
	public:
	CCyanTest();
	~CCyanTest();
};

// Grey Class definitions
class CGreyTest: public CColorKeyTest
{
	public:
	CGreyTest();
	~CGreyTest();
};

// Black Class definitions
class CBlackTest: public CColorKeyTest
{
	public:
	CBlackTest();
	~CBlackTest();

	UINT TestInitialize(void);
	bool ClearFrame(void);
};

#endif
