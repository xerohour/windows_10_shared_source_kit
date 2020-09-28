#include "ObjectPCH.h"

#include "Interface.h"

PINTERFACELIST g_pInterfaceList=NULL;

void WINAPI CompareObjects(void)
{
/*
    PINTERFACELIST pWalk = g_pInterfaceList;

    while (NULL != pWalk)
    {
*/
/*
        ULONG uRefCount=0;

        __try
        {
            pWalk->pIUnknown->AddRef();
            uRefCount = pWalk->pIUnknown->Release();

            if (uRefCount != pWalk->uRefCount)
            {
                TCHAR   szBuffer[MAX_PATH];

                // Dump debug information on mis-matching interface.

                OutputDebugString("Warning: Interface mismatch of ");
                OutputDebugString(pWalk->szDesc);
                OutputDebugString("\n");
                OutputDebugString("         ");
                wsprintf(szBuffer,"%d was expected. %d was returned.\n",pWalk->uRefCount,uRefCount);
                OutputDebugString(szBuffer);
                OutputDebugString("         ");
                OutputDebugString(pWalk->szFile);
                wsprintf(szBuffer,": %d\n",pWalk->uLine);
                OutputDebugString(szBuffer);
            }
        }
        __except(1)
        {
            ReleaseAndClear(&pWalk->pIUnknown);
        }
*/
/*
        if (!ValidInterface(pWalk->pIUnknown))
            ReleaseAndClear(&pWalk->pIUnknown);

        pWalk=pWalk->pNext;
    }
*/
}

void WINAPI CleanupObjects(void)
{
    while (NULL != g_pInterfaceList)
    {
        PINTERFACELIST pHold = g_pInterfaceList->pNext;
        TCHAR          szBuffer[MAX_PATH];

        // Dump debug information on mis-matching interface.

        if (g_fValidateInterface)
        if (ValidInterface(g_pInterfaceList->pIUnknown))
        {
            ULONG   uRefCount;

            g_pInterfaceList->pIUnknown->AddRef();
            uRefCount = g_pInterfaceList->pIUnknown->Release();

            OutputDebugString("Warning: Interface ");
            OutputDebugString(g_pInterfaceList->szDesc);
            OutputDebugString(" was not released.\n");
            OutputDebugString("         ");
            wsprintf(szBuffer,"%d was reference count. \n",uRefCount);
            OutputDebugString(szBuffer);
            OutputDebugString("         ");
            OutputDebugString(g_pInterfaceList->szFile);
            wsprintf(szBuffer,": %d\n",g_pInterfaceList->uLine);
            OutputDebugString(szBuffer);
        }

        delete g_pInterfaceList;
        g_pInterfaceList = pHold;
    }
}


 

