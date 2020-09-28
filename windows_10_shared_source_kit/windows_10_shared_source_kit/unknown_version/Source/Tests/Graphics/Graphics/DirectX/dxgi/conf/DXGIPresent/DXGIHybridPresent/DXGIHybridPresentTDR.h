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
#include "DXGIHybridPresent.h"

/////////////////////////////////////////////////////////////////////////
//		Tests
/////////////////////////////////////////////////////////////////////////

DWORD WINAPI TDRThread(LPVOID _TDRThreadParams);

enum HELPER_THREAD_SCENARIO
{
    HELPER_THREAD_SCENARIO_IGPU = 0,
    HELPER_THREAD_SCENARIO_DGPU = 1,
    HELPER_THREAD_SCENARIO_IGPU_DGPU = 2
};

class CHybridPresentTDR : public CHybridPresentRedirection
{
public:
    CHybridPresentTDR();

    void InitTestParameters();
    virtual TEST_RESULT ExecuteTestCase();
    RFactor CreateSingleModeFactor();

    HELPER_THREAD_SCENARIO m_HelperThreadScenario;
};

