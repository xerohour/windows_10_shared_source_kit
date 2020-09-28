//
// CStateCascade.cpp - Implementation of a state-cascading mechanism
//

#include "CStateCascade.h"
#include <stdio.h>
//-------------------------
// CStateCascade functions
//-------------------------

CStateCascade::CStateCascade()
{
    m_pEntries = NULL;
}

CStateCascade::~CStateCascade()
{
    STATEENTRY *pEntry;

    // Clean up all the entries

    pEntry = m_pEntries;
    while(pEntry != NULL)
    {
        STATEENTRY *pNextEntry = pEntry->pNext;
        delete pEntry;
        pEntry = pNextEntry;
    }
    m_pEntries = NULL;
}

void
CStateCascade::Cascade(int nCount)
{
    if(nCount > 0)
    {
        if(NULL != m_pEntries)
        {
            m_pEntries->Cascade();
            Cascade(nCount - 1);
        }
    }
}

int
CStateCascade::GetEntryStateCount(const char *szState)
{
    STATEENTRY *pEntry;

    pEntry = GetEntry(szState);
    if(pEntry)
        return pEntry->nStates;

    return -1;
}
int
CStateCascade::StateCount()
{
    STATEENTRY *pEntry = m_pEntries;
    int nCount = 1;

    if(m_pEntries == NULL)
        return 0;

    while(NULL != pEntry)
    {
        nCount *= pEntry->nStates;
        pEntry = pEntry->pNext;
    }

    return nCount;
}

bool
CStateCascade::AddEntry(const char *pszKey, const char *pszName, int nStates, bool bEnabled)
{
    if(nStates < 2)
        return false;

    STATEENTRY  *pNewEntry = new STATEENTRY;

    if(NULL != pNewEntry)
    {
        pNewEntry->pszKey = new char[strlen(pszKey) + 1];

        if(pNewEntry->pszKey)
            strcpy(pNewEntry->pszKey, pszKey);
        pNewEntry->pszName = new char[strlen(pszName) + 1];

        if(pNewEntry->pszName)
            strcpy(pNewEntry->pszName, pszName);
        pNewEntry->nStates = nStates;

        pNewEntry->bEnabled = bEnabled;

        if(m_pEntries)
            m_pEntries->AddToList(pNewEntry);
        else
            m_pEntries = pNewEntry;

        return true;
    }
    return false;
}

void
CStateCascade::CascadeToState(int nState)
{
    DWORD nTotalRange = 1;
    STATEENTRY  *pEntry;

    pEntry = m_pEntries;

    while(NULL != pEntry)
    {
        if(pEntry->bEnabled)
            pEntry->nCurrentState = ((UINT) (nState) >= nTotalRange) ? ((nState / nTotalRange) % pEntry->nStates) : 0;

        nTotalRange *= pEntry->nStates;
        pEntry = pEntry->pNext;
    }
}

bool
CStateCascade::SetEntryState(const char *pszKey, int nState)
{
    STATEENTRY  *pEntry = GetEntry(pszKey); 

    if(pEntry)
        if(pEntry->nStates > nState)
        {
            pEntry->nCurrentState = nState;
            return true;
        }
    return false;
}

bool
CStateCascade::SetEntryEnable(const char *pszKey, bool bEnable)
{
    STATEENTRY *pEntry = GetEntry(pszKey);

    if(pEntry)
       pEntry->bEnabled = bEnable;
    else
        return false;

    return true;
}

int
CStateCascade::GetEntryState(const char *pszKey)
{

    STATEENTRY *pEntry = GetEntry(pszKey);

    if(pEntry)
        return pEntry->nCurrentState;

    return -1;
}

STATEENTRY *
CStateCascade::GetEntry(const char *pszKey)
{
    STATEENTRY *pEntry = m_pEntries;

    while(pEntry)
    {
        if(!strcmp(pEntry->pszKey, pszKey))
            return pEntry;
        pEntry = pEntry->pNext;
    }
    return NULL;
}

//----------------------
// StateEntry functions
//----------------------

StateEntry::StateEntry()
{
    pszKey = NULL;
    pszName = NULL;
    pNext = NULL;
    nCurrentState = 0;
    nStates = 0;
    bEnabled = true;
}
StateEntry::~StateEntry()
{
    if(pszKey)
        delete [] pszKey;
    if(pszName)
        delete [] pszName;
    pNext = NULL;
}

void
StateEntry::Cascade()
{
    if(bEnabled)
    {
        nCurrentState += 1;
        if(nCurrentState == nStates)
        {
            nCurrentState = 0;
            if(pNext)
                pNext->Cascade();
        }
    } else {
        if(pNext)
            pNext->Cascade();
    }
}

void
StateEntry::AddToList(struct StateEntry *pEntry)
{
    if(NULL != pNext)
    {
        pNext->AddToList(pEntry);
    } else {
        pNext = pEntry;
    }
}
