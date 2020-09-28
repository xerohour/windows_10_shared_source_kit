#pragma once


class CXProcManager : public CGenericI3DComponent
{
public:
    CXProcManager();
    ~CXProcManager(){};

protected:
    virtual void RunTest();
    virtual void EndTest();

    WCHAR* GetState(STATE_TYPE _stateType);
    void DumpClientStates(CXProcSyncManager* _pxProc);

    void LogMessage(WCHAR* _pMsg);
};

class CXProcClient : public CGenericI3DComponent
{
public:
    CXProcClient();
    ~CXProcClient(){};

protected:
    virtual void RunTest();
    virtual void EndTest();

    void LogMessage(WCHAR* _pMsg);
};


