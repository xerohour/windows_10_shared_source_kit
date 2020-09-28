/******************************Module*Header*******************************
* Module Name: listener.cpp
*
* Implementation of code that recieves kernel profile events (via ETW)
*
* Author: David Flenniken [davidfl]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
//---
#include <etwConsumersRequire.h>

#include <ntwmi.h>

#include <dbghelp.h>

#include <vector>
#include <list>
#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>

#include "NTKernelEventHelper.h"
#include "NTKernelConsumer.h"
#include "NTKernelLegConsumer.h"

#include "generatepnpstop.h"
#include "basevid.h"

#include "AdapterUtil.h"
using namespace DevX::GRFX;

// Use this to get debug info out of the binary at runtime.                
//#define WANT_DBG

C_ASSERT(sizeof(PVOID64) == 8);
C_ASSERT(sizeof(ULONG32) == 4);
C_ASSERT(sizeof(ULONG64) == 8);

// The default sampling size is set here. We need to be able to clear some small number
// of events to say that we've actually walked some stacks
#define MINIMUM_SAMPLING_SET 400
#define MAX_LOGGED_LAYERING_ERRORS 100

// if true, we're testing WDDM1.1 - only check kernel components
extern bool gbWDDM1_1;
// if true, we're testing WDDM1.2 - check user and kernel compoenents
extern bool gbWDDM1_2;
// if true, check user and kernel compoenents as well as detect driver layering
extern bool gbDriverLayering;
// if gbDriverLayering is true and gbInternal is false, don't fail due to FPO check violations
extern bool gbInternal; 
extern bool gbDebugStacks;
extern bool gbAllDrivers;
extern bool gbLayering;	// check for layering violations


class CKernelListener; 


typedef struct sThreadMapInfo
{
public:
    sThreadMapInfo(ULONG _ulThreadId,
                   ULONG64 _ulWin32StartAddr64,
                   ULONG64 _ul64tebBase,
                  bool _bKernel);
    ~sThreadMapInfo();
    bool IsKernelModeThread();
    ULONG GetThreadId();
    ULONG64 GetWin32StartAddr64();
    ULONG64 GetTebBase();
    void IncrementSuccessfulCount();
    void IncrementFailedCount();
    ULONG GetSuccessfulWalks();
    ULONG GetFailedWalks();

private:
    ULONG s_ulThreadId;
    bool s_bKernel;
    ULONG64 s_ulWin32StartAddr64;
    ULONG64 s_ul64tebBase;
    ULONG s_ulCountSuccessfulWalks;
    ULONG s_ulCountFailedWalks;

} THREADMAPINFO;


//
// This little structure is used to store counts. As we process Stackwalk events
// we will track all events, sucessful parsings, failed parsings and events that
// arrive before we're setup to parse.
//
typedef struct _CallCount
{
    ULONG ulStackWalk;
    ULONG ulToEarly;
    ULONG ulWalkSucceeded;
    ULONG ulWalkFailed;
    ULONG ulKernelThreadWithDriver;
    ULONG ulDriverLayering;
} CALLCOUNT;

typedef struct sModuleData
{
public:
    sModuleData(WCHAR* _pwszModule, WCHAR* _pwszComponentName, bool _bOsComponent);
    ~sModuleData();
    WCHAR* GetModuleName();
    WCHAR* GetComponentName();
    void SetModuleAddresses(ULONG64 _ProcessId, ULONG64 _Base, ULONG64 _End);
    ULONG64 GetProcessId();
    ULONG64 GetModuleBase();
    ULONG64 GetModuleEnd();
    bool IsModule(WCHAR* _pwszMatch);
    bool IsOsComponent();
    bool IsSymLoaded();
    void SymLoaded(bool _bLoaded);

    HMODULE m_hMod;
private:
    WCHAR m_wszModuleName[MAX_PATH];
    WCHAR m_wszComponentName[MAX_PATH];	// friendly name of the component this module is part of
    ULONG64 m_ul64ImageBase;
    ULONG64 m_ul64ImageEnd;

    ULONG64 m_ul64ProcessId;

    bool m_bOsComponent; // true if MS component, false if 3'rd party driver component
    bool m_bSymLoaded; // true if the symbols for this module have been loaded.

} MODULEDATA;

typedef struct sStackItem
{
public:
    // For 32 bit values
    sStackItem(PSTACK_WALK_EVENT_DATA32 _pStackWalkData,
               UINT _AddressCount);
    // for 64 bit values.
    sStackItem(PSTACK_WALK_EVENT_DATA64 _pStackWalkData,
               UINT _AddressCount);
    ~sStackItem();
    void LogItem(CLogWriter* _pLog, CKernelListener* _pKernelListener);
    ULONG64 GetThreadId();
private:
    ULONG64* m_ul64Ptr;
    UINT m_uiCount;
    ULONG64 m_ul64ProcessId;
    ULONG64 m_ul64ThreadId;
} STACKITEM;




class CKernelListener : public CNTKernelConsumer
{
public:
    CKernelListener(CLogWriter* _pLog);

    void ThreadGuid_Handler(PEVENT_RECORD pEvent);
    bool IsKernelModeStack(ULONG ulThreadId,
                           THREADMAPINFO** _pptd);
    void StackWalkGuid_Handler(PEVENT_RECORD pEvent);
    virtual void ImageLoadGuid_Handler(PEVENT_RECORD pEvent);

    CALLCOUNT* CallCount();
    void AddModule(WCHAR* _lpwszModule, WCHAR* _lpwszComponentName, bool _bOs,CLogWriter* _pLog);
    bool IsAddressInDriver(ULONG64 _ul64Address);
    WCHAR* GetModuleNameAndSymbol(ULONG64 _ul64Address,bool* _pbDxgkrnl);
    WCHAR* GetDriverSymbol(MODULEDATA* _pmd,ULONG64 _ul64Address);
    UINT StackWalkCount();
    void LogStacks(CLogWriter* _pLog,ULONG _ulThreadId);
    void LogModules(CLogWriter* _pLog);
    std::map<ULONG,THREADMAPINFO*>* GetThreadMap();
    WCHAR* GetModuleName(ULONG64 _ul64Address);
    WCHAR* GetModuleComponentName(ULONG64 _ul64Address);
    bool WaitForModuleData(UINT Time);

private:
    CALLCOUNT m_Count;
    std::list<MODULEDATA*> m_pModuleList;
    std::list<STACKITEM*> m_StackList;
    bool m_bAllModulesLoaded;
    CLogWriter* m_pLog;

    // used to track threads. key is the thread id
    std::map<ULONG,THREADMAPINFO*> m_pThreadMap;
};

//
// StackItem structure is used each time we want to save a failed stackwalk. After
// we've processed everything, this list will get dumped into the test log file.
//
// For 32 bit values
sStackItem::sStackItem(PSTACK_WALK_EVENT_DATA32 _pStackWalkData,
           UINT _AddressCount)
{
    m_ul64Ptr = new ULONG64[_AddressCount];
    if( m_ul64Ptr )
    {
        // Copy each item
        for(UINT i=0;i<_AddressCount;i++)
        {
            m_ul64Ptr[i]=(ULONG64)_pStackWalkData->Addresses[i];
        }

        m_uiCount = _AddressCount;
        m_ul64ProcessId = (ULONG64)_pStackWalkData->ProcessId;
        m_ul64ThreadId = (ULONG64)_pStackWalkData->ThreadId;
    }
}
// for 64 bit values.
sStackItem::sStackItem(PSTACK_WALK_EVENT_DATA64 _pStackWalkData,
           UINT _AddressCount)
{
    m_ul64Ptr = new ULONG64[_AddressCount];
    if( m_ul64Ptr )
    {
        // Copy each item
        for(UINT i=0;i<_AddressCount;i++)
        {
            m_ul64Ptr[i]=_pStackWalkData->Addresses[i];
        }

        m_uiCount = _AddressCount;
        m_ul64ProcessId = (ULONG64)_pStackWalkData->ProcessId;
        m_ul64ThreadId = (ULONG64)_pStackWalkData->ThreadId;
    }
}
sStackItem::~sStackItem()
{
    delete m_ul64Ptr;
}


void sStackItem::LogItem(CLogWriter* _pLog, CKernelListener* _pKernelListener)
{
    WCHAR* pwszDriverModuleName = NULL;
    WCHAR wszLog[132];
    if( m_ul64Ptr )
    {
        StringCchPrintf(wszLog,132,L"Index: Stack, depth %d",m_uiCount);
        if( gbDebugStacks )
        {
            // User asked for this info on the command line
            OutputDebugString(wszLog);
            OutputDebugString(L"\n\r");
        }
        _pLog->Message(wszLog);
        bool bDxgkrnl=false;
        for(UINT i=0;i<m_uiCount;i++)
        {
            // Query for the module name and symbol (or offset)
            pwszDriverModuleName = _pKernelListener->GetModuleNameAndSymbol(m_ul64Ptr[i],&bDxgkrnl);

            if( pwszDriverModuleName )
            {
                StringCchPrintf(wszLog,132,L"%04d: %I64x Pid=%I64d, Tid=%I64d, %s",
                                i,m_ul64Ptr[i],m_ul64ProcessId,m_ul64ThreadId, pwszDriverModuleName);

                // Now free the pointer
                delete pwszDriverModuleName;
            }
            else
            {
                StringCchPrintf(wszLog,132,L"%04d: %I64x Pid=%I64d, Tid=%I64d",
                                i,m_ul64Ptr[i],m_ul64ProcessId,m_ul64ThreadId);
            }
            if( gbDebugStacks )
            {
                // User asked for this info on the command line
                OutputDebugString(wszLog);
                OutputDebugString(L"\n\r");
            }
            _pLog->Message(wszLog);
        }
    }
}
ULONG64 sStackItem::GetThreadId()
{
    return m_ul64ThreadId;
}

//
// The ModuleData structure is used to store the names and addresses of different 
// modules (drivers) that will be used during the processing of stackwalk events.
//
sModuleData::sModuleData(WCHAR* _pwszModule,
                         WCHAR* _pwszComponentName,
                         bool _bOsComponent):
    m_ul64ImageBase(0),
    m_ul64ImageEnd(0),
    m_bOsComponent(_bOsComponent),
    m_ul64ProcessId(0),
    m_bSymLoaded(false)
{
    m_wszModuleName[0] = NULL;
    if( NULL != _pwszModule )
    {
       StringCchCopy(m_wszModuleName,MAX_PATH,_pwszModule);
    }

    ZeroMemory(m_wszComponentName, sizeof(m_wszComponentName));
    if(NULL != _pwszComponentName)
    {
       StringCchCopy(m_wszComponentName,MAX_PATH,_pwszComponentName);
    }

#ifdef WANT_DBG
    WCHAR wszDbg[256];
    StringCchPrintf(wszDbg,256,L"Constructor: %s: Component: %s, Id:0x%X, Base:0x%X, End:0x%X, %s, %s\n\r",
                    m_wszModuleName,
                    m_wszComponentName,
                    (ULONG)m_ul64ProcessId,
                    (ULONG)m_ul64ImageBase,
                    (ULONG)m_ul64ImageEnd,
                    (m_bOsComponent?L"Oskernel":L"Driver"),
                    //(m_bKernel?L"sys":L"dll"),
                    (m_bSymLoaded?L"symloaded":L"symNOTloaded"));
    OutputDebugString(wszDbg);
#endif
}
sModuleData::~sModuleData(){}
WCHAR* sModuleData::GetModuleName()
{
    return m_wszModuleName;
}
WCHAR* sModuleData::GetComponentName()
{
    return m_wszComponentName;
}
void sModuleData::SetModuleAddresses(ULONG64 _ProcessId,
                                     ULONG64 _Base, 
                                     ULONG64 _End)
{
    m_ul64ImageBase=_Base;
    m_ul64ImageEnd=_End;

    // if our process ID comes in as zero, we're going to set it to negative two if it's also
    // a kernel component.
    if( 0 == _ProcessId ) //&& m_bKernel
    {
        m_ul64ProcessId=0x1F1F1F1F; // brown handle
    }
    else
    {
        m_ul64ProcessId = _ProcessId;
    }
#ifdef WANT_DBG
    WCHAR wszDbg[256];
    StringCchPrintf(wszDbg,256,L"SetModuleAddresses: %s: Id:0x%X, Base:0x%X, End:0x%X, %s, CurrentProcessId=0x%X\n\r",
                    m_wszModuleName,
                    (ULONG)m_ul64ProcessId,
                    (ULONG)m_ul64ImageBase,
                    (ULONG)m_ul64ImageEnd,
                    (m_bOsComponent?L"Oskernel":L"Driver"),
                    GetCurrentProcessId());
    OutputDebugString(wszDbg);
#endif
}
ULONG64 sModuleData::GetModuleBase()
{
    return m_ul64ImageBase;
}
ULONG64 sModuleData::GetModuleEnd()
{
    return m_ul64ImageEnd;
}
ULONG64 sModuleData::GetProcessId()
{
    return m_ul64ProcessId;
}
bool sModuleData::IsSymLoaded()
{
    return m_bSymLoaded;
}
void sModuleData::SymLoaded(bool _bLoaded)
{
    m_bSymLoaded = _bLoaded;
}

bool sModuleData::IsModule(WCHAR* _pwszMatch)
{
    if( NULL != _pwszMatch )
    {
        if( wcsstr(_pwszMatch,m_wszModuleName) )
        {
            return true;
        }
    }
    return false;
}
bool sModuleData::IsOsComponent()
{
    return m_bOsComponent;
}



sThreadMapInfo::sThreadMapInfo(ULONG _ulThreadId,
               ULONG64 _ulWin32StartAddr64,
               ULONG64 _ul64tebBase,
              bool _bKernel):
    s_ulThreadId(_ulThreadId),
    s_bKernel(_bKernel),
    s_ulWin32StartAddr64(_ulWin32StartAddr64),
    s_ul64tebBase(_ul64tebBase),
    s_ulCountSuccessfulWalks(0),
    s_ulCountFailedWalks(0)
{
}
sThreadMapInfo::~sThreadMapInfo()
{
}
bool sThreadMapInfo::IsKernelModeThread()
{
    return s_bKernel;
}
ULONG sThreadMapInfo::GetThreadId()
{
    return s_ulThreadId;
}
ULONG64 sThreadMapInfo::GetWin32StartAddr64()
{
    return s_ulWin32StartAddr64;
}
ULONG64 sThreadMapInfo::GetTebBase()
{
    return s_ul64tebBase;
}
void sThreadMapInfo::IncrementSuccessfulCount()
{
    s_ulCountSuccessfulWalks++;
}
void sThreadMapInfo::IncrementFailedCount()
{
    s_ulCountFailedWalks++;
}
ULONG sThreadMapInfo::GetSuccessfulWalks()
{
    return s_ulCountSuccessfulWalks;
}
ULONG sThreadMapInfo::GetFailedWalks()
{
    return s_ulCountFailedWalks;
}
                 


//
// This class impliments the virtual functions from CKERNELListener (in etw library).
// The key routines are the StackWalk() and ImageLoad() routines that get the module
// addresses and parse the stackwalk events.
//
CKernelListener::CKernelListener(CLogWriter* _pLog)
{
    m_bAllModulesLoaded = false;
    m_Count.ulStackWalk = 0;
    m_Count.ulToEarly =0;
    m_Count.ulWalkSucceeded = 0;
    m_Count.ulWalkFailed = 0;
    m_Count.ulKernelThreadWithDriver = 0;
    m_Count.ulDriverLayering = 0;
    m_pLog = _pLog;
}

void CKernelListener::ThreadGuid_Handler(PEVENT_RECORD pEvent)
{
    // Create a map of all threads. All we really want to monitor is thread start
    // and thread stop events.
    CNTKernelEventHelper EH(pEvent);

    switch(EH.Opcode() | EVENT_TRACE_GROUP_THREAD)
    {
        case WMI_LOG_TYPE_THREAD_DC_START:
        case WMI_LOG_TYPE_THREAD_CREATE:
        case WMI_LOG_TYPE_THREAD_DC_END:
            {
                // thread is starting add it to our list

                ULONG ulPointerSize = GetPointerSize();
                sWMI_EXTENDED_THREAD_INFORMATION sData(EH.GetUserData(),EH.GetUserDataLength(),ulPointerSize);

                std::map<ULONG, THREADMAPINFO*>::iterator It = m_pThreadMap.find(sData.ThreadId);
                if(It == m_pThreadMap.end())
                {
                    // we didn't find it, thus add it.
                    THREADMAPINFO* ptd = new THREADMAPINFO(sData.ThreadId,
                                                           sData.Win32StartAddr64,
                                                           sData.TebBase64,
                                                           (0==sData.TebBase64?true:false));
                    m_pThreadMap[sData.ThreadId] = ptd;
                }
            }
            break;

        case WMI_LOG_TYPE_THREAD_DELETE:
            break;

        case PERFINFO_LOG_TYPE_CONTEXTSWAP:
            {
                // swap in progress - ignore
            }
            break;
        case PERFINFO_LOG_TYPE_READY_THREAD:
            {
                // Thread Ready - ignore
            }
            break;
    }
}

// takes a thread Id and outputs the ThreadMapInfo pointer associated with it
// and returns true of this is a kernel thread.
bool CKernelListener::IsKernelModeStack(ULONG ulThreadId,
                       THREADMAPINFO** _pptd)
{
    THREADMAPINFO* ptd = NULL;
    ULONG64 ul64tebBase=-1;
    // Find the associated thread
    ULONG64 ul64StartAddr=0;
    std::map<ULONG, THREADMAPINFO*>::iterator It = m_pThreadMap.find(ulThreadId);
    if(It != m_pThreadMap.end())
    {
        // We found our entry, update it's count.
        ptd = (*It).second;
        ul64StartAddr = ptd->GetWin32StartAddr64();
        ul64tebBase = ptd->GetTebBase();
    }
    // now write back the pointer if we have one.
    *_pptd = ptd;

    // Find the associated thread so we can get the thread start address and the TebBase.
    // if the thread start address is in the video driver and the tebBase is zero
    // then we know that this is a kernel mode thread created by the video driver.
    if( IsAddressInDriver(ul64StartAddr) && (0 == ul64tebBase) )
    {
        return true;
    }
    return false;
}


// When tracking stackwalk events, we will walk every stack to the end. What this means
// is that we should always be able to walk the stack back to an OS component that launched
// the thread - like ntdll.dll for user mode threads. For kernel mode threads there are cases
// where we won't be able to walk back to an OS component - if the thread is a kernel mode
// thread.
//
// Also note that the OS breaks up stackwalk events into kernel stacks and user mode stacks.
// we should see the kernel mode stack event before the user mode stack event. but order should
// not be important.
//
// The basic idea here is that we'll either get a user mode or kernel mode stackwalk event.
// we'll walk that stack looking for a module address of the drivers that we're tracking. if 
// we find a driver address, we'll then look for the opposite side of the stack. If we find it,
// the walk passes. If we don't, the walk fails.
void CKernelListener::StackWalkGuid_Handler(PEVENT_RECORD pEvent)
{
    // Skip all stack walks that come before all modules are loaded
    if(!m_bAllModulesLoaded)
        return;

    m_Count.ulStackWalk++;

    CNTKernelEventHelper EH(pEvent);

    // Can't do any real work until we've processed the ImageLoad event that has
    // the address locations for our drivers.
    if( 0 == m_pModuleList.size() )
    {
        m_Count.ulToEarly++;
        return;
    }

    bool bFoundVideoDriver=false;
    bool bFoundOtherSideOfStack=false;
    bool bFoundEndOfStack=false;
    bool bKernelThreadInVideoDriver=false;

    ULONG MofLength = EH.GetUserDataLength();
    ULONG ulPointerSize = GetPointerSize();
    UINT AddressCount;
    THREADMAPINFO* ptd = NULL;
    WCHAR* pCurrentModule = NULL;
    WCHAR* pPrevModule = NULL;
    bool bSuccess=true;

    if( 4 == ulPointerSize )
    {
        // the pStackWalkData->Address[i] is a ULONG32 value.
        PSTACK_WALK_EVENT_DATA32 pStackWalkData = ((PSTACK_WALK_EVENT_DATA32)EH.GetUserData());
        AddressCount = (MofLength - (sizeof(STACK_WALK_EVENT_DATA32) - sizeof(ULONG32))) / sizeof(ULONG32);

        bKernelThreadInVideoDriver = IsKernelModeStack(pStackWalkData->ThreadId,&ptd);
        if( bKernelThreadInVideoDriver )
        {
            m_Count.ulKernelThreadWithDriver++;
        }

        // walk the stack.
        UINT i;
        for(i=0;i<AddressCount;i++)
        {
            pCurrentModule = GetModuleComponentName((ULONG64)pStackWalkData->Addresses[i]);

            // All we should have to do is see if this address is in our module.
            if( bFoundVideoDriver )
            {
                if( !IsAddressInDriver((ULONG64)pStackWalkData->Addresses[i]) )
                {
                    // we made it through the video driver successfully.
                    bFoundOtherSideOfStack=true;
                    break;
                }
                else
                {
                    if(gbDriverLayering)
                    {
                        // We are in the driver and calling into a driver. Make sure it's the same driver
                        if(!pCurrentModule || 0 != _wcsicmp(pCurrentModule, pPrevModule))
                        {
                                bSuccess=false;
                                m_Count.ulDriverLayering++;

                                WCHAR wszDbg[256];
                                StringCchPrintf(wszDbg,256,L"Driver Layering detected: %s is calling %s\n",  pCurrentModule, pPrevModule);

                                if(m_Count.ulDriverLayering < MAX_LOGGED_LAYERING_ERRORS)
                                    m_pLog->Message(wszDbg);
#ifdef DBG
                                OutputDebugString(wszDbg);
#endif
                        }
                    }
                }
            }
            else
            {
                // is the address in our module?
                if( IsAddressInDriver((ULONG64)pStackWalkData->Addresses[i]) )
                {
                    bFoundVideoDriver=true;

/*
#ifdef DBG
                        WCHAR wszDbg[256];
                        if( pStackWalkData->Addresses[i] < 0x80000000 )
                        {
                            // User mode address.
                            PATH_TRAP();
                            StringCchPrintf(wszDbg,256,L"User mode driver address: 0x%X\n\r",pStackWalkData->Addresses[i]);
                        }
                        else
                        {
                            PATH_TRAP();
                            StringCchPrintf(wszDbg,256,L"Kernel mode driver address: 0x%X\n\r",pStackWalkData->Addresses[i]);
                        }
                        OutputDebugString(wszDbg);
#endif
*/
                }
            }

            pPrevModule = pCurrentModule;
        }

        // If we walked through all the addresses and we found our video driver
        // but not the other side of the stack, we'll check to see if this is a kernel
        // mode thread AND it was created by the video driver.
        if( (i == AddressCount) & bFoundVideoDriver & !bFoundOtherSideOfStack)
        {
            // we have a kernel mode thread Started by the video driver
            if( bKernelThreadInVideoDriver )
            {
                // that was created by our Video driver
                if( IsAddressInDriver((ULONG64)pStackWalkData->Addresses[AddressCount-1]))
                {
                    // and the last address on the stack is also in the video driver.
                    bFoundEndOfStack=true;
                }
            }
        }
    }
    else if( 8 == ulPointerSize )
    {
        // else it's 64 bit addresses.
        PSTACK_WALK_EVENT_DATA64 pStackWalkData = ((PSTACK_WALK_EVENT_DATA64)EH.GetUserData());
        AddressCount = (MofLength - (sizeof(STACK_WALK_EVENT_DATA64) - sizeof(ULONG64))) / sizeof(ULONG64);

        bKernelThreadInVideoDriver = IsKernelModeStack(pStackWalkData->ThreadId,&ptd);
        if( bKernelThreadInVideoDriver )
        {
            m_Count.ulKernelThreadWithDriver++;
        }

        UINT i;
        for(i=0;i<AddressCount;i++)
        {
            pCurrentModule = GetModuleComponentName((ULONG64)pStackWalkData->Addresses[i]);

            // All we should have to do is see if this address is in our module.
            if( bFoundVideoDriver )
            {
                if( !IsAddressInDriver((ULONG64)pStackWalkData->Addresses[i]) )
                {
                    // we made it through the video driver successfully.
                    bFoundOtherSideOfStack=true;
                    break;
                }
                else
                {
                    if(gbDriverLayering)
                    {
                        // We are in the driver and calling into a driver. Make sure it's the same driver
                        if(!pCurrentModule || 0 != _wcsicmp(pCurrentModule, pPrevModule))
                        {
                                bSuccess=false;
                                m_Count.ulDriverLayering++;

                                WCHAR wszDbg[256];
                                StringCchPrintf(wszDbg,256,L"Driver Layering detected: %s is calling %s\n",  pCurrentModule, pPrevModule);

                                if(m_Count.ulDriverLayering < MAX_LOGGED_LAYERING_ERRORS)
                                    m_pLog->Message(wszDbg);
#ifdef DBG
                                OutputDebugString(wszDbg);
#endif
                        }
                    }
                }
            }
            else
            {
                // is the address in our module?
                if( IsAddressInDriver(pStackWalkData->Addresses[i]) )
                {
                    bFoundVideoDriver=true;
                }
            }

            pPrevModule = pCurrentModule;
        }

        // If we walked through all the addresses and we found our video driver
        // but not the other side of the stack, we'll check to see if this is a kernel
        // mode thread AND it was created by the video driver.
        if( (i == AddressCount) & bFoundVideoDriver & !bFoundOtherSideOfStack)
        {
            // we have a kernel mode thread Started by the video driver
            if( bKernelThreadInVideoDriver )
            {
                // that was created by our Video driver
                if( IsAddressInDriver((ULONG64)pStackWalkData->Addresses[AddressCount-1]))
                {
                    // and the last address on the stack is also in the video driver.
                    bFoundEndOfStack=true;
                }
            }
        }
    }
    else
    {
        // Unknown size, nothing we can do here.
        return;
    }

    // Did it pass our test?
