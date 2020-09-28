#include "ObjectPCH.h"

#include "CObject.h"
#include "TestUtils.h"
#include "d3d8.h"

CObject::CObject()
{
    m_ulRef=1;
}

CObject::~CObject()
{
    m_ulRef=0;
}

bool CObject::Initialize(void)
{
    return true;
}

ULONG WINAPI CObject::AddRef(void)
{
    return ++m_ulRef;
}

ULONG WINAPI CObject::Release(void)
{
    m_ulRef--;

    if (0 == m_ulRef)
    {
        delete this;
        return 0;
    }

    return m_ulRef;
}

ULONG CObject::GetRefCount(void)
{
    return m_ulRef;
}

HRESULT WINAPI CObject::QueryInterface(REFIID iid, PVOID * ppRes)
{
    IUnknown * pObject = NULL;
    HRESULT hr;
	IUnknown* pInternal = (IUnknown*)GetInternalPointer();

	if (!pInternal)
		return E_FAIL;

    hr = pInternal->QueryInterface(iid, PassPointer((void**)&pObject, ppRes));

    if (FAILED(hr))
    {
		return hr;
    }

    if ( ppRes )
    {
        *ppRes = pObject;
    }

    return hr;
}

void * CObject::GetInternalPointer(void)
{
	return NULL;
}

bool operator==(CObject& o1, CObject& o2)
{
	return o1.GetInternalPointer() == o2.GetInternalPointer();
}

bool operator!=(CObject& o1, CObject& o2)
{
	return o1.GetInternalPointer() != o2.GetInternalPointer();
}
