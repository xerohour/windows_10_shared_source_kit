#include "ResourcePCH.h"

#include "CResource.h"
#include "CDevice.h"

// --------------------------------------------------------------------------------------
CResource::CResource(void)
{
	m_pResource = NULL;
    m_pDevice = NULL;

    pGetPriority = &CResource::FailGetPriority;
    pSetPriority = &CResource::FailSetPriority;
    pGetType     = &CResource::FailGetType;
}

CResource::~CResource(void)
{
    m_pResource = NULL;
    m_pDevice = NULL;

    pGetPriority = NULL;
    pSetPriority = NULL;
    pGetType     = NULL;
}

// --------------------------------------------------------------------------------------

HRESULT CResource::Initialize(PVOID pResource, RESOURCETYPE type, CDevice* pDevice)
{
    HRESULT hResult = E_FAIL;

    if( NULL == pDevice || NULL == pResource ) return hResult;
    m_pDevice = pDevice;

    if (9.0f <= m_pDevice->GetInterface() && 10.0f > m_pDevice->GetInterface())
    {
        pGetPriority = &CResource::GetPriority9;
        pSetPriority = &CResource::SetPriority9;
        pGetType = &CResource::GetType9;
        hResult = m_pDevice->ExtractResource9(pResource, type, &m_pResource);
    }

     return  hResult;
}