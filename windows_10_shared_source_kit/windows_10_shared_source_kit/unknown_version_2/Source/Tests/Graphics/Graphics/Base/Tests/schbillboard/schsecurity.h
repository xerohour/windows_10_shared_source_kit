/******************************Module*Header*******************************
* Module Name: schsecurity.h
*
* Header for a test that verifies the scheduler properly checks
* for security permissions when apps try to set their priority class
* 
* Author: Blake Pelton [blakep]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#ifndef __SCH_SECURITY_H_
#define __SCH_SECURITY_H_

#include "testcomponent.h"

class CSchSecurity : public CTestComponent
{
public:
    CSchSecurity();
    
protected:
    virtual void PreWalk(CLogWriter* pLogWriter);
    virtual void PostWalk(CLogWriter* pLogWriter);
};



#endif
