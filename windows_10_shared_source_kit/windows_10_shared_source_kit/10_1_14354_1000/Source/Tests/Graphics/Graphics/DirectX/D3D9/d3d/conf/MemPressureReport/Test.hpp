//**********************************************************************
// File: test.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for the test.
//
// History:
// 09/04/2007 Created by Jimmy Chen.
//**********************************************************************
#ifndef __TEST__
#define __TEST__

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dwmapi.h>
#include <comdef.h>
#include <strsafe.h>

#include <D3D9CustomTestApp.hpp>
#include <CustomTest.hpp>
#include <D3D9CustomTest.hpp>
#include <D3D9ToGDIAdapterTopologyMap.hpp>

#include "MemPressureReport.hpp"

///////////////////////////////////////////////////////
// Class:  CMyTestApp
//
// Purpose:
// Test application object.
//
// Notes:
// Singleton application object derived from
// CWGFTestFramework.  Required by the WGF framework.
///////////////////////////////////////////////////////
class CMyTestApp : public CD3D9CustomTestApp
{
public:
    // Construction/destruction
	CMyTestApp();

    // Public framework method overrides
	virtual TESTTYPE GetTestType() const;        // Returns the type of the app.
	virtual bool InitTestGroups();               // Registers test groups.
    virtual void CleanupTestGroup();             // Clean up test groups.
};


///////////////////////////////////////////////////////
// Class:  CMyTest
//
// Purpose:
// Test group object.
///////////////////////////////////////////////////////
class CMyTest : public CD3D9CustomTest< CMyTestApp >
{
public:
    // Construction/destruction.
	CMyTest();

public:
    static TestCaseDescriptor   m_testCaseDesc[];
    static LPTSTR               ptszHelperAppD3D;
    static LPTSTR               g_pwszWindowClass;
    static LPTSTR               g_pwszWindowName;

    // Public framework method overrides
	virtual void            InitTestParameters();         // Initializes parameters.
    virtual TEST_RESULT     Setup();                      // whole test program setup  
    virtual void            Cleanup();                    // whole test program cleanup
	virtual TEST_RESULT     SetupTestCase();              // Per-test-case setup. 
	virtual TEST_RESULT     ExecuteTestCase();            // Runs a test case.
	virtual void            CleanupTestCase();            // Per-test-case cleanup.

    static LRESULT CALLBACK WndProc(__in HWND hWnd, __in UINT uiMessage, __in WPARAM wParam, __in LPARAM lParam);

private:
    // Data members
    enum VerifyingTypes
    {
        Verify_Result_None,
        Verify_Result_InitialCondition,                   // verify memory pressure report after Issue(Begin) 
        Verify_Result_NoVideoMemoryAllocation,            // verify memory pressure report for no memory alloction
        Verify_Result_VideoMemoryAllocation,              // verify memory pressure report for memory allocation
        Verify_Result_VideoMemoryDeallocation,            // Verify memory pressure report for memory deallocation
    };
    enum AppActionCodes
    {
        Action_None,                              // do nothing
        Action_Wait,                              // wait
        Action_Wait_Process,                      // wait a given process to exit
        Action_Issue_Begin,                       // call IDirect3DQuery9::Issue(D3DISSUE_BEGIN)
        Action_Issue_End,                         // call IDirect3DQuery9::Issue(D3DISSUE_END)
        Action_RenderToTarget,                    // call Helper_RenderToTarget()
        Action_CreateTexture,                     // call Helper_CreateTexture()
        Action_ReleaseMemory,                     // call Helper_ReleaseResources()
        Action_End,
    };
    enum VideoMemType
    {
        Mem_None,
        Mem_Texture,
        Mem_RenderTarget,
        Mem_DepthStencilSurface,
    };

    int         m_ushCurTestCase;  
    UINT        m_uiVideoMemSize;

    IDirect3D9Ex            *m_pD3DEx;  
    IDirect3DDevice9Ex      *m_pD3dDeviceEx;
    IDirect3DQuery9         *m_pDirect3DQuery9;
    HWND                    m_hWnd;
    IDirect3DTexture9       *m_pTexture[MAX_TEXTURES_CREATED];
    IDirect3DSurface9       *m_pRenderTarget[MAX_TEXTURES_CREATED];
    IDirect3DSurface9       *m_pDepthStencilSurface[MAX_TEXTURES_CREATED];

    D3DPRESENT_PARAMETERS   g_pPresentParams;
    D3DMEMORYPRESSURE       m_iniD3dMemPressure;   // Initial D3D memory pressure report
    D3DMEMORYPRESSURE       m_preD3dMemPressure;   // previous D3D memory pressure report
    D3DMEMORYPRESSURE       m_curD3dMemPressure;   // current D3D memory pressure report

    BOOL                    m_bImplementAllTests;

    // Methods for test cases
    void        Helper_DisplayLastError(__in LPCTSTR pszParentFunctionName, __in LPCTSTR pszFunctionName);
    UINT        Helper_TotalAdapters();
    HRESULT     Helper_RegisterWindowClass();
    HRESULT     Helper_CreateAppWindows();
    HRESULT     Helper_CreateDeviceForAdapter();
    HRESULT     Helper_Initialization();
    HRESULT     Helper_RenderToTarget();
    HRESULT     Helper_CreateTexture(__in D3DPOOL d3dPool);
    void        Helper_ReleaseResources(__in USHORT ushMemType);
    HRESULT     Helper_GetMemPressureData(__in D3DMEMORYPRESSURE *pD3dMemPressure, __in DWORD dwWaitTime);
    HRESULT     Helper_CommonTestFunction(__in_ecount(ushApps) AppDescriptor *pAppDescriptor, __in USHORT ushApps);
    HRESULT     Helper_ImplementTests(__in AppDescriptor *pAppDescriptor);
    void        Helper_Cleanup();
    HRESULT     Helper_VerifyTestResults();

    // test cases
    HRESULT     TC_TU1_NoVideoMemoryAllocated_TC01();
    HRESULT     TC_TU1_NoVideoMemoryAllocated_TC02();
    HRESULT     TC_TU2_VideoMemoryAllocated_TC01();
    HRESULT     TC_TU2_VideoMemoryDeallocated_TC02();
    HRESULT     TC_TU2_IssueBeginTwice_TC03();
    HRESULT     TC_TU3_Multiprocess_TC01();
};

// Singleton application object.
extern CMyTestApp g_Application;

#endif // __TEST__