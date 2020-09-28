/******************************Module*Header*******************************
* Module Name: badapp.h
*
* Header for a component that uses a large GPU quantum
*
* Author: Blake Pelton [blakep]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/

#ifndef __BADAPP_H_
#define __BADAPP_H_


#include "testcomponent.h"

class CBadApp : public CTestComponent
{
public:
    CBadApp();
    
protected:
    virtual void PreWalk(CLogWriter* pLog);
    virtual void PostWalk(CLogWriter* pLog);
};




#endif