//#define TEST_DBG
#ifdef TEST_DBG
    // to test the code, we're going to fake failures
    if( (m_Count.ulStackWalk > 50000) && (m_Count.ulStackWalk < 52000) ) 
    {
        // Pretend that all the events in this range failed.
        bFoundEndOfStack=false;
        bFoundOtherSideOfStack=false;
    }
#endif

    // Now, before we go looking for the next event, did we find what we're looking
    // for? The idea here is did we walk to the end of the stack?  If so, success.
    // Did we find the video driver on the stack? If so, did we get through it?
    if( !bFoundEndOfStack )
    {
        // we did not find the end of the stack. Did we find the video driver?
        if( bFoundVideoDriver && (AddressCount > 1) )
        {
            // we found the video driver, were we able to walk to the other side?
            if( bFoundOtherSideOfStack )
            {
                // Good.
                m_Count.ulWalkSucceeded++;
            }
            else
            {
                // don't fail for Layering test if it is not run internally. Layering test enumerates more binaries then the original FPOCheck test
                // we don't want partners to suddenly start seeing failures where the test passed before
                if(!gbDriverLayering || gbInternal)
                {
                    // This is not good!  We were unable to walk to the other side.
                    // Here is where we report an error. If we were not able to walk through
                    // the driver to find OS code on the other side, the OS is not able to
                    // stackwalk that component at that time.
                    bSuccess=false;
                }
            }
        }
        // else we don't care - we did not find the end of the stack or a video driver
        // on the stack, thus we don't count this.
    }
    else
    {
        // This is always a good thing.

        // If we also found the video driver, then we'll count a success.
        if( bFoundVideoDriver && (AddressCount > 1) )
        {
            m_Count.ulWalkSucceeded++;
#ifdef WANT_TEST_DBG
            // By setting bSuccess to false here, we've just pretended to fail a stack
            // with the video driver on it. This allows for testing the symbol resolution 
            // code.
            bSuccess=false;
#endif
        }
    }

    if( !bSuccess )
    {
        m_Count.ulWalkFailed++;

        // Now update our ThreadMap so as to indicate that this thread has been unsuccessful 
        if( ptd )
        {
            ptd->IncrementFailedCount();
        }

        // Everytime a walk fails, we need to save the module address that was found 
        // on the stack along with the length of the stack.

        if( 4 == ulPointerSize )
        {
            PSTACK_WALK_EVENT_DATA32 pStackWalkData = ((PSTACK_WALK_EVENT_DATA32)EH.GetUserData());
            AddressCount = (MofLength - (sizeof(STACK_WALK_EVENT_DATA32) - sizeof(ULONG32))) / sizeof(ULONG32);
#ifdef WANT_DBG_VERBOSE
            OutputDebugString(L"Stack:\n\r");
            WCHAR wszDbg[256];
            for(UINT i=0;i<AddressCount;i++)
            {
                StringCchPrintf(wszDbg,256,L"0x%X\n\r",pStackWalkData->Addresses[i]);
                OutputDebugString(wszDbg);
                wszDbg[0]=NULL;
            }

            OutputDebugString(L"\n\r");
#endif
            // Allocate a block of memory that will hold the addresses, copy them over and then
            // new a structure of type StackItem with the data.
            STACKITEM* pStackItem = new STACKITEM(pStackWalkData,AddressCount);
            m_StackList.push_back(pStackItem);
        }
        else if( 8 == ulPointerSize )
        {
            PSTACK_WALK_EVENT_DATA64 pStackWalkData = ((PSTACK_WALK_EVENT_DATA64)EH.GetUserData());
            AddressCount = (MofLength - (sizeof(STACK_WALK_EVENT_DATA64) - sizeof(ULONG64))) / sizeof(ULONG64);

            // Allocate a block of memory that will hold the addresses, copy them over and then
            // new a structure of type StackItem with the data.
            STACKITEM* pStackItem = new STACKITEM(pStackWalkData,AddressCount);
            m_StackList.push_back(pStackItem);
        }
    }
    else
    {
        // Now update our ThreadMap so as to indicate that this thread has been successful 
        if( ptd )
        {
            ptd->IncrementSuccessfulCount();
        }
    }
}


