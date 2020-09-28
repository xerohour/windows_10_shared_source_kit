/////////////////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) Microsoft Corporation
//
//	Module Name:
//	   DXGIHybridPresent.h
//
//	Abstract:
//	This file contains declarations for DXGIHybridPresentPnPStop tests
//
//////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "DXGIHybridPresentTDR.h"

#define PNPSTOP_WAIT_TIME 10000

/////////////////////////////////////////////////////////////////////////
//		Tests
/////////////////////////////////////////////////////////////////////////

DWORD WINAPI PnPStopThread(LPVOID _PnPStopThreadParams);


struct PnPStopThreadParams : public GPUStopThreadParams
{
    CHybridETWMaster* pETWMaster;
};

class CHybridPresentPnPStop : public CHybridPresentTDR
{
public:
    virtual TEST_RESULT ExecuteTestCase();
};

