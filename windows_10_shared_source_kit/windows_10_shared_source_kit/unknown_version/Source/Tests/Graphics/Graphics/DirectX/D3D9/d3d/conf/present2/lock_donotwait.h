#if !defined(AFX_LOCKDONOTWAIT_H)
#define AFX_LOCKDONOTWAIT_H

#include "d3d9.h"
#include "CD3DTest.h"
#include "CSurface.h"

    
//-----------------------------------------------------------------------------
// Definition of the test class
//-----------------------------------------------------------------------------
class LockDoNotWait : public CD3DTest
{
public:
	LockDoNotWait();
	virtual bool CapsCheck();						// Check caps
	UINT TestInitialize();
	bool ExecuteTest(UINT);
};

#endif