/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	Overdraw.h
 *
 ***************************************************************************/

#ifndef __OVERDRAW_H__
#define __OVERDRAW_H__

#include "CD3DTest.h"
#include "CSurface.h"

// Defines
#define MAX_TESTS	40
#define RAND_SEED	4321

// Class definition
class COverdrawTest: public CD3DTest
{
    // Data
	private:
    CSurface * pSrcTarget;
	TCHAR	msgString[80];
	int		nOffset;

	public:
	COverdrawTest();
	~COverdrawTest();

	// Framework functions
	virtual bool SetDefaultRenderStates(void);
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual bool ProcessFrame(void);
    virtual bool TestTerminate(void);

	// Helper functions
	bool AlphaGrid(int nTest, float fGridXPos, float fGridYPos, int nXDivisions, int nYDivisions);
	void ReverseVertices(D3DTLVERTEX * pVertices, int nVertices);
};

// Alpha Class definitions
class CAlpha_OverdrawTest: public COverdrawTest
{
	public:
	CAlpha_OverdrawTest();
	~CAlpha_OverdrawTest();
};

#endif
