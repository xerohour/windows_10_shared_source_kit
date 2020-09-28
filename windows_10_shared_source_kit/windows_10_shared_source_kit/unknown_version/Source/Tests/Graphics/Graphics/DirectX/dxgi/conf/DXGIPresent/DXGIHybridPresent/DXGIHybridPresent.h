/////////////////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) Microsoft Corporation
//
//	Module Name:
//	   DXGIHybridPresent.h
//
//	Abstract:
//	This file contains declarations for DXGIHybridPresent tests
//
//////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "HybridPresentETWValidation.h"
#include <windows.h>
#include <dxgipresent.h>
#include <AdapterUtil.h>
#include "DXGIValueSets.h"
#include "CrossProcSharing.h"

using namespace DevX::GRFX;

extern bool g_HybridMode;
extern UINT g_TimeToExecute;
extern HWND* g_pTestWindow;
extern bool g_bBDDHybrid;
extern bool g_bNoETWValidation;
extern bool g_bSkipVistaBlt;

#define START_THREAD_EVENT "StartThreadEvent"
#define STOP_THREAD_EVENT "StopThreadEvent"
#define TDR_THREAD_WAIT_TIME 15000

#define HYBRID_BASIC_MAPPING_NAME L"HybridBasicMapping"
#define HYBRID_BASIC_TOTAL_SHARERS 2
#define HYBRID_BASIC_MUTEX_WAIT 5000
#define ERROR_SIZE 512
#define SYNC_PAIR_INDEX 0
#define MANAGER_HANDLE 1
#define CLIENT_HANDLE 2
#define XPROC_TIMEOUT 200000


/////////////////////////////////////////////////////////////////////////
//		Tests
/////////////////////////////////////////////////////////////////////////

class CHybridPresentRedirection : public CPresentWinConvertStretch
{
public:
    CHybridPresentRedirection();
    virtual ~CHybridPresentRedirection();

    //
    // Static methods
    //
public:
    static bool CheckSystemAdapters( bool &bIsHybridSystem, bool &bFoundBDAorBDDAdapter );

    //
    // Framework override methods
    //
public:
    void InitTestParametersImpl();
    HRESULT TransitionBeforePresent();
    HRESULT TransitionAfterPresent();
    virtual TEST_RESULT Setup();
    virtual void Cleanup();
    TEST_RESULT ValidateCrossAdapterSurfaces();
    TEST_RESULT ValidatePresents(bool bEnableNoPresentsValidation = true);
    bool IsVistaBLT();

public:
    virtual TEST_RESULT ExecuteTestCase();
    bool m_bIsWindowed;
    CHybridETWMaster* GetETWMaster(){return &m_ETWMaster;}

private:
    CHybridETWMaster m_ETWMaster;
    void CleanupETW();
    bool m_bHybridMode;
    bool m_bFullScreenWindow;
};

struct GPUStopThreadParams
{
    CritSection csCritSection;
    bool bIGPU;
    bool bDGPU;
    bool* pbExpectDeviceLost;
    bool* pbStopNow;
    bool* pbIsWindowed;
    int action;
    TCHAR strError[ERROR_SIZE];
    HRESULT hr;
};

struct sHYBRID_PRESENT_SHARED_DATA
{
    CDXGITest* pTest;
    TEST_RESULT tr;
    TCHAR strError[ERROR_SIZE];
};

