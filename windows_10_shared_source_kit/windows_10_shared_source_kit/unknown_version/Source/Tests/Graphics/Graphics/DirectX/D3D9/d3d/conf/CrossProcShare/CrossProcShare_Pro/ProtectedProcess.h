#ifndef _XPROTECTEDPROCESS_H_
#define _XPROTECTEDPROCESS_H_

#include "CrossProcShareTest.h"

const UINT PRO_CREATE          = 0x00000001;
const UINT PRO_FAILOPEN        = 0x00000002;
const UINT PRO_VERIFYNONPRO    = 0x00000004;
const UINT PRO_VERIFYPRO       = 0x00000008;
const UINT PRO_FILLNONPRO      = 0x00000010;
const UINT PRO_FILLPRO         = 0x00000020;

// ----------------------------------------------------------------------------
//class CCrossProcShareConf
// ----------------------------------------------------------------------------
class CProtectedProcess : public CCrossProcShareTest
{
protected:
    
    void SetTestsToRun();

    // Methods that needs to be overriden in each tests
    virtual UINT ServerDispatchTest(DWORD dwTestID);

    virtual bool StartClients();

	// The Server tests
	UINT Test_Server_Create(DWORD dwRTYPE);
    UINT Test_Server_FailOpen(DWORD dwRTYPE);
    UINT Test_Server_VerifyFill(DWORD dwRTYPE, bool bProtected, bool bFill);

public: // class functions
	CProtectedProcess();
	~CProtectedProcess();

	// Framework overloads
    void CommandLineHelp(void);
};

#endif