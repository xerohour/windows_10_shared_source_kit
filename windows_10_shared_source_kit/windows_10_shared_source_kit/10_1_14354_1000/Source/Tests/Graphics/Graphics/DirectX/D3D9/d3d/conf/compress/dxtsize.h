/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	DXTSize.h
 *  Header for a dxt textures of varying sizes test.  
 *  This was written to catch a bug in an S3 driver exposed by NASCAR Racing 2003
 *  See product studio bug (74312)
 *
 ***************************************************************************/

#ifndef DXTSIZE_H
#define DXTSIZE_H

#include "CD3DTest.h"

// Defines
#define MAX_TESTS		18

// Class definition
class CDXTSizeTest: public CD3DTest
{
public:
	CDXTSizeTest();
	~CDXTSizeTest();

	// Framework functions
	virtual UINT TestInitialize();
	virtual bool ExecuteTest(UINT);
    virtual void UpdateStatus();
	virtual bool CapsCheck();


private:
	virtual bool DrawTriangle();

	//These store the maximum dimensions for textures (set in CapsCheck())
	UINT	m_nMaxTextureWidth;
	UINT	m_nMaxTextureHeight;

	//These store the minimum dimensinos for textures (set in CapsCheck())
	UINT	m_nMinTextureWidth;
	UINT	m_nMinTextureHeight;

	//These are set by ExecuteTest and read by UpdateStatus();
	UINT	m_nLastTextureWidth;
	UINT	m_nLastTextureHeight;

};



#endif //DXTSIZE_H

