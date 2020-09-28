#ifndef _PNP_STOP_TEST_H_
#define _PNP_STOP_TEST_H_

#include "BaseVid.h"

class CPnPStopDeviceParams: public CTestComponent
{
    public:
        CPnPStopDeviceParams();

    protected:
        virtual void PreWalk(CLogWriter * pLog);
        virtual void PostWalk(CLogWriter * pLog);    
		
};

class CPnPRestartLinkedDevice: public CTestComponent
{
    public:
        CPnPRestartLinkedDevice();

    protected:
        virtual void PreWalk(CLogWriter * pLog);
        virtual void PostWalk(CLogWriter * pLog);    
		
};

#endif
