#ifndef __DLIST_H
#define __DLIST_H

#include "windows.h"
typedef struct _MONITOR_LIST_ENTRY
{
	_MONITOR_LIST_ENTRY* Blink;
	_MONITOR_LIST_ENTRY* Flink;
	
	int nType;
	void* pData;
}MONITOR_LIST_ENTRY, *PMONITOR_LIST_ENTRY;

void InitializeListHeadM(PMONITOR_LIST_ENTRY ListHead);
HRESULT GetHeadListItemM(PMONITOR_LIST_ENTRY ListHead, DWORD dwIndex, PMONITOR_LIST_ENTRY* ppEntry);
PMONITOR_LIST_ENTRY RemoveHeadListM(PMONITOR_LIST_ENTRY ListHead);
bool IsListEmptyM(PMONITOR_LIST_ENTRY ListHead);
void RemoveEntryListM(PMONITOR_LIST_ENTRY Entry);
void InsertTailListM(PMONITOR_LIST_ENTRY ListHead, PMONITOR_LIST_ENTRY Entry);
DWORD GetItemCount(PMONITOR_LIST_ENTRY ListHead);

#endif