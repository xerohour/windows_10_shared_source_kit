/******************************Module*Header*******************************
* Module Name: overcommit.h
*
* Header for a test that overcommits to video memory
*
* Author: Blake Pelton [blakep]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#ifndef __OVERCOMMIT_H_
#define __OVERCOMMIT_H_

#include "testcomponent.h"
#include <vector>

struct IDirect3DDevice9;
struct IDirect3DSurface9;

class COverCommit : public CTestComponent
{
public:
    COverCommit();
    
protected:
    virtual void PreWalk(CLogWriter* pLog);
    virtual void PostWalk(CLogWriter* pLog);

private:
    void Cleanup();
    void AllocateSurfaces(IDirect3DDevice9* pDevice,
                          UINT nSize);
    void RenderFrame(IDirect3DDevice9* pDevice,
                     UINT nSurface);

    
    std::vector< IDirect3DSurface9* > m_Surfaces;
};



#endif