bool CKernelListener::WaitForModuleData(UINT Time)
{
    bool ModuleDataLoaded = true;

    // Not all driver binaries will be loaded, just wait a reasonable amount of time (few seconds) for all binaries
    // that are going to be loaded to be loaded.
    Sleep(Time);
    m_bAllModulesLoaded = ModuleDataLoaded;

    return ModuleDataLoaded;
}

//
// Here, what we want to get is the base and extent of the modules that we're looking for.
// When it comes to processing the stackwalk events, we need these addresses.
//
void CKernelListener::ImageLoadGuid_Handler(PEVENT_RECORD pEvent)
{
    CNTKernelEventHelper EH(pEvent);

    ULONG ulPointerSize = GetPointerSize();
    if( 4 == ulPointerSize )
    {
        WMI_IMAGELOAD_INFORMATION32* pImageLoadInfo = ((WMI_IMAGELOAD_INFORMATION32*)EH.GetUserData());

        std::list<MODULEDATA*>::iterator it;
        for( it = m_pModuleList.begin(); it != m_pModuleList.end(); ++it)
        {
            MODULEDATA* pmd = (*it);

            if(pmd->IsModule((WCHAR*)pImageLoadInfo->FileName) )
            {
#ifdef WANT_DBG
                // Debug dump the data.
                WCHAR wszDbg[132];
                StringCchPrintf(wszDbg,132,L"ImageLoadGuid: %s(%d)\n\r",
                                (EH.IsStart()?L"Start":
                    (EH.IsStop()?L"Stop":
                    (EH.IsInfo()?L"info":
                        (EH.IsDCStart()?L"DCStart":L"Other")))),
                                EH.Opcode());
                OutputDebugString(wszDbg);
                StringCchPrintf(wszDbg,132,L"ImageLoadGuid: Module Found, processid=0x%X base=0x%X, End=0x%X, %s\n\r",
                                pImageLoadInfo->ProcessId,
                                pImageLoadInfo->ImageBase32,
                                (pImageLoadInfo->ImageBase32+pImageLoadInfo->ImageSize32),
                                (WCHAR*)pImageLoadInfo->FileName);
                OutputDebugString(wszDbg);
#endif
                pmd->SetModuleAddresses((ULONG64)pImageLoadInfo->ProcessId,
                                        (ULONG64)pImageLoadInfo->ImageBase32,
                                        (ULONG64)(pImageLoadInfo->ImageBase32+pImageLoadInfo->ImageSize32));
                break;
            }
        }
    }
    else if( 8 == ulPointerSize )
    {
        // pointer size 64 bit
        WMI_IMAGELOAD_INFORMATION64 *pImageLoadInfo = ((WMI_IMAGELOAD_INFORMATION64*)EH.GetUserData());

        std::list<MODULEDATA*>::iterator it;
        for( it = m_pModuleList.begin(); it != m_pModuleList.end(); ++it)
        {
            MODULEDATA* pmd = (*it);

            if(pmd->IsModule((WCHAR*)pImageLoadInfo->FileName) )
            {
#ifdef WANT_DBG
                // Debug dump the data.
                WCHAR wszDbg[132];
                StringCchPrintf(wszDbg,132,L"ImageLoadGuid: Module Found, base=0x%I64x, End=0x%I64x, %s\n\r",
                                pImageLoadInfo->ImageBase64,
                                (pImageLoadInfo->ImageBase64+pImageLoadInfo->ImageSize64),
                                (WCHAR*)pImageLoadInfo->FileName);
                OutputDebugString(wszDbg);
#endif
                pmd->SetModuleAddresses((ULONG64)pImageLoadInfo->ProcessId,
                                        pImageLoadInfo->ImageBase64,
                                        (pImageLoadInfo->ImageBase64+pImageLoadInfo->ImageSize64));
                break;
            }
        }
    }
    else
    {
        return;
    }
}



