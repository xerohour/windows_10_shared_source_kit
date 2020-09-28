#ifndef _MODE_SWITCH_H
#define _MODE_SWITCH_H

#include "BaseVid.h"
#include "GeneratePnPStop.h"

class CModeSwitch: public CTestComponent
{
    public:
        CModeSwitch();

    protected:
        /*
            Overrides from CGenericI3DComponent
            This is where we actually execute the test.
        */
        virtual void PreWalk(CLogWriter * pLog);
        virtual void PostWalk(CLogWriter * pLog);   

    private:
        LONG m_nThreadsActive;
};

struct ThreadData
{
    //pointer to a variable that contains the number of active threads
    LONG* pThreadsActive;
    UINT numRepetitions;
};


class CModeSwitchThread
{
public:
    void operator()(ThreadData threadData);
};

class CPnPStopThread
{
public:
    void operator()(ThreadData threadData);
};


#endif //_MODE_SWITCH_H
