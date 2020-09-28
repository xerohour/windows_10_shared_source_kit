//**@@@*@@@****************************************************
//
// Copyright (C) Microsoft. All rights reserved.
//
//**@@@*@@@****************************************************

//
// FileName:    PropertyStore.h
//
// Abstract:    Creates a minimal property store in memory only
//              ( This implementation only implements the VT_BLOB
//                data type.)
//
// Author:      rsaad (taken from multimedia\audiocore\DeviceApi\core\)
//
// Created:     2004/8/24
// --------------------------------------------------------------------------------

#pragma once

using namespace std;

typedef map<wstring, PROPVARIANT*> MAP_STR_PROP;

// --------------------------------------------------------------------------------
//
// Class:
//      CMemoryPropertyStore 
//
// Description:
//      Implements IPropertyStore for Endpoints
//
// --------------------------------------------------------------------------------
class CMemoryPropertyStore : public IPropertyStore
{
protected:
    MAP_STR_PROP        m_Memory;
    HANDLE              m_hMutexMemory;

    LONG                m_refCount;

public:
    CMemoryPropertyStore();
    virtual ~CMemoryPropertyStore();
    HRESULT Initialize();

    // IPropertyStore
    STDMETHOD(GetCount)(OUT DWORD* pcProps);
    STDMETHOD(GetAt)(IN DWORD iProp, OUT PROPERTYKEY* pkey);
    STDMETHOD(GetValue)(IN REFPROPERTYKEY key, OUT PROPVARIANT* pv);
    STDMETHOD(SetValue)(IN REFPROPERTYKEY key, IN REFPROPVARIANT pv);
    STDMETHOD(Commit)();

    // IUnknown
    HRESULT STDMETHODCALLTYPE   QueryInterface(const IID& iid, void** ppUnk);
    ULONG   STDMETHODCALLTYPE   AddRef(void);
    ULONG   STDMETHODCALLTYPE   Release(void);

private:
    void KeyConv(wstring &ws, REFPROPERTYKEY  key, const char *message);
};

inline void CMemoryPropertyStore::KeyConv
    (wstring &ws, REFPROPERTYKEY  key, const char *message)
{
    WCHAR wszRep[200]={0};
    swprintf(wszRep,
            L"{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x},%d",
            key.fmtid.Data1, key.fmtid.Data2, key.fmtid.Data3,
            key.fmtid.Data4[0], key.fmtid.Data4[1],
            key.fmtid.Data4[2], key.fmtid.Data4[3],
            key.fmtid.Data4[4], key.fmtid.Data4[5],
            key.fmtid.Data4[6], key.fmtid.Data4[7], key.pid);
    ws = wszRep;

    //SLOG(eError, "CMemroyPropertyStore::%sValue  -  Key: %S", 
    //                    message, wszRep);
}
