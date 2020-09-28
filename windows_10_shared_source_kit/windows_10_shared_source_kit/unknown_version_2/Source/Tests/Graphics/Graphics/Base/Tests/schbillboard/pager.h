/******************************Module*Header*******************************
* Module Name: pager.h
*
* Header for a component that periodically pages in resources
*
* Author: Blake Pelton [blakep]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#pragma once

class CPagerBase : public CTestComponent
{
public:
    void CommonSetupCommandLine();

    void CreateSurfaces(UINT nMegabytes);
    void UseSurface(size_t nIndex);

    std::vector< CComPtr< IDirect3DSurface9 > > m_Surfaces;
    CComPtr< IDirect3DDevice9Ex > m_pDevice;
};


class CPager : public CPagerBase
{
public:
    CPager();
    
protected:
    virtual void PreWalk(CLogWriter* pLog);
    virtual void PostWalk(CLogWriter* pLog);
};

class CPagerSync : public CPagerBase
{
public:
    CPagerSync();
    
protected:
    virtual void PreWalk(CLogWriter* pLog);
    virtual void PostWalk(CLogWriter* pLog);
};



