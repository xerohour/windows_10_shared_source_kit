/******************************Module*Header*******************************
* Module Name: AllocOverhead.h
*
* Header for a test that measures the cpu overhead
* involved in locking, unlocking, creating, destroying, opening, and closing
* resources
*
* Author: Blake Pelton [blakep]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#ifndef __ALLOCOVERHEAD_H_
#define __ALLOCOVERHEAD_H_

#include "testcomponent.h"
#include <vector>

struct IDirect3DDevice9;

typedef std::vector<ULONGLONG> ResultVec;

class CAllocOverhead : public CTestComponent
{
public:
    CAllocOverhead();
    
protected:
    virtual void PreWalk(CLogWriter* pLog);
    virtual void PostWalk(CLogWriter* pLog);

private:
    void DestroyOverheadTest(bool bShared, 
                             IDirect3DDevice9* pDevice, 
                             ResultVec& Out);

    void LockOverheadTest(IDirect3DDevice9* pDevice,
                          DWORD dwLockFlags,
                          bool bRender,
                          ResultVec& Out);

    void CreateOverheadTest(IDirect3DDevice9* pDevice,
                            bool bShared,
                            ResultVec& Out);
};


#endif
