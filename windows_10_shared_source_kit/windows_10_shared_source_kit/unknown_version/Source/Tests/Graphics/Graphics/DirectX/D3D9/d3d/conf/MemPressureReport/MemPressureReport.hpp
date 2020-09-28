//**********************************************************************
// File: MemPressureReport.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces or data structures for the test of Support DWM On.
//
// History:
// 09/04/2007 Created by Jimmy Chen.
//**********************************************************************
#ifndef __SUPPORT_DWM_ON__
#define __SUPPORT_DWM_ON__

#include <windows.h>

#define THREAD_SLEEP_TIME           50
#define DWM_COMPOSITION_OFF_TIME    4000

// Self define error codes
#define E_TESTSKIPPED           MAKE_DDHRESULT(0xffffff0)


#define D3DFVF_CUSTOMVERTEX             (D3DFVF_XYZ|D3DFVF_DIFFUSE)
#define MAX_TEXTURES_CREATED            1000

typedef struct
{
    float x, y, z;      // The untransformed, 3D position for the vertex
    DWORD color;        // The vertex color
} SIMPLE_VERTEX;

// AppDescriptor: store information about an application used for the test
typedef struct tagAppDescriptor
{
    struct    {
    	USHORT    ushAction;                     // Action_None will be used as "Terminator"
        DWORD     dwParameter;                   // parameter for the action   
        void      *pData;                        // pointer to test data 
    } AppOperations[2];                          // Operations for the application. 
} AppDescriptor, *PAppDescriptor;

// TestCaseDescriptor: store test cases specific information
typedef struct tagTestCaseDescriptor
{
	LPCTSTR         TestCaseName;       // The name of test case 
	BOOL            bEnabled;           // TRUE - the test case is enabled, otherwise disabled 
	USHORT          ExpectedResults[2]; // Expected test results. Verify_Result_None will be used as "Terminator" 
} TestCaseDescriptor, *PTestCaseDescriptor;

#endif // __SUPPORT_DWM_ON__
