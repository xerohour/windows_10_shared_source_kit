#ifndef USER_H
#define USER_H

class CUser:public IUserBase
{
public:
    CUser(HWND);
    HRESULT Initialize(BOOL bWindowed);
    HRESULT Run();
    HRESULT Cleanup();

    HRESULT XProcTest_IndexBuffer_Open();
    HRESULT XProcTest_VertexBuffer_Open();
    HRESULT XProcTest_Basic_Open();    
    HRESULT XProcTest_InvalidFormat();
    HRESULT XProcTest_InvalidLevel();
    HRESULT XProcTest_OwnerGoesAway();
    HRESULT XProcTest_UserTriesToLock();
    HRESULT XProcTest_ValidateTexture();
    HRESULT XProcTest_MultiUserScenario();
    HRESULT XProcTest_RoundRobin();
    HRESULT XProcTest_OpenClose_RaceCondition();
    HRESULT XProcTest_EvictSharedAllocation();
    HRESULT XProcTest_CrossProcSwizzlingRangeContention();

    HRESULT OpenSharedAllocation(Message * msg, CI3DTexture** ppTexture);
    HRESULT UseSharedAllocation(Message* msg, CI3DTexture* pTexture);
    HRESULT OwnerDoWork();
    HRESULT CreatePrimary(CI3DNonMipped2DResource** pRes);
    HRESULT UsePrimary(CI3DNonMipped2DResource* pRes);
    HRESULT LockPrimary(CI3DNonMipped2DResource* pRes);
    HRESULT UnlockPrimary(CI3DNonMipped2DResource* pRes);

    void PrintMessage(PMessage);

private:
    
    HWND m_hWnd;
    HANDLE m_hOwner;
    HANDLE m_hUser;
    HANDLE m_hRoundRobinOwner;
    HANDLE m_hRoundRobinUser;
    // Rather than a raw pointer to the shared memory, we're going to abstract it a little
    // in order to get locking functionality in the object itself.
    PSharedMemory m_pSharedObject;

    CI3DDevicePtr m_pDevice;
    CI3DPtr m_p3D;

    //Defines the length of time to wait before declaring syncronization timeouts a failure.
    DWORD m_syncronizationTimeout;

};

#endif