CALLCOUNT* CKernelListener::CallCount()
{
    return &m_Count;
}

void CKernelListener::AddModule(WCHAR* _lpwszModule,
                                WCHAR* _lpwszComponentName,
                                bool _bOs,
                                CLogWriter* pLog)
{
    // we're looking for the same module name.
    bool bSame=false;
    std::list<MODULEDATA*>::iterator It = m_pModuleList.begin();
    while( It != m_pModuleList.end() )
    {
        MODULEDATA* pmd = (*It);

        if( 0 == _wcsicmp(_lpwszModule,pmd->GetModuleName()))
        {
            bSame = true;
        }
        It++;
    }

    // need to search the list and make sure it's not there before adding another.

    if( !bSame )
    {
        MODULEDATA* pmd = new MODULEDATA(_lpwszModule,_lpwszComponentName,_bOs);

        m_pModuleList.push_back(pmd);
        // Debug dump the data.
        WCHAR wszDbg[132];
        StringCchPrintf(wszDbg,132,L"AddModule: %s, Component Name - %s\n\r",_lpwszModule, _lpwszComponentName);
#ifdef WANT_DBG
        OutputDebugString(wszDbg);
#endif
        pLog->Message(wszDbg);
    }
    else
    {
        // Debug dump the data.
        WCHAR wszDbg[132];
        StringCchPrintf(wszDbg,132,L"NOT ADDING DUPLICATE Module: %s\n\r",_lpwszModule);
#ifdef WANT_DBG
        OutputDebugString(wszDbg);
#endif
        pLog->Message(wszDbg);
    }
}

