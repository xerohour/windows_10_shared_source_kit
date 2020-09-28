/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	compress.h
 *
 ***************************************************************************/

#ifndef __COMPRESS_H__
#define __COMPRESS_H__

#include "CD3DTest.h"
#include "CTextures.h"

// Defines
#define MAX_TESTS		18

// Class definition
class CCompressTest: public CD3DTest
{
    // Data
	private:
    D3DTLVERTEX     VertexList[6];
    DWORD           dwVertexCount;
	CnTexture *		pTexture;
	CnTexture *		pDXTnTexture;
	CnTexture *		pSysDXTnTexture;
	D3DCOLOR		Stripes[8];
	D3DCOLOR		AlphaStripes[8];
	D3DCOLOR		AlphaWhite[4];
	DWORD			dwPerspectiveCap;
	DWORD			dwSrcAlphaCap;
	DWORD			dwOneCap;
	DWORD			dwInvSrcAlphaCap;
	TCHAR			msgString[80];
	TCHAR			szStatus[80];

    // Data
	public:
	FMT             FourCC;

	public:
	CCompressTest();
	~CCompressTest();

	// Framework functions
	virtual bool SetDefaultRenderStates(void);
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
    virtual void UpdateStatus(void);
	virtual bool TestTerminate(void);

	// Helper functions
	bool CheckDXTnFormat(DWORD dwDXTn);
	void DrawBasicGrid(int nTest, int nTexFormats);
	void DrawBlendingGrid(int nAlpha);
};

// DXT1 Class definitions
class CDXT1Test: public CCompressTest
{
	public:
	CDXT1Test();
	~CDXT1Test();
};

// DXT2 Class definitions
class CDXT2Test: public CCompressTest
{
	public:
	CDXT2Test();
	~CDXT2Test();
};

// DXT3 Class definitions
class CDXT3Test: public CCompressTest
{
	public:
	CDXT3Test();
	~CDXT3Test();
};

// DXT4 Class definitions
class CDXT4Test: public CCompressTest
{
	public:
	CDXT4Test();
	~CDXT4Test();
};

// DXT5 Class definitions
class CDXT5Test: public CCompressTest
{
	public:
	CDXT5Test();
	~CDXT5Test();
};

#endif

