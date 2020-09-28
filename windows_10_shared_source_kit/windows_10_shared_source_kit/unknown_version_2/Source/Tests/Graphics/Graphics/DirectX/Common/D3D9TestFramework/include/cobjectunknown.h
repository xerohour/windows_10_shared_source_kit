// ======================================================================================
//
//  CObjectUnknown.h
//
//  Header file for the CObjectUnknown class.
//
//
//  Copyright (C) 2000 Microsoft Corporation
//
// ======================================================================================

#ifndef __COBJECTUNKNOWN_H__
#define __COBJECTUNKNOWN_H__

#include <CObject.h>

class CObjectUnknown : public CObject
{
protected:
	virtual void* GetInternalPointer(void)
    {
        return m_pUnknown;
    }

public:
    CObjectUnknown()
    {
        m_pUnknown = 0;
    }
    ~CObjectUnknown()
    {
        if (m_pUnknown)
        {
            ULONG count = m_pUnknown->Release();
        }
    }
    bool Init(void* pUnknown)
    {
        if (pUnknown && !m_pUnknown)
        {
            m_pUnknown = (IUnknown*)pUnknown;
            ULONG count = m_pUnknown->AddRef();
            return true;
        }
        return false;
    }

private:
    IUnknown* m_pUnknown;
};

#endif //__COBJECT_H__