bool CKernelListener::IsAddressInDriver(ULONG64 _ul64Address)
{
    bool bRet=false;

    std::list<MODULEDATA*>::iterator it;
    for( it = m_pModuleList.begin(); it != m_pModuleList.end(); ++it)
    {
        MODULEDATA* pmd = (*it);

        // we're looking for non OS components.
        if( !pmd->IsOsComponent() )
        {
            if( _ul64Address != 0 && 
                _ul64Address >= pmd->GetModuleBase() && 
                _ul64Address <= pmd->GetModuleEnd() )
            {
                // found it
                bRet = true;
                break;
            }
        }
    }
    return bRet;
}

WCHAR* CKernelListener::GetModuleName(ULONG64 _ul64Address)
{
    std::list<MODULEDATA*>::iterator it;
    for( it = m_pModuleList.begin(); it != m_pModuleList.end(); ++it)
    {
        MODULEDATA* pmd = (*it);

        if( _ul64Address != 0 && 
            _ul64Address >= pmd->GetModuleBase() && 
            _ul64Address <= pmd->GetModuleEnd() )
        {
            // found it
            return pmd->GetModuleName();
        }
    }
    return NULL;
}

WCHAR* CKernelListener::GetModuleComponentName(ULONG64 _ul64Address)
{
    std::list<MODULEDATA*>::iterator it;
    for( it = m_pModuleList.begin(); it != m_pModuleList.end(); ++it)
    {
        MODULEDATA* pmd = (*it);

        if( _ul64Address != 0 && 
            _ul64Address >= pmd->GetModuleBase() && 
            _ul64Address <= pmd->GetModuleEnd() )
        {
            // found it
            return pmd->GetComponentName();
        }
    }
    return NULL;
}

// Caller will need to free the returned memory pointer
WCHAR* CKernelListener::GetModuleNameAndSymbol(ULONG64 _ul64Address,bool* _pbDxgkrnl)
{
    WCHAR* pRet=NULL;

    std::list<MODULEDATA*>::iterator it;
    for( it = m_pModuleList.begin(); it != m_pModuleList.end(); ++it)
    {
        MODULEDATA* pmd = (*it);

        // we're looking for non OS components.
        if( !pmd->IsOsComponent() )
        {
            if( _ul64Address != 0 && 
                _ul64Address >= pmd->GetModuleBase() && 
                _ul64Address <= pmd->GetModuleEnd() )
            {
                // found it
                WCHAR* pTemp = NULL;
                WCHAR* pSymbol = new WCHAR[MAX_PATH];
                if( pSymbol )
                {
                    // Validate the string
                    StringCchCopy(pSymbol,MAX_PATH,L"");

                    // Get the module name and then go for the symbol
                    pTemp = pmd->GetModuleName();
                    if( pTemp )
                    {
                        StringCchCat(pSymbol,MAX_PATH,pTemp);

                        // look specifically for dxgkrnl.sys
                        if( wcsstr(pSymbol,L"dxgkrnl.sys") )
                        {
                            if(_pbDxgkrnl)
                                *_pbDxgkrnl = true;
                        }

                        // We'll ether get back the symbol, or NULL and we'll calculate the offset.
                        WCHAR* pwszSymbol = GetDriverSymbol(pmd,_ul64Address);
                        if( pwszSymbol )
                        {
                            StringCchCat(pSymbol,MAX_PATH,L"!");
                            PATH_TRAP();
                            // copy it in and free this allocation.
                            StringCchCat(pSymbol,MAX_PATH,pwszSymbol);

                            delete pwszSymbol;
                        }
                        else
                        {
                            WCHAR wszBase[MAX_PATH]={0};

                            // output the offset into the module
                            ULONG64 _ul64Offset = _ul64Address - pmd->GetModuleBase();

                            StringCchPrintf(wszBase, MAX_PATH, L"+0x%I64x",_ul64Offset);
                            StringCchCat(pSymbol,MAX_PATH,wszBase);
                        }
                    }

                    pRet = pSymbol;
                }
                break;
            }
        }
    }

    return pRet;
}


