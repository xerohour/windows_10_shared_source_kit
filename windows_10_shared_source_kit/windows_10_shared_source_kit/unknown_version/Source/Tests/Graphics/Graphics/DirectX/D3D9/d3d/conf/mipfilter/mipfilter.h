/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	MipFilter.h
 *
 ***************************************************************************/

#ifndef __MIPFILTER_H__
#define __MIPFILTER_H__

#include "CD3DTest.h"
#include "CTextures.h"

// Defines
#define MAX_TESTS	9

// Base Class definitions
class CMipFilterTest: public CD3DTest
{
    // Data
    private:
    D3DTLVERTEX     VertexList[6];
    DWORD           dwVertexCount;
	CnTexture *		pTexture;
	CImage *	    pMipImage[9];
	float			SizeArray[18];

    // Data
	public:
	bool			bNonSquareW;
	bool			bNonSquareH;
	bool			bMipLinear;
	bool			bMinLinear;
	bool			bMipNone;
	bool			bBias;
	bool			bFull;
	bool			bMax;
    DWORD           dwLoadOption;
	TCHAR			msgString[80];
	TCHAR			szStatus[80];

	public:
	CMipFilterTest();
	~CMipFilterTest();

	// Framework functions
	virtual bool SetDefaultRenderStates(void);
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
    virtual void UpdateStatus(void);
	virtual bool TestTerminate(void);

	// Helper functions
	void InitSize(void);
	void InitMipImage(DWORD dwAspectRatio);
	void DrawGrid(float fWidth, float fHeight);

#if FAST_TEST
    UINT m_totalNumTestCases;
#endif
};

// Point_MinPoint Class definitions
class CPoint_MinPointTest: public CMipFilterTest
{
	public:
	CPoint_MinPointTest();
};

// Point_MinLinear Class definitions
class CPoint_MinLinearTest: public CMipFilterTest
{
	public:
	CPoint_MinLinearTest();
};

// Point_NonSquareW Class definitions
class CPoint_NonSquareWTest: public CMipFilterTest
{
	public:
	CPoint_NonSquareWTest();
};

// Point_NonSquareH Class definitions
class CPoint_NonSquareHTest: public CMipFilterTest
{
	public:
	CPoint_NonSquareHTest();
};

// Point_FullSquareUT Class definitions
class CPoint_FullSquareUTTest: public CMipFilterTest
{
	public:
	CPoint_FullSquareUTTest();
};

// Point_FullSquareCR Class definitions
class CPoint_FullSquareCRTest: public CMipFilterTest
{
	public:
	CPoint_FullSquareCRTest();
};

// Point_FullSquareLC Class definitions
class CPoint_FullSquareLCTest: public CMipFilterTest
{
	public:
	CPoint_FullSquareLCTest();
};

// Point_FullSquareUS Class definitions
class CPoint_FullSquareUSTest: public CMipFilterTest
{
	public:
	CPoint_FullSquareUSTest();
};

// Point_FullNonSquareWUT Class definitions
class CPoint_FullNonSquareWUTTest: public CMipFilterTest
{
	public:
	CPoint_FullNonSquareWUTTest();
};

// Point_FullNonSquareWCR Class definitions
class CPoint_FullNonSquareWCRTest: public CMipFilterTest
{
	public:
	CPoint_FullNonSquareWCRTest();
};

// Point_FullNonSquareWLC Class definitions
class CPoint_FullNonSquareWLCTest: public CMipFilterTest
{
	public:
	CPoint_FullNonSquareWLCTest();
};

// Point_FullNonSquareWUS Class definitions
class CPoint_FullNonSquareWUSTest: public CMipFilterTest
{
	public:
	CPoint_FullNonSquareWUSTest();
};

// Point_FullNonSquareHUT Class definitions
class CPoint_FullNonSquareHUTTest: public CMipFilterTest
{
	public:
	CPoint_FullNonSquareHUTTest();
};

// Point_FullNonSquareHCR Class definitions
class CPoint_FullNonSquareHCRTest: public CMipFilterTest
{
	public:
	CPoint_FullNonSquareHCRTest();
};

// Point_FullNonSquareHLC Class definitions
class CPoint_FullNonSquareHLCTest: public CMipFilterTest
{
	public:
	CPoint_FullNonSquareHLCTest();
};

