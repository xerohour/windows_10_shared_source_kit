#ifndef COMMON_H
#define COMMON_H

//------------------------------------------------------------------------------
// This flag turns on PATH_TRAP functionality.
#define ENABLE_PATH_TRAP

// For path verification development testing
#ifndef PATH_TRAP

#ifdef _X86_

#ifdef ENABLE_PATH_TRAP

#ifdef DBG
#define PATH_TRAP() {_asm int 3}
#else
#define PATH_TRAP()
#endif

#else
#define PATH_TRAP()
#endif

#else
// KdBreakpoint doesn't work nearly as nicely as int 3 on x86, but at least we 
// can trap this way
#ifdef ENABLE_PATH_TRAP

#ifdef DBG

#define PATH_TRAP() { DbgBreakPoint(); }
#else
#define PATH_TRAP()
#endif

#else // else of _DEBUG
#define PATH_TRAP() 
#endif

#endif  // end of #ifdef _X86_

#endif // end of #ifndef PATH_TRAP
//------------------------------------------------------------------------------

// This is a MACRO so that we can get the function and line where the problem occurs
// in the file. Note that if the _LOG_ param is non-NULL the same message will be
// echoed into the test log file. Also, this MACRO should not be used for debug
// trace messages - but rather valid errors that we want to track.
#ifdef DBG
#define LOG_AND_DBG_MSG(_LOG_,_STR_) { WCHAR _wszDbg_x[MAX_PATH]; \
    StringCchPrintf(_wszDbg_x,MAX_PATH,L"%S:%d %s\n\r",__FUNCTION__,__LINE__,_STR_); \
    if(_LOG_) { _LOG_->Message(_wszDbg_x); } \
    OutputDebugString(_wszDbg_x); }
#else
#define LOG_AND_DBG_MSG(_LOG_,_STR_) { WCHAR _wszDbg_x[MAX_PATH]; \
    StringCchPrintf(_wszDbg_x,MAX_PATH,L"%S:%d %s\n\r",__FUNCTION__,__LINE__,_STR_); \
    if(_LOG_) { _LOG_->Message(_wszDbg_x); } }
#endif

// This MACRO should be used for debug tracing but not error messages.
#ifdef DBG
#define DBG_MSG(_STR_) { WCHAR _wszDbg_x[MAX_PATH]; \
    StringCchPrintf(_wszDbg_x,MAX_PATH,L"%S:%d %s\n\r",__FUNCTION__,__LINE__,_STR_); \
    OutputDebugString(_wszDbg_x); }
#else
#define DBG_MSG(_STR_) 
#endif

// Note that this MACRO is for the User process to use while (if) it's under the lock.
// it will fill in the shared memory region with data that can be logged to the 
// test log from the Owner process. note that if the memory space already contains 
// an error, it will not overwrite that data. Thus, the first error in sticks.
#define USER_RET_ERR(_PMSG_,_STR_) { if( m_pSharedObject->UnderLock() ) { \
        if(!_PMSG_->bErrorLogged) { \
            _PMSG_->dwLine = __LINE__; \
            StringCchPrintf(_PMSG_->wszFunction,MAX_PATH,L"%S",__FUNCTION__); \
            StringCchPrintf(_PMSG_->wszData,MAX_PATH,L"%s",_STR_); \
            _PMSG_->bErrorLogged=true; } } }

//------------------------------------------------------------------------------

const unsigned int  VERTEXCOUNT   = 6;
const unsigned int  WIDTH		    = 320;
const unsigned int  HEIGHT		    = 240;
const SColor        FILL_COLOR	    = SColor(1.0f, 0.60f, 0.50f, 0.40f);

class IUserBase
{
public:
    virtual HRESULT Run()			  = 0;
    virtual HRESULT Initialize(BOOL bWindowed)       = 0;
    virtual HRESULT Cleanup()		  = 0;
    virtual ~IUserBase(){};
};

enum 
{
    eXProcTest_IndexBuffer_Open,
    eXProcTest_VertexBuffer_Open,        
    eXProcTest_Basic_CreateOpen,
    eXProcTest_Basic_ReadWrite,
    eXProcTest_InvalidHandle_DecrementOne,
    eXProcTest_InvalidHandle_IncrementOne,
    eXProcTest_InvalidHeightDimension,
    eXProcTest_InvalidWidthDimension,
    eXProcTest_InvalidFormat,
    eXProcTest_InvalidLevel,
    eXProcTest_OwnerGoesAway,
    eXProcTest_UserTriesToLock,
    eXProcTest_OwnerTriesToLock,        
    eXProcTest_ValidateTexture,
    eXProcTest_MultiUserScenario,
    eXProcTest_RoundRobin,
    eXProcTest_RoundRobinExt, //this is a special case, and shouldn't be parsed for in the ini
    eXProcTest_OpenClose_RaceCondition,    
    eXProcTest_MultipleAllocations,
    eXProcTest_FUSTest,
    eTotalTestCount,
    eXProcTest_CrossAdapterNotShare,
    eXProcTest_CrossSessionNotShare,
    eXProcTest_EvictSharedAllocation,
    eXProcTest_CrossProcSwizzlingRangeContention
};