// the caller needs to free the returned pointer.
WCHAR* CKernelListener::GetDriverSymbol(MODULEDATA* _pmd,ULONG64 _ul64Address)
{
    WCHAR* pRet = NULL;
    HANDLE hProcess = (HANDLE)_pmd->GetProcessId();

#ifdef WANT_DBG_VERBOSE
    WCHAR wszLog[MAX_PATH];
    StringCchPrintf(wszLog,132,L"***Loading symbols for Module: %s, addr=0x%X, handle=0x%X\n\r",
                    _pmd->GetModuleName(),(DWORD)_ul64Address,hProcess);
    OutputDebugString(wszLog);
#endif
    //
    // load the module into DbgHelp.lib
    //
    DWORD dwSize = 0;

    ULONGLONG ModuleBaseAddr = SymLoadModuleExW(hProcess,        // Previously registerd handle
                                                NULL,            // hFile, which we are not using
                              // Get the imagename from the sModuleData structure
                                                _pmd->GetModuleName(), // Image File Name that we registered
                                                NULL,            // Shortcut name for the module
                              // Also get the base and size from there too...
                                                _pmd->GetModuleBase(), 
                                                dwSize,
                                                NULL,
                                                0);

    //
    // If the call loaded the module, or if the module was already loaded
    //
    DWORD dwError = GetLastError();
    if (ModuleBaseAddr > 0 || ERROR_SUCCESS == dwError)
    {
        //
        // Search for the symbol in the module
        //
        char buffer[(sizeof(SYMBOL_INFOW) + MAX_SYM_NAME*sizeof(TCHAR))]={0};
        SYMBOL_INFOW* pSymbol = (SYMBOL_INFOW*)buffer;

        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFOW);
        pSymbol->MaxNameLen = MAX_SYM_NAME;

        ULONGLONG Displacement=0;
        BOOL bFound = SymFromAddrW(hProcess, _ul64Address, &Displacement, pSymbol);
        if(bFound)
        {
            pRet = new WCHAR[MAX_SYM_NAME*sizeof(TCHAR)*2];
            if( pRet )
            {
                //
                // Get the symbol
                //
                if (pSymbol->Name != NULL)
                {
                    // Copy the name into our allocated memory
                    StringCchCopy(pRet, MAX_SYM_NAME, pSymbol->Name);
                }
            }
        }
        else
        {
#ifdef WANT_DBG_VERBOSE
            DWORD dwError = GetLastError();
            StringCchPrintf(wszLog,132,L"Failed to fetch address - GetLastError=%d(%x)\n\r",dwError,dwError);
            OutputDebugString(wszLog);
#endif
        }


        //
        // Unload the module to save memory
        //
        if(SymUnloadModule64(hProcess, _pmd->GetModuleBase()))
        {
#ifdef WANT_DBG_VERBOSE
            StringCchPrintf(wszLog,132,L"   ***UnLoading symbols for Module: %s, addr=0x%X, handle=0x%X\n\r",
                            _pmd->GetModuleName(),(DWORD)_ul64Address,hProcess);
            OutputDebugString(wszLog);
#endif
        }
    }
    else
    {
        // Failure
        PATH_TRAP();
    }
    return pRet;
}




UINT CKernelListener::StackWalkCount()
{
    return m_StackList.size();
}

// Log each event to the output file as a message and clean up the list at the same time.
void CKernelListener::LogStacks(CLogWriter* _pLog,ULONG _ulThreadId)
{
    // Only log the stackitems that have a matching thread Id
    std::list<STACKITEM*>::iterator It = m_StackList.begin();
    while( It != m_StackList.end() )
    {
        STACKITEM* psItem = *It;

        if( psItem->GetThreadId() == _ulThreadId )
        {
            // matching Id, log this one.
            psItem->LogItem(_pLog,this);

            if( gbDebugStacks )
            {
                // if the user wants to debug the stacks, we'll want to stall here. To do this
                // we will either trap (checked builds) or prompt the user with a messagebox
                // before continuing.
#ifdef DBG
                PATH_TRAP();
#else
                MessageBox(NULL,
                           L"Stack has been dumped using OutputDebugString. Break in debugger and debug now.", 
                           L"Found Fauly Stack.",
                           MB_ICONWARNING | MB_OK);
#endif
            }

        }

        It++;
    }
}


// Log the modules and their addresses to the file
void CKernelListener::LogModules(CLogWriter* _pLog)
{
    WCHAR wszLog[132];

    SymSetOptions(SYMOPT_UNDNAME | 
                  SYMOPT_DEFERRED_LOADS | 
                  SYMOPT_DEBUG |
                  //SYMOPT_ALLOW_ABSOLUTE_SYMBOLS |
                  SYMOPT_LOAD_LINES);

    StringCchPrintf(wszLog,132,L"The module list addresses used to resolve stackwalk events.");
    _pLog->Message(wszLog);

    StringCchPrintf(wszLog,132,L"ProcessId : Base : End : Name");
    _pLog->Message(wszLog);

    std::list<MODULEDATA*>::iterator it;
    for( it = m_pModuleList.begin(); it != m_pModuleList.end(); ++it)
    {
        MODULEDATA* pmd = (*it);

        // Dump to the log file the base address, the end address and the name.
        // these values can then be used to help determine the failed stackwalk
        // addresses.
        if( pmd->GetModuleBase() > 0 )
        {
            StringCchPrintf(wszLog,132,L"LogModules - 0x%I64x : 0x%I64x : 0x%I64x : %s",
                            pmd->GetProcessId(),
                            pmd->GetModuleBase(),
                            pmd->GetModuleEnd(),
                            pmd->GetModuleName());
#ifdef WANT_DBG
            OutputDebugString(wszLog);
            OutputDebugString(L"\n\r");
#endif
            _pLog->Message(wszLog);
        }
        else
        {
            StringCchPrintf(wszLog,132,L"Not Tracked: LogModules - 0x%I64x : 0x%I64x : 0x%I64x : %s",
                            pmd->GetProcessId(),
                            pmd->GetModuleBase(),
                            pmd->GetModuleEnd(),
                            pmd->GetModuleName());
            OutputDebugString(wszLog);
            OutputDebugString(L"\n\r");
            _pLog->Message(wszLog);
        }

        // if the module is already loaded, don't load it a second time.
        if( pmd->IsSymLoaded() )
        {
            continue;
        }

        if( pmd->GetProcessId() == 0 )
        {
            // didn't get a load for this module, don't need the symbol.
            continue;
        }

        // in order for the module to remain in memory, we've got to load it
        pmd->m_hMod = LoadLibraryEx(pmd->GetModuleName(),
                                    NULL,
                                    LOAD_LIBRARY_SEARCH_SYSTEM32);



        // Only load this symbol if it's in our process and only do this once.
        // Here is where we want to load the symbols for this module so that when we
        // got to dumping the actual failure events (address in module) we'll be able to
        // get that symbol.

        HANDLE hTest = (HANDLE)pmd->GetProcessId();
        if(!SymInitialize(hTest,
                          NULL, // Use _NT_SYMBOL_PATH environment variable
                          FALSE)) // FALSE
        {
            // Failure
            DWORD dwError = GetLastError();
#ifdef WANT_DBG
            StringCchPrintf(wszLog,132,L"GetLastError=%d(%x)",dwError,dwError);
            OutputDebugString(wszLog);
            OutputDebugString(L"\n\r");
#endif
            pmd->SymLoaded(false);
            PATH_TRAP();
        }
        else
        {
            pmd->SymLoaded(true);
#ifdef WANT_DBG
            //success?
            StringCchPrintf(wszLog,132,L"\n\rSuccessfully loaded Module: %s\n\r\n\r",pmd->GetModuleName());
            OutputDebugString(wszLog);
#endif
        }
    }
}
std::map<ULONG,THREADMAPINFO*>* CKernelListener::GetThreadMap()
{
    return &m_pThreadMap;
}

