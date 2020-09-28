#ifndef _XPROCCONF_H_
#define _XPROCCONF_H_

#include "CrossProcShareTest.h"

const UINT CONF_CREATE                  = 0x00000001; // Create
const UINT CONF_OPEN                    = 0x00000002; // Create open in same process
const UINT CONF_CLIENTOPEN              = 0x00000004; // Create in process1 open in process2
const UINT CONF_VERIFY                  = 0x00000008; // Create/fill in process1 open/verify in process2
const UINT CONF_FILL                    = 0x00000010; // Create/verif in process1 open/fill in process2
const UINT CONF_LOCK                    = 0x00000020; // Create/lock/unlock in process1 open/lock/unlock in process2
const UINT CONF_RELEASESOURCEUSEOPENED  = 0x00000040; //


const UINT CONF_FROMOTHERMONITOR        = 0x00000001;


// ----------------------------------------------------------------------------
// class CCrossProcShare_Conf
// ----------------------------------------------------------------------------
class CCrossProcShare_Conf : public CCrossProcShareTest
{
protected:
    
    void SetTestsToRun();

    // Methods that needs to be overriden in each tests
    virtual UINT ServerDispatchTest(DWORD dwTestID);

	// The Server tests
	UINT Test_Server_Create(DWORD dwRTYPE);
    UINT Test_Server_Open(DWORD dwRTYPE);
    UINT Test_Server_ClientOpen(DWORD dwRTYPE);
    UINT Test_Server_Verify(DWORD dwRTYPE);
    UINT Test_Server_Fill(DWORD dwRTYPE);
    UINT Test_Server_Lock(DWORD dwRTYPE);
    UINT Test_Server_ReleaseSourceUseOpen(DWORD dwRTYPE);

public:
	CCrossProcShare_Conf();
	~CCrossProcShare_Conf();

	// Framework overloads
    void CommandLineHelp(void);
};
#endif