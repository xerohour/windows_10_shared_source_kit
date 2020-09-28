#ifndef __INTERFACE_TEMPLATE__
#define __INTERFACE_TEMPLATE__

#include <unknwn.h>



typedef BOOL (WINAPI * DEBUGFUNC) (void);

typedef struct _tagIList
{
    IUnknown          * pIUnknown;
    PVOID               pInterface;
    ULONG               uRefCount;
    LPCTSTR             szDesc;
    LPCTSTR             szFile;
    int                 uLine;
    struct _tagIList  * pNext;
} INTERFACELIST, *PINTERFACELIST;

extern PINTERFACELIST g_pInterfaceList;
extern DEBUGFUNC      g_pDebuggerAvailable;
extern bool           g_fValidateInterface;

void WINAPI CompareObjects(void);
void WINAPI CleanupObjects(void);

template <class x> ULONG ReleaseAndClear(x ** ppObj)
{
    ULONG uRefCount = 0;

    if (NULL != *ppObj)
    {
        uRefCount = (*ppObj)->Release();

        *ppObj = NULL;

        return uRefCount;
    }

    return uRefCount;
    

    if (NULL != *ppObj)
    {
        // Wrap this in a try-except. ValidateInterface() only
        // works if specifically requested, and it's possible
        // that certain objects may go away in the wrong order (like
        // flipping chains).

        __try
        {
            if (ValidInterface((*ppObj)))
                uRefCount = (*ppObj)->Release();
        }
        __except(1)
        {
            uRefCount = 0;
        }

        if (0 == uRefCount)
        {
            PINTERFACELIST pWalk = g_pInterfaceList;
            PINTERFACELIST pHold;

            // Make sure that the interface was removed from the list.

            while (NULL != pWalk)
            {
                if (pWalk->pInterface == (PVOID) *ppObj ||
                    pWalk->pIUnknown == (IUnknown *) *ppObj)
                    break;

                pHold = pWalk;
                pWalk=pWalk->pNext;
            }

            if (NULL != pWalk)
            {
                if (pWalk == g_pInterfaceList)
                    g_pInterfaceList = g_pInterfaceList->pNext;
                else
                    pHold->pNext = pWalk->pNext;

                delete pWalk;
            }
        }
        else
            UpdateObject(*ppObj,NULL,NULL,0);
    }

    *ppObj = NULL;

    return uRefCount;
}

template <class x> bool UpdateObject(x * pObj, LPCTSTR szDesc, LPCTSTR szFile, UINT uLine)
{
    return true;

    PINTERFACELIST  pWalk = g_pInterfaceList;

    if (NULL == pObj)
        return true;

    if (!ValidInterface(pObj))
        return true;

    while (NULL != pWalk)
    {
        if (pObj == (x *) pWalk->pInterface)
            break;

        pWalk=pWalk->pNext;
    }

    if (NULL == pWalk)
    {
        pWalk = new INTERFACELIST;

        if (NULL == pWalk)
            return false;

        if (NULL != szDesc)
            pWalk->szDesc = szDesc;
        else
            pWalk->szDesc = "Unknown";

        if (NULL != szFile)
            pWalk->szFile = szFile;
        else
            pWalk->szFile = "Created in unknown location";

        pWalk->uLine = uLine;

        
        __try
        {
            pObj->QueryInterface(IID_IUnknown,(PVOID *) &pWalk->pIUnknown);
            pWalk->pIUnknown->Release();
        }
        __except(1)
        {
            delete pWalk;
            return true;
        }

        pWalk->pInterface = (PVOID) pObj;
        pWalk->pNext = g_pInterfaceList;

        g_pInterfaceList = pWalk;
    }

    pObj->AddRef();
    pWalk->uRefCount = pObj->Release();

    return true;
}

template <class x> bool ValidInterface(x * pObj)
{

// if UNLEASH_INTERFACE_VALIDATION ifdef is removed,
// code needs to be made ia64 compatible
// code currently has multiple ia64 compatibility issues

#ifdef UNLEASH_INTERFACE_VALIDATION
    if (NULL == pObj)
        return false;

    if (!g_fValidateInterface)
        return true;

    if (NULL == g_pDebuggerAvailable)
        return true;
    
    if (!g_pDebuggerAvailable())
        return true;

    // Evaluate the pointer itself.

    if (IsBadReadPtr(pObj,sizeof(x)))
        return false;

    // Evaluate the VTable pointer.

    __try
    {
        PDWORD  pPtr;
        DWORD   dwVal;

        pPtr = (PDWORD)(*(PDWORD)pObj);

        if (NULL == pPtr)
            return false;

        // Under NT, the VTable is cleared to 0xfeeefeee. This check is simply made
        // because IsBadReadPtr() on NT is generating a number of first-chance
        // exception debug spew, which we want to avoid if at all possible. If not
        // possible, no big deal... it's benign and won't kill us.

        if ((PDWORD) 0xfeeefeee == pPtr)
            return false;

        dwVal = *pPtr;
        dwVal = *pPtr+1;
        dwVal = *pPtr+2;
    }
    __except(1)
    {
        return false;
    }

    if (IsBadReadPtr((PDWORD)(*(PDWORD)pObj),sizeof(DWORD)))
        return false;

    // Ok. The memory is ok. Try to access it like an object.

    __try
    {
        pObj->AddRef();
        pObj->Release();
    }
    __except(1)
    {
        return false;
    }
#endif

    return true;
}

// UPDATEOBJECT does nothing

#define UPDATEOBJECT(x)
// UpdateObject(x,#x,__FILE__,__LINE__)
#define TRACKOBJECT(x) UPDATEOBJECT(x);
#define TRACKUNKNOWN(x) UpdateObject(x,#x,NULL,0)

 
#endif

