//
// Copyright(C) Microsoft.All rights reserved.
//
// Utilities.h
//

//
// Description:
//
//  Common functions and macros
//
// Contents:
//  SAFE_RELEASE
//  SAFE_DELETE
//  SAFE_COTASKMEMFREE
//  SAFE_DELETE_ARRAY
//  SAFE_CLOSE_HANDLE
//  SAFE_SYSFREESTRING
//
#ifndef __UTILITIES_H__
#define __UTILITIES_H__

template <class T>
inline void SAFE_RELEASE(T* &pUnknown)
{
    if(NULL != pUnknown)
    {
        pUnknown->Release();
        pUnknown = NULL;
    }
}

template <class T>
inline void SAFE_DELETE(T* &pUnknown)
{
    if(NULL != pUnknown)
    {
        delete pUnknown;
        pUnknown = NULL;
    }
}

template <class T>
inline void SAFE_DELETE_ARRAY(T* &pT)
{
    if(NULL != pT)
    {
        delete[] pT;
        pT = NULL;
    }
}

template <class T>
inline void SAFE_COTASKMEMFREE(T* &pT)
{
    if(NULL != pT)
    {
        ::CoTaskMemFree(pT);
        pT = NULL;
    }
}


inline void SAFE_CLOSE_HANDLE(HANDLE& h)
{
    if((NULL != h) &&
       (INVALID_HANDLE_VALUE != h))
    {
        CloseHandle(h);
        h = INVALID_HANDLE_VALUE;
    }       
}

inline void SAFE_SYSFREESTRING(BSTR& bstr)
{
    SysFreeString(bstr);
    bstr = NULL;
}

#endif