// Point_FullNonSquareHUS Class definitions
class CPoint_FullNonSquareHUSTest: public CMipFilterTest
{
	public:
	CPoint_FullNonSquareHUSTest();
};

// Point_MaxMipLevel Class definitions
class CPoint_MaxMipLevelTest: public CMipFilterTest
{
	public:
	CPoint_MaxMipLevelTest();
};

// Point_LODBias Class definitions
class CPoint_LODBiasTest: public CMipFilterTest
{
	public:
	CPoint_LODBiasTest();
};

// Linear_MinPoint Class definitions
class CLinear_MinPointTest: public CMipFilterTest
{
	public:
	CLinear_MinPointTest();
};

// Linear_MinLinear Class definitions
class CLinear_MinLinearTest: public CMipFilterTest
{
	public:
	CLinear_MinLinearTest();
};

// Linear_NonSquareW Class definitions
class CLinear_NonSquareWTest: public CMipFilterTest
{
	public:
	CLinear_NonSquareWTest();
};

// Linear_NonSquareH Class definitions
class CLinear_NonSquareHTest: public CMipFilterTest
{
	public:
	CLinear_NonSquareHTest();
};

// Linear_FullSquareUT Class definitions
class CLinear_FullSquareUTTest: public CMipFilterTest
{
	public:
	CLinear_FullSquareUTTest();
};

// Linear_FullSquareCR Class definitions
class CLinear_FullSquareCRTest: public CMipFilterTest
{
	public:
	CLinear_FullSquareCRTest();
};

// Linear_FullSquareLC Class definitions
class CLinear_FullSquareLCTest: public CMipFilterTest
{
	public:
	CLinear_FullSquareLCTest();
};

// Linear_FullSquareUS Class definitions
class CLinear_FullSquareUSTest: public CMipFilterTest
{
	public:
	CLinear_FullSquareUSTest();
};

// Linear_FullNonSquareWUT Class definitions
class CLinear_FullNonSquareWUTTest: public CMipFilterTest
{
	public:
	CLinear_FullNonSquareWUTTest();
};

// Linear_FullNonSquareWCR Class definitions
class CLinear_FullNonSquareWCRTest: public CMipFilterTest
{
	public:
	CLinear_FullNonSquareWCRTest();
};

// Linear_FullNonSquareWLC Class definitions
class CLinear_FullNonSquareWLCTest: public CMipFilterTest
{
	public:
	CLinear_FullNonSquareWLCTest();
};

// Linear_FullNonSquareWUS Class definitions
class CLinear_FullNonSquareWUSTest: public CMipFilterTest
{
	public:
	CLinear_FullNonSquareWUSTest();
};

// Linear_FullNonSquareHUT Class definitions
class CLinear_FullNonSquareHUTTest: public CMipFilterTest
{
	public:
	CLinear_FullNonSquareHUTTest();
};

// Linear_FullNonSquareHCR Class definitions
class CLinear_FullNonSquareHCRTest: public CMipFilterTest
{
	public:
	CLinear_FullNonSquareHCRTest();
};

// Linear_FullNonSquareHLC Class definitions
class CLinear_FullNonSquareHLCTest: public CMipFilterTest
{
	public:
	CLinear_FullNonSquareHLCTest();
};

// Linear_FullNonSquareHUS Class definitions
class CLinear_FullNonSquareHUSTest: public CMipFilterTest
{
	public:
	CLinear_FullNonSquareHUSTest();
};

// Linear_MaxMipLevel Class definitions
class CLinear_MaxMipLevelTest: public CMipFilterTest
{
	public:
	CLinear_MaxMipLevelTest();
};

// Linear_LODBias Class definitions
class CLinear_LODBiasTest: public CMipFilterTest
{
	public:
	CLinear_LODBiasTest();
};

// None_FullSquare Class definitions
class CNone_FullSquareTest: public CMipFilterTest
{
	public:
	CNone_FullSquareTest();
};

// None_MaxMipLevel Class definitions
class CNone_MaxMipLevelTest: public CMipFilterTest
{
	public:
	CNone_MaxMipLevelTest();
};

// None_LODBias Class definitions
class CNone_LODBiasTest: public CMipFilterTest
{
	public:
	CNone_LODBiasTest();
};

#endif