// This structure defines the look of the shared memory region used by both the Owner
// and User processes.
typedef struct tMESSAGE
{
    DWORD dwTestId; 

    //shared texture information
    D3DKMT_HANDLE hTexHandle;
    UINT uiWidth;
    UINT uiHeight;
    UINT uiLevels;  
    D3DDDIFORMAT d3dFormat;     

    //shared vertex buffer information (currently not supported)
    D3DKMT_HANDLE hVBHandle;		
    UINT uiLength;
    DWORD dwFVF;
       
    LONG lCount;
    LONG lCount2;
    BOOL fReserved;

    HRESULT hrUser;

    // The information below this point is for error handling. If the User process
    // errors out for any reason, it will fill in data for the owner process to log.
    bool bErrorLogged;
    DWORD dwLine;
    WCHAR wszFunction[MAX_PATH];
    WCHAR wszData[MAX_PATH];

}Message, * PMessage;


// This structure holds the information about the shared memory region. The only
// way a caller can get this shared memory is to retrieve a pointer here.
typedef struct sSharedMemory
{
    sSharedMemory():
        m_hMapping(0),
        m_pSharedMemory(0),
        m_dwTimeOut(0),
        m_bIAmTheCurrentOwner(false),
        m_hMutex(NULL)
    {
        // State will be setup in Initialize().
    };
    ~sSharedMemory()
    {
        // First need to free up the shared mutex.
        if( m_hMutex )
        {
            CloseHandle(m_hMutex);
        }
        // Now remove our mapping.
        if( m_pSharedMemory )
        {
            UnmapViewOfFile(m_pSharedMemory);
            m_pSharedMemory = NULL;
        }
/*
        //CR: [davidfl] freeing the mapping here causes an access violation. Come back to this...
        // free the mapping.
        if( m_hMapping )
        {
            PATH_TRAP();
            CloseHandle(m_hMapping);
            m_hMapping = NULL;
        }
*/        
    }
    // Note that _psa can be NULL
    HRESULT Initialize(SECURITY_ATTRIBUTES* _psa,
                       DWORD _dwTimeOut)
    {
        HRESULT hr = S_OK;

        if( NULL != m_hMapping )
        {
            PATH_TRAP();
            DBG_MSG(L"Invalid second Initialize call");
            hr = TYPE_E_INVALIDSTATE;
            return hr;
        }

        //Create shared memory region for interprocess communication   
        m_hMapping = CreateFileMapping(INVALID_HANDLE_VALUE,
                                       _psa,
                                       PAGE_READWRITE,
                                       0,
                                       sizeof(Message),
                                       L"Global\\XProcResShare");
        if(m_hMapping == NULL)
        {
            PATH_TRAP();
            DBG_MSG(L"Unable to create shared file mapping");
            hr = E_OUTOFMEMORY;
            return hr;
        }   

        // Note: if the object already existed in memory, we'll get a pointer to it. But,
        // in that case, we do not want to re-initialize it because it should already
        // contain a valid mutex. So, as the documenation says, we need to check
        // GetLastError to see if it returns ERROR_ALREADY_EXISTS.
        DWORD dwError = GetLastError();

        m_pSharedMemory = (PMessage)MapViewOfFileEx(m_hMapping,FILE_MAP_ALL_ACCESS,0,0,0,0);
        if(m_pSharedMemory == NULL)
        {
            PATH_TRAP();
            DBG_MSG(L"Unable to map shared memory into process address space");
            hr = E_OUTOFMEMORY;
            return hr;
        }

        // Now only perform this code if the shared memory did not already exist.
        if( ERROR_ALREADY_EXISTS == dwError )
        {
//            PATH_TRAP();
        }
        else if ( ERROR_SUCCESS != dwError )
        {
            // Some random error?
            hr = E_OUTOFMEMORY;
            PATH_TRAP();
        }
        else
        {
            // Now that we have a NEW block of shared memory, zero it out
            ZeroMemory(m_pSharedMemory,sizeof(Message));
        }

        // Now for this block of shared memory, we need to allocate a mutex that
        // both processes can use. We'll store it in this shared memory block.
        m_hMutex = CreateMutex(NULL, FALSE, L"CrossProcMessageLock");
        if(NULL == m_hMutex)
        {
            PATH_TRAP();
            DBG_MSG(L"Unable to allocate Mutex");
            hr = E_OUTOFMEMORY;
            // can't get required memory
            return hr;
        }

        // Now set our timeout when acquiring the mutex.
        m_dwTimeOut = _dwTimeOut;

        return hr;
    };

    Message* AcquireLock()
    {
        // can only wait if we have a mutex.
        if( NULL == m_hMutex )
        {
            PATH_TRAP();
            return NULL;
        }

        // wait on mutex
        DWORD dwRet = WaitForSingleObject(m_hMutex, m_dwTimeOut);
        if(WAIT_OBJECT_0 != dwRet)
        {
            // we failed.
            if( dwRet == WAIT_ABANDONED )
            {
                DBG_MSG(L"Error: WaitForSingleObject returned WAIT_ABANDONED");
            }
            else if( dwRet == WAIT_TIMEOUT )
            {
                DBG_MSG(L"Error: WaitForSingleObject returned WAIT_TIMEOUT");
            }
            else if( dwRet == WAIT_FAILED )
            {
                DBG_MSG(L"Error: WaitForSingleObject returned WAIT_FAILED");
                DWORD dwGetLastError = GetLastError();
                PATH_TRAP();
            }
            else 
            {
                DBG_MSG(L"Error: WaitForSingleObject returned Unknown value");
            }
            PATH_TRAP();
            return NULL;
        }

        // when we hold the lock, we'll also set this true so we can validate our state
        // through our check calls.
        m_bIAmTheCurrentOwner=true;

        // else, we acquired the mutex so let's return a pointer to the caller
        return m_pSharedMemory;
    };

    // Upon the return, the caller's memory pointer will be NULL.
    void ReleaseLock(PMessage* _ppSharedMemory)
    {
        // did they pass back a pointer to our value? 
        if( ( _ppSharedMemory != NULL ) && (*_ppSharedMemory == m_pSharedMemory) )
        {
            // we're no longer holding the mutex, thus we give up our check.
            m_bIAmTheCurrentOwner=false;

            ReleaseMutex(m_hMutex);
            *_ppSharedMemory = NULL;
            // Want to validate that we still have a mutex handle.
            if( m_hMutex == NULL )
            {
                PATH_TRAP();
            }
        }
    };

    // This routines true if we're under our lock, false otherwise.
    bool UnderLock()
    {
        return m_bIAmTheCurrentOwner;
    }

private:
    HANDLE m_hMapping;
    PMessage m_pSharedMemory;
    DWORD m_dwTimeOut;
    bool m_bIAmTheCurrentOwner;
    HANDLE m_hMutex;

}SharedMemory, * PSharedMemory;