void GetUserModeDriverList(CKernelListener* _pListener,
                           CLogWriter* pLog)
{
    AdapterUtil* putil = NULL;
    const AdapterDescriptor* pad = NULL;

    std::vector<std::wstring> DriverNames;
    LPTSTR lpwsz;

    putil = AdapterUtil::CreateAdapterUtil();
    if( NULL == putil )
    {
        // Failed to create our adapter type
        throw std::exception("Failed to create our adapter type!");
    }           

    vector<AdapterDescriptor*> vAdapters = putil->GetAllAdapters();
    if( vAdapters.empty() )
    {
        // Failed to get Primary Adatper
        throw std::exception("Failed to get Adatpers!");
    }

    for (AdapterDescriptor* pAdapter : vAdapters)
    {
        bool bSoftwareDevice = false;

        // skip BRD
        pAdapter->IsSoftwareDevice(bSoftwareDevice);
        if (bSoftwareDevice)
        {
            continue;
        }

        if(gbDriverLayering)
            DriverNames = pAdapter->GetDriverBinaryList();
        else
            DriverNames = pAdapter->GetUserModeDriverBinaryList();

        for(std::vector<std::wstring>::iterator y = DriverNames.begin(); y != DriverNames.end(); y++)
        {
            lpwsz = (LPTSTR)(*y).c_str();

            if( NULL != lpwsz )
            {
#ifdef WANT_DBG
                OutputDebugString(L"Module: (Registry) ");
                OutputDebugString(lpwsz);
                OutputDebugString(L"\n\r");
#endif

                // Log the file that we will check.
                WCHAR wszName[MAX_PATH];
                StringCchPrintf(wszName,MAX_PATH,L"Checking(Reg): Module: %s",lpwsz);
                pLog->Message(wszName);

                // Add the name here. Use false to indicate that it's not a OS component.
                _pListener->AddModule(lpwsz,(WCHAR*)pAdapter->GetDevicePath(),false,pLog);
            }
        }
    }
}


bool DetermineWhatDriverToTrack(LPTSTR lpwsz)
{
    bool bKernel = false;
    bool bUser = false;
    LPTSTR lpwszScratch;
    // Now look at every string in order to extract module names. But we
    // only really care about kernel names.
    lpwszScratch = wcsstr(lpwsz,L".sys");
    if( NULL != lpwszScratch )
    {
        bKernel = true;
    }
    lpwszScratch = wcsstr(lpwsz,L".dll");
    if( NULL != lpwszScratch )
    {
        bUser = true;
    }

    // If the user is tracking all modules, if we have either a sys or dll, we'll
    // return true
    if( gbAllDrivers )
    {
        return (bKernel | bUser);
    }
    else
    {
        // else, we're either WDDMv1.1 or WDDMv1.2 so we'll want kernel mode drivers.
        // the caller will look up the user mode drivers from the registry.
        return bKernel;
    }
}

//
// This routine parses the driver names returned form the Pnp Help utility and 
// places the .sys driver names in our module list.
// It will also add OS Graphics Components to the list
//
void
AddModulesToList(CKernelListener* _pListener,
                 CLogWriter* pLog)
{
    if(!gbDriverLayering)
    {
        pLog->Message(L"Loading kernel drivers only");
        CPnPControlDevice controlAdapter;

        for(int DeviceIndex=0;DeviceIndex<16;DeviceIndex++)
        {
            CPnPControlDriverNames DriverNames;
            if( controlAdapter.GetDriverNames(&DriverNames,DeviceIndex) )
            {
                WCHAR wszComponentName[MAX_PATH];
                ZeroMemory(wszComponentName, sizeof(wszComponentName));
                StringCchPrintf(wszComponentName, MAX_PATH, L"DisplayDevice%d", DeviceIndex);
                int i=0;
                LPTSTR lpwsz;
                lpwsz=DriverNames.GetNameByIndex(i++);
                while(lpwsz != NULL )
                {
                    if( DetermineWhatDriverToTrack(lpwsz)  )
                    {
#ifdef WANT_DBG_VERBOSE
                        OutputDebugString(L"Module: ");
                        OutputDebugString(lpwsz);
                        OutputDebugString(L"\n\r");
#endif
                        LPTSTR lpwszThis,lpwszLast;

                        // Save the last known name pointer and attempt to advance it.
                        lpwszLast = lpwsz;
                        lpwszThis = wcsstr(lpwsz,L"\\");
                        while(NULL != lpwszThis)
                        {
                            // We found one, 
                            lpwszLast = lpwszThis;
                            lpwszThis = wcsstr(lpwszLast,L"\\");
                            if( NULL != lpwszThis )
                            {
                                // Advance past the whack.
                                lpwszThis++;
                            }
                        }
                        if( NULL != lpwszLast )
                        {
#ifdef WANT_DBG
                            OutputDebugString(L"Module: ");
                            OutputDebugString(lpwszLast);
                            OutputDebugString(L"\n\r");
#endif

                            // Log the file that we will check.
                            WCHAR wszName[MAX_PATH];
                            StringCchPrintf(wszName,MAX_PATH,L"Checking(PnP): Module: %s",lpwszLast);
                            pLog->Message(wszName);

                            // Add the name here. Use false to indicate that it's not a OS component.
                            _pListener->AddModule(lpwszLast,wszComponentName,false,pLog);
                        }
                    }

                    lpwsz=DriverNames.GetNameByIndex(i++);
                }
            }
            else
            {
#ifdef WANT_DBG
                OutputDebugString(L"Failed to find kernel video driver on platform!\n\r");
#endif
            }
        }
    }

    // Now the user mode drivers
    if( !gbWDDM1_1 || gbAllDrivers || gbDriverLayering)
    {
        pLog->Message(L"Loading all drivers");
        GetUserModeDriverList(_pListener,pLog);
    }
}

