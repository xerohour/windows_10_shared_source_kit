#ifndef _PNP_STOP_VGA_H
#define _PNP_STOP_VGA_H

#include "BaseVid.h"
#include "GeneratePnPStop.h"

class CPnPStopVGA: public CTestComponent
{
    public:
        CPnPStopVGA();

    protected:
        virtual void PreWalk(CLogWriter * pLog);
        virtual void PostWalk(CLogWriter * pLog);

    private:
        void CheckVGA(CLogWriter * pLog, bool bUMPC);
        bool IsMinResolutionSupported(CLogWriter * pLog, DISPLAY_DEVICE* pDispDev, bool bUMPC);
        
};

#endif