typedef struct tTESTCASE
{
    //Global Test Params
    BOOL fGlobal_BreakOnFail;

    //Test Cases
    BOOL fIB_Basic_CreateOpen;
    BOOL fVB_Basic_CreateOpen;
    BOOL fTex_Basic_CreateOpen;
    BOOL fTex_ReadWrite;
    BOOL fTex_InvalidHandle_DecrementOne;
    BOOL fTex_InvalidHandle_IncrementOne;
    BOOL fTex_InvalidHeightDimension;
    BOOL fTex_InvalidWidthDimension;
    BOOL fTex_InvalidFormat;
    BOOL fTex_InvalidLevel;
    BOOL fTex_OwnerGoesAway;
    BOOL fTex_UserTriesToLock;
    BOOL fTex_OwnerTriesToLock;    
    BOOL fTex_ValidateTexture;
    BOOL fTex_MultiUserScenario;
    BOOL fTex_OpenClose_RaceCondition;
    BOOL fTex_RoundRobin;
    BOOL fTex_MultipleAllocations;
    BOOL fTex_FUSTest;
    BOOL fTex_CrossAdapterNotShare;
    BOOL fTex_CrossSessionNotShare;
    BOOL fTex_EvictSharedAllocation;
    BOOL fTex_CrossProcSwizzlingRangeContention;


    //test specific data
    INT  nNum_OpenClose;
    INT  nNum_RoundRobins;
    INT  nNum_MultiUsers;
    INT  nNum_FUSSleepSeconds;
    INT  nNum_SwizzlingRangeContentionSecondsToRun;

}TESTCASE, * PTESTCASE;


struct MyVertex
{
    float Pos[4];	
    float Tex[2];
};
const DWORD MYFVF = (D3DFVF_XYZRHW | D3DFVF_TEX1);


#endif