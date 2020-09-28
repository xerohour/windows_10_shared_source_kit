#include "ResourcePCH.h"
#include <d3d9.h>
#include "CResource.h"
#include "CDevice.h"


DWORD CResource::GetPriority9()
{
    if( NULL == m_pResource ) return 0;

    return ((LPDIRECT3DRESOURCE9)m_pResource)->GetPriority();
}

DWORD CResource::SetPriority9(DWORD PriorityNew)
{
    if( NULL == m_pResource ) return 0;

    return ((LPDIRECT3DRESOURCE9)m_pResource)->SetPriority(PriorityNew);
}

RESOURCETYPE CResource::GetType9()
{
    if( NULL == m_pResource ) return RTYPE_FORCE_DWORD;

    D3DRESOURCETYPE d3dtype = ((LPDIRECT3DRESOURCE9)m_pResource)->GetType();
    RESOURCETYPE type;

    switch( d3dtype )
    {
        case( D3DRTYPE_SURFACE ):
            type = RTYPE_SURFACE;
            break;
        case( D3DRTYPE_VOLUME ):
            type = RTYPE_VOLUME;
            break;
        case( D3DRTYPE_TEXTURE ):
            type = RTYPE_TEXTURE;
            break;
        case( D3DRTYPE_VOLUMETEXTURE ):
            type = RTYPE_VOLUMETEXTURE;
            break;
        case( D3DRTYPE_VERTEXBUFFER ):
            type = RTYPE_VERTEXBUFFER;
            break;
        case( D3DRTYPE_INDEXBUFFER ):
            type = RTYPE_INDEXBUFFER;
            break;
        default:
            type = RTYPE_FORCE_DWORD;
            break;                
    }

    return type;
}