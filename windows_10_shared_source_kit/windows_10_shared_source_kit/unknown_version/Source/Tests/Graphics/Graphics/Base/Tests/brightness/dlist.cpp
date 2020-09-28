#include "brightnesspch.h"

void InitializeListHeadM(PMONITOR_LIST_ENTRY ListHead)
{
    (ListHead)->Flink = (ListHead)->Blink = ListHead;
    (ListHead)->pData = NULL;
    (ListHead)->nType = 0xff;
}

HRESULT GetHeadListItemM(PMONITOR_LIST_ENTRY ListHead, DWORD dwIndex, PMONITOR_LIST_ENTRY* ppEntry)
{
    HRESULT S_OK;
    if(IsListEmptyM(ListHead))
    {
        return S_FALSE;
    }
    else
    {
        DWORD dwCount = 0;
        DWORD dwItems = GetItemCount(ListHead);
        if(dwItems < (dwIndex+1))
            return E_INVALIDARG;

        PMONITOR_LIST_ENTRY pTemp = ListHead->Flink;
        while((pTemp != ListHead))
        {
            if(dwCount == dwIndex)
            {
                *ppEntry = pTemp;
                return S_OK;
            }
            dwCount++;
            
            pTemp = pTemp->Flink;
        }
        return E_INVALIDARG;
    }
}

PMONITOR_LIST_ENTRY RemoveHeadListM(PMONITOR_LIST_ENTRY ListHead)
{
    PMONITOR_LIST_ENTRY _EX_Flink;
    _EX_Flink = (ListHead)->Flink;
    RemoveEntryListM((ListHead)->Flink);
    return _EX_Flink;
}
    
bool IsListEmptyM(PMONITOR_LIST_ENTRY ListHead)
{
    return ((ListHead)->Flink == (ListHead));
}


void RemoveEntryListM(PMONITOR_LIST_ENTRY Entry)
{
    PMONITOR_LIST_ENTRY _EX_Blink;
    PMONITOR_LIST_ENTRY _EX_Flink;
    _EX_Flink = (Entry)->Flink;
    _EX_Blink = (Entry)->Blink;
    _EX_Blink->Flink = _EX_Flink;
    _EX_Flink->Blink = _EX_Blink;
}

void InsertTailListM(PMONITOR_LIST_ENTRY ListHead, PMONITOR_LIST_ENTRY Entry)
{
    PMONITOR_LIST_ENTRY _EX_Blink;
    PMONITOR_LIST_ENTRY _EX_ListHead;
    _EX_ListHead = (ListHead);
    _EX_Blink = _EX_ListHead->Blink;
    (Entry)->Flink = _EX_ListHead;
    (Entry)->Blink = _EX_Blink;
    _EX_Blink->Flink = (Entry);
    _EX_ListHead->Blink = (Entry);
}

DWORD GetItemCount(PMONITOR_LIST_ENTRY ListHead)
{
    if(IsListEmptyM(ListHead))
        return 0;
    else
    {
        DWORD dwCount = 1;
        PMONITOR_LIST_ENTRY pTemp = ListHead->Flink;
        while(pTemp->Flink != ListHead)
        {
            dwCount++;
            pTemp = pTemp->Flink;
        }
        return dwCount;
    }	
}