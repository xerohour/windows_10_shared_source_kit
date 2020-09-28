// ======================================================================================
//
//  CObject.h
//
//  Header file for the CObject class.
//
//	This sets up the standard COM interface for test objects.
//
//  Copyright (C) 2000 Microsoft Corporation
//
// ======================================================================================

#ifndef __COBJECT_H__
#define __COBJECT_H__

#include <windows.h>

class CObject
{
    ULONG m_ulRef;

protected:
	virtual void*	GetInternalPointer(void);

public:
    CObject();

    // Part of standard IUknown
    virtual HRESULT WINAPI QueryInterface(REFIID iid, PVOID *);
    virtual ULONG   WINAPI AddRef(void);
    virtual ULONG   WINAPI Release(void);

    // helper functions
    virtual bool    Initialize(void);
    virtual ULONG   GetRefCount(void);

    virtual ~CObject();

	friend bool operator==(CObject& o1, CObject& o2);
	friend bool operator!=(CObject& o1, CObject& o2);
};

#endif //__COBJECT_H__
