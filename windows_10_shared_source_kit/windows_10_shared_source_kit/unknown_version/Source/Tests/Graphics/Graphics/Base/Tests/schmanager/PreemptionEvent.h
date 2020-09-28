#pragma once

class CBasePreemptionEvent : public CGenericI3DComponent
{
public:
    CBasePreemptionEvent();
    ~CBasePreemptionEvent();

protected:

    void ReadBaseCommandLine();
    void LogMessage(WCHAR* _pMsg);

    std::string SyncKey;
    ULONG ulCount;
    ULONG ulRecordingTime;
    ULONG ulLoopIterations;

    void GetPreemptionCapabilties();
    bool m_SupportsGraphicsPreemption;
    D3DKMDT_GRAPHICS_PREEMPTION_GRANULARITY m_GraphicsPreemptionGranularity;
    bool m_SupportsComputePreemption;
    D3DKMDT_COMPUTE_PREEMPTION_GRANULARITY m_ComputePreemptionGranularity;
    UINT m_uiQuadrantLoc;
    bool m_bSoftwareDevice;
private:

};

class CXProcListenForPreemptionEvent : public CBasePreemptionEvent
{
public:
    CXProcListenForPreemptionEvent();
    ~CXProcListenForPreemptionEvent(){};

protected:
    virtual void RunTest();
    virtual void EndTest();

#define DECLARE_VALUE(_Name_) ULONG m_ul##_Name_

    DECLARE_VALUE(Success);
    DECLARE_VALUE(MissNoCommand);
    DECLARE_VALUE(MissNotEnabled);
    DECLARE_VALUE(MissNextFence);
    DECLARE_VALUE(SuccessNextFence);
    DECLARE_VALUE(MissPagingCommand);
    DECLARE_VALUE(SuccessPagingCommand);
    DECLARE_VALUE(MissSplittedCommand);
    DECLARE_VALUE(SuccessSplittedCommand);
    DECLARE_VALUE(MissFenceCommand);
    DECLARE_VALUE(MissRenderPendingFlip);
    DECLARE_VALUE(SuccessRenderPendingFlip);
    DECLARE_VALUE(MissNotMakingProgress);
    DECLARE_VALUE(MissLessPriority);
    DECLARE_VALUE(MissRemainingQuantum);
    DECLARE_VALUE(MissRemainingPreemptionQuantum);
    DECLARE_VALUE(MissAlreadyPreempting);
    DECLARE_VALUE(MissGlobalBlock);
    DECLARE_VALUE(SuccessGlobalBlock);
    DECLARE_VALUE(MissAlreadyRunning);
    DECLARE_VALUE(SuccessHigherInProcPriority);

private:

};


// The longest spec'd time that a preemption packet should take on a preemption aware
// driver.                             
#define LONGEST_SPECED_TIME_NS (4*1000) // microsecond value of 4 milliseconds

#define LONGEST_ALLOWED_TIME_NS (250*1000) // microsecond value or 250 milliseconds.

class CXProcListenForPreemptionTime : public CBasePreemptionEvent
{
public:
    CXProcListenForPreemptionTime();
    ~CXProcListenForPreemptionTime(){};

protected:
    virtual void RunTest();
    virtual void EndTest();

private:
};


class CXProcListenForTime : public CBasePreemptionEvent
{
public:
    CXProcListenForTime();
    ~CXProcListenForTime(){};

protected:
    virtual void RunTest();
    virtual void EndTest();

private:
};