//
// Main routine that sets up the listener.
//
HRESULT PerfinfoGuidEnabled(HANDLE _Event,
                            CLogWriter* pLog)
{
    HRESULT hr = ERROR_SUCCESS;
    
    try
    {
        WCHAR wszDbg[256];
        WCHAR wszLog[256];

        pLog->Message(L"PerfinfoGuidEnabled Start");

        CController etwController;
        CKernelListener NTConsumer(pLog);

        AddModulesToList(&NTConsumer,pLog);

        // Because we're going to use extended flags, we'll need to set the extended flags for
        // this session here.
        ULONG ulSessionFlags = USE_EXTENDED_SESSION_FLAGS;
        NTConsumer.SetSessionEnableFlags(&ulSessionFlags);
        //
        // CSwitch PagefaultHard SyscallEnter SyscallExit Profile
        //
        hr = NTConsumer.AddStackWalkItem(sw_CSwitch);
        IF_TRUE_ACTION_JUMP(ERROR_SUCCESS!=hr,
                            StringCchPrintf(wszDbg,256,L"AddStackWalkItem sw_CSwitch failed hr=0x%x(%d)",hr,hr);
                                pLog->Message(wszDbg),
                            PerfinfoGuidEnabled_Ret);
        hr = NTConsumer.AddStackWalkItem(sw_PagefaultHard);
        IF_TRUE_ACTION_JUMP(ERROR_SUCCESS!=hr,
                            StringCchPrintf(wszDbg,256,L"AddStackWalkItem sw_PagefaultHard failed hr=0x%x(%d)",hr,hr);
                                pLog->Message(wszDbg),
                            PerfinfoGuidEnabled_Ret);
        hr = NTConsumer.AddStackWalkItem(sw_Profile);
        IF_TRUE_ACTION_JUMP(ERROR_SUCCESS!=hr,
                            StringCchPrintf(wszDbg,256,L"AddStackWalkItem sw_Profile failed hr=0x%x(%d)",hr,hr);
                                pLog->Message(wszDbg),
                            PerfinfoGuidEnabled_Ret);


        // We'll do this for each flag we want to set. Note that these are 'grouped'
        // so as to only get one high bit set. That high bit is the index into the
        // PERFINFO_GROUPMASK structure. 
        // First set.
        hr = NTConsumer.SetEnableFlag(PERF_LOADER | 
                                       PERF_PROC_THREAD | 
                                       PERF_DISK_IO | 
                                       PERF_HARD_FAULTS |
                                       PERF_ALL_FAULTS);
        IF_TRUE_ACTION_JUMP(ERROR_SUCCESS!=hr,
                            StringCchPrintf(wszDbg,256,L"SetEnableFlag Loader failed hr=0x%x(%d)",hr,hr);
                                pLog->Message(wszDbg),
                            PerfinfoGuidEnabled_Ret);

        // second set
        hr = NTConsumer.SetEnableFlag(PERF_DPC |
                                       PERF_INTERRUPT | 
                                       PERF_PROFILE);
        IF_TRUE_ACTION_JUMP(ERROR_SUCCESS!=hr,
                            StringCchPrintf(wszDbg,256,L"SetEnableFlag DPC failed hr=0x%x(%d)",hr,hr);
                                pLog->Message(wszDbg),
                            PerfinfoGuidEnabled_Ret);

#ifdef WANT_DBG
        OutputDebugString(L"Starting to listen...\n\r");
#endif
        etwController.AddConsumer(&NTConsumer);

        DWORD dwStart = GetTickCount();
        StringCchPrintf(wszDbg,256,L"PerfinfoGuidEnabled: StartProcessing() GetTickCount Start=%d",dwStart);
        pLog->Message(wszDbg);

        // Start processing, wait and then stop after we've collected enough info.
        if( !etwController.StartProcessing() )
        {
            pLog->Message(L"PerfinfoGuidEnabled: StartProcessing() failed");
        }
        if(!NTConsumer.WaitForModuleData(5000))
        {
            pLog->Fail(L"Didn't get module information loaded in time");
        }

        WaitForSingleObject(_Event, INFINITE);
        etwController.StopProcessing();

        DWORD dwEnd = GetTickCount();
        StringCchPrintf(wszDbg,256,L"PerfinfoGuidEnabled: StopProcessing() GetTickCount =%d, length=%d",
                        dwEnd,((dwEnd-dwStart)/1000));
        pLog->Message(wszDbg);

        // Now log how many events we trapped

        CALLCOUNT* pccData = NTConsumer.CallCount();
#ifdef WANT_DBG_VERBOSE
        StringCchPrintf(wszDbg,256,L"CallCounts: StackWalk=%d, ToEarly=%d.\n\r",
                        pccData->ulStackWalk,pccData->ulToEarly);
        OutputDebugString(wszDbg);
        // Now for success Failure stats

        StringCchPrintf(wszDbg,256,L"            SuccessfullWalks=%d, FailedWalks=%d, DriverLayeringCount=%d.\n\r",
                        pccData->ulWalkSucceeded,
                        pccData->ulWalkFailed,
                        pccData->ulDriverLayering);
        OutputDebugString(wszDbg);

        StringCchPrintf(wszDbg,256,L"            StackList Count=%d\n\r\n\r",
                        Listener.StackWalkCount());
        OutputDebugString(wszDbg);
#endif
        NTConsumer.LogModules(pLog);

        if( pccData->ulWalkSucceeded > MINIMUM_SAMPLING_SET )
        {
            // we had enough samples
            if( 0 == pccData->ulWalkFailed )
            {
                // we had no failures, log success

                StringCchPrintf(wszLog,256,L"StackWalks=%d, Sampling set=%d, no failures",
                                pccData->ulStackWalk,
                                pccData->ulWalkSucceeded);
                pLog->Message(wszLog);
            }
            else
            {
                // we had a failure... where?
                StringCchPrintf(wszLog,256,L"Total Failure count=%d",pccData->ulWalkFailed);
                pLog->Message(wszLog);


                // Walk the list of threads looking at the success and failure counts. we want to 
                // report failures on all user mode threads AND report warning on kernel threads that
                // have successful and failure conditions AND skips on kernel mode threads that have all 
                // failures and no successes.

                std::map<ULONG, THREADMAPINFO*>* pMap = NTConsumer.GetThreadMap();
                std::map<ULONG, THREADMAPINFO*>::iterator It = pMap->begin();
                while(It != pMap->end())
                {
                    // Let's output our structure
                    THREADMAPINFO* ptd = (*It).second;

                    // All we care about here is passes and failures.
                    if( ptd->GetFailedWalks() )
                    {
                        // we've had failures on this thread, log them as a failure!
                        StringCchPrintf(wszLog,256,
                                        L"Failed to successfully walk %d StackWalk events on thread %d, Stacks follow:",
                                        ptd->GetFailedWalks(),
                                        ptd->GetThreadId());
                        pLog->Fail(wszLog);

                        // Dump the bad stacks associated with this thread.
                        NTConsumer.LogStacks(pLog,ptd->GetThreadId());
                    }
                    // advance iterator
                    It++;
                }
            }

            // This second walk through the threads simply logs the thread info
            pLog->Message(L"Logging all threads and the state:");

            std::map<ULONG, THREADMAPINFO*>* pMap = NTConsumer.GetThreadMap();
            std::map<ULONG, THREADMAPINFO*>::iterator It = pMap->begin();
            while(It != pMap->end())
            {
                // Let's output our structure
                THREADMAPINFO* ptd = (*It).second;

                // only output threads with numbers
                if (ptd->GetSuccessfulWalks() || ptd->GetFailedWalks())
                {
                    // Now log out the state of the threads on the system.
                    StringCchPrintf(wszLog,256,L"ThreadId=%5d, %6s, Successful=%4d, Failed=%2d",
                                    ptd->GetThreadId(),
                                    (ptd->IsKernelModeThread()?L"kernel":L"user"),
                                    ptd->GetSuccessfulWalks(),
                                    ptd->GetFailedWalks());
                    pLog->Message(wszLog);
#ifdef WANT_DBG
                    OutputDebugString(wszLog);
                    OutputDebugString(L"\n\r");
#endif
                }
                // advance iterator
                It++;
            }

            pLog->Message(L"Logging just threads with failures:");
            pMap = NTConsumer.GetThreadMap();
            It = pMap->begin();
            while(It != pMap->end())
            {
                // Let's output our structure
                THREADMAPINFO* ptd = (*It).second;

                // only output threads with numbers
                if (ptd->GetFailedWalks())
                {
                    // Now log out the state of the threads on the system.
                    StringCchPrintf(wszLog,256,L"ThreadId=%5d, %6s, Successful=%4d, Failed=%2d",
                                    ptd->GetThreadId(),
                                    (ptd->IsKernelModeThread()?L"kernel":L"user"),
                                    ptd->GetSuccessfulWalks(),
                                    ptd->GetFailedWalks());
                    pLog->Message(wszLog);
#ifdef WANT_DBG
                    OutputDebugString(wszLog);
                    OutputDebugString(L"\n\r");
#endif
                }
                // advance iterator
                It++;
            }
        }
        else
        {
            StringCchPrintf(wszLog,256,L"Not a large enough sampling set, Samples=%d",pccData->ulWalkSucceeded);
            pLog->Fail(wszLog);
        }
    }
    catch(...)
    {
        pLog->Message(L"PerfinfoGuidEnabled: Caught an exception!");
    }
PerfinfoGuidEnabled_Ret:
    pLog->Message(L"PerfinfoGuidEnabled Done");

    return hr;
}


