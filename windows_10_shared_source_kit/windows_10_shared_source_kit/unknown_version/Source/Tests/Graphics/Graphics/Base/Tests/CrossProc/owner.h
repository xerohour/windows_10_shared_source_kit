#ifndef OWNER_H
#define OWNER_H

#include "AdapterDisplayInfo.h"
#include "CTlLogger.h"

//Forward define
class AdapterDisplayInfo;

class COwner:public IUserBase
{
public:
    COwner(HWND);
    ~COwner();
    HRESULT Initialize(BOOL bWindowed);
    HRESULT Run();
    HRESULT Cleanup();
    void ThrowResult(PMessage, HANDLE testHandle);


    //test to run in FUS for cross session testing
    HRESULT XProcTest_FUSTest(HANDLE testHandle);

    //test to create and share an index buffer across two processes
    HRESULT XProcTest_IndexBuffer_Open(HANDLE testHandle);
    //test to create and share an vertex buffer across two processes
    HRESULT XProcTest_VertexBuffer_Open(HANDLE testHandle);    
    //basic texture open, the user should be able to open the texture
    HRESULT XProcTest_Basic_CreateOpen(HANDLE testHandle);
    
    //these tests check the param validation of the user
    //the user should not be able to open the texture during these tests
    HRESULT XProcTest_InvalidHandle_DecrementOne(HANDLE testHandle); 
    HRESULT XProcTest_InvalidHandle_IncrementOne(HANDLE testHandle);
    HRESULT XProcTest_InvalidHeightDimension(HANDLE testHandle);
    HRESULT XProcTest_InvalidWidthDimension(HANDLE testHandle);
    HRESULT XProcTest_InvalidFormat(HANDLE testHandle);
    HRESULT XProcTest_InvalidLevel(HANDLE testHandle);
    HRESULT XProcTest_RoundRobin(HANDLE testHandle);
    HRESULT XProcTest_OpenClose_RaceCondition(HANDLE testHandle);
    

    //these tests check that the user cannot lock and 
    //that the owner can lock the surface    
    HRESULT XProcTest_UserTriesToLock(HANDLE testHandle);
    HRESULT XProcTest_OwnerTriesToLock(HANDLE testHandle);   
    
    //what happens if the owner goes away? can the user access the resource?
    HRESULT XProcTest_OwnerGoesAway(HANDLE testHandle);
    //check that the resource is the same when opened by the user
    HRESULT XProcTest_ValidateTexture(HANDLE testHandle);

    //Create several user processes and have them open and verify a surface
    HRESULT XProcTest_MultiUserScenario(HANDLE testHandle);

    // Attempting to open a shared resource from one adapter which was created on 
    // another adapter should not be allowed
    HRESULT XProcTest_CrossAdapterNotShare(HANDLE testHandle);

    //Attempting to open a resource created in one session from another session 
    //should not be allowed.
    HRESULT XProcTest_CrossSessionNotShare(HANDLE testHandle);

    // Shared allocation will be evicted after being opened by both owner and user
    // owner and user will take turn in causing allocation to be paged back in
    HRESULT XProcTest_EvictSharedAllocation(HANDLE testHandle);

    //	2 processes will content for swizzling ranges by both locking swizzled 
    //  allocations without unlocking
    //	owner will lock non-pinned allocations which will be eventually evicted
    //  by the user locking pinned allocations
    HRESULT XProcTest_CrossProcSwizzlingRangeContention(HANDLE testHandle);


    HRESULT OnAllocate(D3DDDICB_ALLOCATE* pInfo, bool* pContinue);
    HRESULT CreateUserProcess(BOOL bWindowed = TRUE);
    HRESULT ParseINI();
    void DebugBreak();
    HRESULT CauseEviction();
    HRESULT CreateAndFillTexture(Message * msg, UINT Flags, CI3DTexture** ppTexture);
    HRESULT CreateUserProcessAndWait(BOOL bWindowed = TRUE);
    HRESULT UserDoWork();
    HRESULT UseTexture(Message * msg, CI3DTexture* pTexture);
    HRESULT CheckTexturePattern(CI3DTexture* pTexture);
    HRESULT LockTexture(UINT Flags, CI3DTexture* pTexture);
    HRESULT UnlockTexture(CI3DTexture* pTexture);

private:
    HWND                    m_hWnd;
    HANDLE                  m_hOwner;
    HANDLE                  m_hUser;
    HANDLE                  m_hRoundRobinOwner;
    // Rather than a raw pointer to the shared memory, we're going to abstract it a little
    // in order to get locking functionality in the object itself.
    PSharedMemory           m_pSharedObject;

    CI3DVertexBufferPtr     m_pVertexBuffer;
    CI3DDevicePtr           m_pDevice;
    CI3DPtr                 m_p3D;  
    TESTCASE                m_TestCaseParams;

    CCTLLogger*             m_Log;

    //Login credentials for use in the FUS framework and any cross session testing
    LPCWSTR m_pwszUserName;
    LPCWSTR m_pwszPassword;

    //Timeout for syncronization events
    DWORD m_syncronizationTimeout;


    //Check to see if an error occurred, if so it will be logged
    bool ErrorOccurredDuringInitialization(HRESULT hr, LPWSTR pwszTestCaseName, LPWSTR pwszDebugErrorMessage, HANDLE testHandle);

    //Initializes a CI3DPtr object and CI3DDevicePtr object for use with the specified adapter
    HRESULT InitializeD3DObjects(CI3DPtr* p3D, CI3DDevicePtr* pDevice, LPCWSTR pwszAdapterName);

    //For the given DisplayName adds the corresponding adapter to the adapterList IF
    //there is not already another display connected to the same adapter which was 
    //already passed into this function.
    HRESULT GetUniqueAdapterList(const std::wstring displayName, std::vector<AdapterDisplayInfo>& adapterList);
};

#endif
