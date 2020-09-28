/************************************************************
 *
 *   Copyright (c) Microsoft Corporation, 2001
 *
 *   Program:   Header for Utils
 *   Created:   06/12/01
 *    Author:   Bruce Nelson (brucen)
 *
 ************************************************************/
#ifndef _UTILS_H_
#define _UTILS_H_

#include<d3d9.h>

/*
 *  CAutoPtr - simple smart pointer to automatically release of resources
 */

template<class T>
class CAutoPtr
{
public:
    explicit CAutoPtr(T *t=0) :
        _t(t)
    {
    }
    CAutoPtr(const CAutoPtr<T> &pT) :
        _t(0)
    {
        *this = pT;
    }
    ~CAutoPtr()
    {
        if (_t) _t->Release();
    }
    CAutoPtr &operator=(const CAutoPtr<T> &t)
    {
        if (this != &t)
        {
            this->~CAutoPtr();
            _t = t._t;
            if (_t) _t->AddRef();
        }
        return *this;
    }
    T *operator->()
    {
        return _t;
    }
    T &operator*()
    {
        return *_t;
    }
    T **operator&()
    {
        return &_t;
    }
    operator T*()
    {
        return _t;
    }
    operator const T*() const
    {
        return _t;
    }
private:
    T *_t;
};


/*
 *  CHRESULT - encapsulates HRESULT error exception handling
 */

class CHRESULT 
{
public:
    CHRESULT(HRESULT hr = S_OK)
    {
        *this = hr;
    }
    CHRESULT &operator=(HRESULT hr)
    {
        if (FAILED(m_hr = hr))
        {
            throw *this;
        }
        return *this;
    }
    CHRESULT &operator=(bool b)
    {
        return *this = b ? S_OK : E_FAIL;
    }
    CHRESULT &operator=(BOOL b)
    {
        return *this = b ? S_OK : E_FAIL;
    }
    operator HRESULT() const
    {
        return m_hr;
    }
	bool setTestResult(CD3DTest *test) const
	{
		if ( test && FAILED(m_hr) )
		{
			if (D3DERR_OUTOFVIDEOMEMORY == m_hr)
			{
				test->Skip();
			}
			else
			{
				test->Fail();
			}
			return false;
		}
		return true;
	}
private:
    HRESULT m_hr;
};

#endif
