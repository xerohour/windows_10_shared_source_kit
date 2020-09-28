/******************************Module*Header*******************************
* Module Name: WGFPayloadTDR.h
*
* D3D11 Graphics and Compute TDR workload test
*
* Author: David Flenniken [davidfl]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#pragma once

//
// This class monitors profile functions on a per thread basis. 
//

class CPayloadConsumer: public CDxgKernelConsumer
{
public:
    CPayloadConsumer(CController* _petwController);
    ~CPayloadConsumer();

    void Start();
    void Stop();
    bool Reset();

    void Device_Handler(PEVENT_RECORD pEvent);
    void BlockGpuAccess_Handler(PEVENT_RECORD pEvent);
    void DelayContextScheduling_Handler(PEVENT_RECORD pEvent);

    bool IsDisableGPUTimeoutSet()
    {
        return m_bDisableGPUTimeout;
    }

    bool IsBlockGpuAccessBlocked()
    {
        return m_bAccessBlocked;
    }

    ULONG GetDelayContextSchedulingCount()
    {
        return m_DelayContextScheduling;
    }
    ULONG GetGlobalDelayContextSchedulingCount()
    {
        return m_GlobalDelayContextScheduling;
    }

private:
    CController* m_petwController;
    bool m_bRunning;

    bool m_bDisableGPUTimeout;
    bool m_bAccessBlocked;
    ULONG m_DelayContextScheduling;
    ULONG m_GlobalDelayContextScheduling;
};


