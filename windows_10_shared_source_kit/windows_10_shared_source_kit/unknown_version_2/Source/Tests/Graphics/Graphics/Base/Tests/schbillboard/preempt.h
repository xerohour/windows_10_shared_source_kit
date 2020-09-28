/******************************Module*Header*******************************
* Module Name: Preempt.h
*
* Header for a component that creates preemptable situations
*
* Author: David Flenniken [davidfl]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#pragma once

#define MAX_UNSIGNED_INT 0xFFFFFFFF

#define FAST_AS_POSSIBLE 1000


typedef enum _TriangleType {
    EquilateralTriangle,
    RightTriangle,
    IsoscelesTriangle,
    ScaleneTriangle,
    ObtuseTriangle
} TriangleType;

class CTriangle
{
private:
    TriangleType m_Type;
    double m_Base;
    int m_iBase;
    int m_iHeight;
public:
    CTriangle(TriangleType _Type,
              UINT _iPixelsPerTriangle);
    ~CTriangle();

    int Base();
    int Height();
    int Pixels();
};

struct Vertex
{
    D3DXVECTOR4 Pos;//x y z rhw
    DWORD Color;
};


class CWorkLoad
{
private:
    IDirect3DDevice9Ex* m_pDevice;

    CComPtr< IDirect3DVertexBuffer9 > m_pVB;

    Vertex* m_pVertices;

public:
    CWorkLoad(IDirect3DDevice9Ex* _pDevice);
    ~CWorkLoad();
    void Release();
    void Initialize(CTriangle* _pTriangle,
                    int _iTriangles,
                    bool _bcase);
    void SetStreamSource();

    IDirect3DDevice9Ex* GetDevice();
};
          
class CPreemptBase: public CTestComponent
{
public:
    CPreemptBase();

    void PerformWork();

    void ReadCommandLine(CLogWriter* _pLog);

    HRESULT DeterminePriorities();

    // Command line arguments
    UINT nTotalTime;
    UINT nTriangles;
    UINT nppPrimitive;
    UINT nppFrame;
    int  iGPUPriority;
    DWORD dwPriorityClass;
    int  iThreadPriority;
    bool bWaitForVBlank;

    //Other variables
    IDirect3DDevice9Ex* pDevice;
    CLogWriter* pLog;

    void DumpParameters(UINT _uiCounter);
private:
};
             
class CPreempt : public CPreemptBase
{
public:
    CPreempt();
    
protected:
    virtual void PreWalk(CLogWriter* pLog);
    virtual void PostWalk(CLogWriter* pLog);
};

class CAutoPreempt : public CPreemptBase
{
public:
    CAutoPreempt();
    
protected:
    virtual void PreWalk(CLogWriter* pLog);
    virtual void PostWalk(CLogWriter* pLog);
};

class CPreempter : public CPreemptBase
{
public:
    CPreempter();
    
protected:
    virtual void PreWalk(CLogWriter* pLog);
    virtual void PostWalk(CLogWriter* pLog);

    CWorkLoad* CreateWorkLoad(LPDIRECT3DDEVICE9EX _pDevice,
                              UINT _nppPrimitive,
                              UINT _nTriangles);

    void ResetDevice(bool bWindowed);

    UINT nPreemptionsPerSecond;

    CTriangle* m_pTrianglePrim;
    CWorkLoad* m_pWork;

    UINT m_nAdapterOrdinal;
    D3DSURFACE_DESC m_BackBufferDesc;
    HWND  m_hRenderWnd;

};


