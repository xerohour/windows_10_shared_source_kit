// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// File Name:
//
//  tests.h
//
// Abstract:
//
//  include file for test case declarations
//


// -------------------------------------------------------------------------------

#ifndef _AC3Tst_INC_
#define _AC3Tst_INC_

#include <oacr.h>

OACR_WARNING_PUSH
OACR_WARNING_DISABLE(28302, "")
OACR_WARNING_DISABLE(28727, "")

//#include <windows.h>
#include <tchar.h>
#include <shell98.h>
#include <dvmodule.h>
#include <kslib.h>
#include <stdio.h>
#include <dsound.h>
#include <mmsystem.h>
#include <api2ks.h>
#include <devaudio.h>
#include <tests.h>
#include <waveres.h>
#include "resource.h"


// ------------------------------------------------------------------------------
class CDSoundDev : public CDeviceDescriptor
{
public:
    LPGUID  m_pGuid;
    ULONG   m_cPossibleInstances;
    BOOL    m_fHaveVolumePan;

    CDSoundDev(LPGUID pGuid, LPCSTR strRep);
};

class CWinMMDev : public CDeviceDescriptor
{
public:
    UINT    m_uDevID;
    ULONG   m_cPossibleInstances;

    CWinMMDev(UINT nID, LPCSTR strRep);
};

class CPCMPinDev : public CDeviceDescriptor
{
public:
    CKsPin* m_pPin;

    CPCMPinDev(CKsPin* pPin);
};

// ----------------------------------------------------------
// Class declaration
class CAc3Test : public CDeviceTestModule
{
public:
    // this member points to the current "device" being tested.
    // the test shell will set this in SetTestDevice
    CDeviceDescriptor*  m_pCurDevice;
    TTList<CPCMPinDev>  m_listPinDevs;
    BOOL                m_fLogPositions;
    BOOL                m_fGotKS;

public:
    CAc3Test(CTestShell* pShell, HINSTANCE hInstDLL);
    virtual ~CAc3Test(void);

    void            SetTestDevice(CDeviceDescriptor* pDevice);
    BOOL            EnumerateDevices(void);

    CPCMPinDev*     FindKsDevWithFilterName(LPCSTR pstrFilterName);
    CPCMPinDev*     FindKsDevWithDevNode(DWORD_PTR dwDevNodeID);

public:
    DWORD           Initialize(void);
    DWORD           OnInitialUpdate(void);
    void            ProcessMenuItem( DWORD nID, HWND hWndCtl, DWORD codeNotify, HMENU hMenu);
    DWORD           RunTest(DWORD dwTestID);
};

#define LOG g_IShell->Log
// ----------------------------------------------------------
// ----------------------------------------------------------

// Globals
extern CAc3Test*        g_pAC3Tst;
extern WAVERESOURCE     g_wrAC3;
extern WAVERESOURCE     g_wrPCM;
extern OSVERSIONINFO    g_OSVersionInfo;

#define g_pCurDevice    g_pAC3Tst->m_pCurDevice
#define g_fLogPositions g_pAC3Tst->m_fLogPositions
#define IS_LONGHORN     (6 <= g_OSVersionInfo.dwMajorVersion)

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------

// fns
extern double qpcGetTime(void);

#define LogCall(s)      LOG(eInfo1, "Calling " TEXT(#s))

inline BOOL
IsEqualGUIDAligned(GUID guid1, GUID guid2)
{
    return ((*(PLONGLONG)(&guid1) == *(PLONGLONG)(&guid2)) && (*((PLONGLONG)(&guid1) + 1) == *((PLONGLONG)(&guid2) + 1)));
}

// ------------------------------------------------------------------------------
#define POS_ERR_TOLERANCE_MS 2.

// ------------------------------------------------------------------------------

#endif // #ifndef _AC3Tst_INC_

OACR_WARNING_POP
