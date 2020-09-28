#ifndef _XPROIV_H_
#define _XPROIV_H_

#include "CrossProcShareTest.h"

const UINT IV_DIFFERENTSIZE         = 0x00000001;
const UINT IV_DIFFERENTUSAGE        = 0x00000002;
const UINT IV_DIFFERENTFORMAT       = 0x00000004;

const UINT IV_DIFFERENTTYPE         = 0x00000010;
const UINT IV_NULLRESOURCE          = 0x00000020;
const UINT IV_WRITEONREDONLY        = 0x00000040;
const UINT IV_CREATEDIFFERENTSIZE   = 0x00000080;

const UINT IV_CREATEDIFFERENTFORMAT = 0x00000200;
const UINT IV_OPENCLOSEDRESOURCE	= 0x00000400;

// ----------------------------------------------------------------------------
// class CCrossProcShare_Conf
// ----------------------------------------------------------------------------
class CCrossProcShare_IV : public CCrossProcShareTest
{
protected:
    
    void SetTestsToRun();

    // Methods that needs to be overriden in each tests
    virtual UINT ServerDispatchTest(DWORD dwTestID);

	// The Server tests
    UINT DifferentCreate();
    UINT DifferentReadOnlyCreate();
    UINT ClientOpenResource_Test();

    // Diffrent create input for read-only case
    UINT Test_Server_CreateDifferentSize(DWORD dwRTYPE);
    UINT Test_Server_CreateDifferentFormat(DWORD dwRTYPE);

    // Diffrent open input
    UINT Test_Server_DifferentSize(DWORD dwRTYPE);
    UINT Test_Server_DifferentUsage(DWORD dwRTYPE);
    UINT Test_Server_DifferentFormat(DWORD dwRTYPE);
    UINT Test_Server_DifferentType(DWORD dwRTYPE);

    UINT Test_Server_NullResource(DWORD dwRTYPE);
    UINT Test_Server_WriteOnReadOnly(DWORD dwRTYPE);
	UINT Test_Server_OpenClosedResource(DWORD dwRTYPE);


public: // class functions
	CCrossProcShare_IV();
	~CCrossProcShare_IV();

	// Framework overloads
    void CommandLineHelp(void);
};

#endif