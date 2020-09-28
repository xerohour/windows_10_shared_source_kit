// ======================================================================================
//
//  CResource.h
//
//  Header file for the CResource class.
//
//	This is a wrapper for all versions of resources
//
//  Copyright (C) 2000 Microsoft Corporation
//
// ======================================================================================

#ifndef __CResource_h__
#define __CResource_h__

#include "Framework.h"

class CResource
{
protected:
	friend class CDevice;

    DWORD   (CResource::*pGetPriority)(void);
    virtual DWORD FailGetPriority(void) {return 0;}
    virtual DWORD GetPriority9();

    DWORD   (CResource::*pSetPriority)(DWORD PriorityNew);
    virtual DWORD FailSetPriority(DWORD) {return 0;}
    virtual DWORD SetPriority9(DWORD PriorityNew);

    RESOURCETYPE (CResource::*pGetType)(void);
    virtual RESOURCETYPE FailGetType(void) {return RTYPE_FORCE_DWORD;}
    virtual RESOURCETYPE GetType9(void);
  

    PVOID GetResource() {return m_pResource;}

    CDevice* m_pDevice;
    PVOID    m_pResource;

public:

    CResource(void);
    ~CResource(void);

    virtual HRESULT Initialize(PVOID pResource, RESOURCETYPE type, CDevice *pDevice);

    virtual DWORD   GetPriority(void) {return (this->*pGetPriority)();}
    virtual DWORD   SetPriority(DWORD PriorityNew) {return (this->*pSetPriority)(PriorityNew);}

    virtual RESOURCETYPE   GetType(void) {return (this->*pGetType)();}
};

#endif