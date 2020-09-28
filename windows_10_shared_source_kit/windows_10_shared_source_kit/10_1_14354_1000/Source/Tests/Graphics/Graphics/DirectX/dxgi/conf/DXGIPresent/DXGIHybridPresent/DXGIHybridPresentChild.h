/////////////////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) Microsoft Corporation
//
//	Module Name:
//	   DXGIHybridPresentChild.h
//
//	Abstract:
//	This file contains declarations for DXGIHybridPresentChild test. This test will be 
//  launched by DXGIHybridPowerManager group tests
//
//////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "DXGIHybridPresentTDR.h"

/////////////////////////////////////////////////////////////////////////
//		Tests
/////////////////////////////////////////////////////////////////////////

class CHybridPresentChild : public CHybridPresentTDR
{
public:
    CHybridPresentChild();
    ~CHybridPresentChild();

    void InitTestParameters();
    TEST_RESULT SetupTestCase();
    TEST_RESULT ExecuteTestCase();
    void CleanupTestCase();

protected:
    CXProcSyncClient* m_pClientSync;
    UINT m_TestCasesExecuted;
};

class CHybridPresentChildIdle : public CHybridPresentChild
{
public:
    TEST_RESULT ExecuteTestCase();
    HRESULT TransitionAfterPresent();	// will use this method to cause idle time after presenting
};

