// Copyright (C) Microsoft. All rights reserved.
#include "common.h"

// ----------------------------------------------------------------------
// CMemoryPropertyStore implementation
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Function:
//      CMemoryPropertyStore::CMemoryPropertyStore
//
// Description:
//      Constructor
//
// ----------------------------------------------------------------------
CMemoryPropertyStore::CMemoryPropertyStore()
    :  m_refCount(1),
       m_hMutexMemory(NULL)       
{
}

// ----------------------------------------------------------------------
// Function:
//      CMemoryPropertyStore::~CMemoryPropertyStore
//
// Description:
//      Destructor
//
// ----------------------------------------------------------------------
CMemoryPropertyStore::~CMemoryPropertyStore()
{
    MAP_STR_PROP::iterator iter = NULL;
    MAP_STR_PROP::iterator iterEnd = NULL;

    iterEnd = m_Memory.end();
    for(iter = m_Memory.begin(); iter != iterEnd; ++iter)
    {
        PropVariantClear(iter->second);
        delete iter->second;
    }
    ASSERT(m_refCount == 1);
}

// ----------------------------------------------------------------------
// Function:
//      CMemoryPropertyStore::Initialize()
//
// Description:
//      If the store was opened for write access, we may need to create the 
//      key.  If opened for read access, then we just need to see if we can 
//      open the key.
//
// Return:
//      S_OK if successful
//
// ----------------------------------------------------------------------
HRESULT CMemoryPropertyStore::Initialize()
{
    HRESULT hr = S_OK;

    m_hMutexMemory = CreateMutex(NULL, FALSE, NULL);
    if(NULL == m_hMutexMemory)
    {
        hr = E_POINTER;
    }

    return hr;
}


// ----------------------------------------------------------------------
// Function:
//      CMemoryPropertyStore::QueryInterface
//
// Description:
//      Implements IUnknown::QueryInterface
//
// Return:
//      S_OK if successful
//
// ----------------------------------------------------------------------
HRESULT CMemoryPropertyStore::QueryInterface
(
                const IID&  iid, 
    __deref_out void**      ppUnk
)
{
    if (!ppUnk)
        return E_POINTER;

    HRESULT hr = E_NOINTERFACE;
    *ppUnk = NULL;

    if (iid == __uuidof(IUnknown))
    {
        *ppUnk = (IUnknown*)this;
        hr = S_OK;
        AddRef();
    }
    else if (iid == __uuidof(IPropertyStore))
    {
        *ppUnk = (IPropertyStore*)this;
        hr = S_OK;
        AddRef();
    }
        
    return hr;
}

// ----------------------------------------------------------------------
// Function:
//      CMemoryPropertyStore::AddRef(void)
//
// Description:
//      Implements IUnknown::AddRef()
//
// Return:
//      New refcount
//
// ----------------------------------------------------------------------
ULONG CMemoryPropertyStore::AddRef(void)
{
    return InterlockedIncrement(&m_refCount);
}

// ----------------------------------------------------------------------
// Function:
//      CMemoryPropertyStore::Release(void)
//
// Description:
//      Implements IUnknown::Release()      
//
// Return:
//      New refcount
//
// ----------------------------------------------------------------------
ULONG CMemoryPropertyStore::Release(void)
{
    ASSERT(m_refCount > 0);
    LONG lRef = InterlockedDecrement(&m_refCount);
    ASSERT(lRef >= 0);

    if (lRef == 0)
    {
        delete this;
    }
    return lRef;
}

// ----------------------------------------------------------------------
// Function:
//      CMemoryPropertyStore::GetCount
//
// Description:
//      
//
// Return:
//      S_OK if successful
//
// ----------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CMemoryPropertyStore::GetCount
(
    __out DWORD* pcProps
)
{
    WaitForSingleObject(m_hMutexMemory, INFINITE);
    *pcProps = m_Memory.size();
    ReleaseMutex(m_hMutexMemory);

    return S_OK;
}

// ----------------------------------------------------------------------
// Function:
//      CMemoryPropertyStore::GetAt
//
// Description:
//      
//
// Return:
//      S_OK if successful
//
// ----------------------------------------------------------------------
HRESULT CMemoryPropertyStore::GetAt
(
            DWORD           iProp, 
    __out   PROPERTYKEY*    pkey
)
{
    // NOT IMPLEMENTED since not used by KRMProxy for now
    // Implement in the future when required
    return S_OK;
}


// ----------------------------------------------------------------------
// Function:
//      CMemoryPropertyStore::GetValue
//
// Description:
//      Implementation of IPropertyStore::GetValue.  This is basically a
//      wrapper around RegQueryValueEx
//
// Return:
//      S_OK if successful
//
// ----------------------------------------------------------------------
HRESULT CMemoryPropertyStore::GetValue
(
            REFPROPERTYKEY  key, 
    __out   PROPVARIANT*    pv
)
{
    HRESULT hr = S_OK;
    wstring wsKey;
    MAP_STR_PROP::iterator iter;

    // validate PROPVARIANT
    if(pv == NULL)  { return E_POINTER; }

    PropVariantInit(pv);
    KeyConv(wsKey, key, "Get");
    
    WaitForSingleObject(m_hMutexMemory, INFINITE);
    iter = m_Memory.find(wsKey);
    if(iter != m_Memory.end())
    {
        PropVariantCopy(pv, m_Memory[wsKey]);
    }
    ReleaseMutex(m_hMutexMemory);

    return hr;
}

// ----------------------------------------------------------------------
// Function:
//      CMemoryPropertyStore::SetValue
//
// Description:
//      Implementation of IPropertyStore::SetValue.  This is basically a
//      wrapper around RegSetValueEx
//
// Return:
//      S_OK if successful
//
// ----------------------------------------------------------------------
HRESULT CMemoryPropertyStore::SetValue
(
    REFPROPERTYKEY      key, 
    REFPROPVARIANT  pv
)
{
    HRESULT hr = S_OK;
    wstring wsKey;
    PROPVARIANT * v = NULL;

    // validate PROPVARIANT
    if(&pv == NULL)  { return E_POINTER; }

    // Create a new place to store the PROPVARIANT
    KeyConv(wsKey, key, "Set");
    v = new PROPVARIANT;
    PropVariantCopy(v, &pv);

    WaitForSingleObject(m_hMutexMemory, INFINITE);
    m_Memory[wsKey] = v;
    ReleaseMutex(m_hMutexMemory);

    return hr;
}

// ----------------------------------------------------------------------
// Function:
//      CMemoryPropertyStore::Commit
//
// Description:
//      Implementation of IPropertyStore::Commit.  This is basically a
//      wrapper around RegFlushKey
//
// Return:
//      S_OK if successful
//
// ----------------------------------------------------------------------
HRESULT CMemoryPropertyStore::Commit(void)
{
    return S_OK;
}


// ----------------------------------------------------------------------
// Function:
//      CMemoryPropertyStore::KeyConv
//
// Description:
//      This converts the key from a PROPERTYKEY to a wstring
//
// Return:
//
// ----------------------------------------------------------------------
