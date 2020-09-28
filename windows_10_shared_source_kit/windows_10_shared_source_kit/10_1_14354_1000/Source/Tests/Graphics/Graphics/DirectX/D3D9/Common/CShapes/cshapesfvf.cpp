#include "CShapesPCH.h"

#include "cshapesfvf.h"

CShapesFVF::CShapesFVF()
{
    CShapes::CShapes();
    m_TexCoords = NULL;
    m_FVFTypes = 0;
}

CShapesFVF::~CShapesFVF()
{
    if(NULL != m_TexCoords)
        free(m_TexCoords);
}

BOOL
CShapesFVF::ConvertToFVF(DWORD dwFlags)
{
    //
    // Clear out the current data
    //

    memset(&m_StrideData, 0, sizeof(m_StrideData));

    //
    // Set the appropriate values depending on the flags passed in
    // 

    if(dwFlags & D3DFVF_XYZ)
    {
        m_StrideData.position.lpvData = (LPVOID)(&(m_pVertices->x));
        m_StrideData.position.dwStride = sizeof(D3DVERTEX);
    }
    if(dwFlags & D3DFVF_NORMAL)
    {
        m_StrideData.normal.lpvData = (LPVOID)(&(m_pVertices->nx));
        m_StrideData.normal.dwStride = sizeof(D3DVERTEX);
    }

    //
    // Reserve memory for the texture coords
    //

    m_TexCoords = (TEXCOORDS *)malloc(sizeof(TEXCOORDS) * m_nVertices);
    if(NULL == m_TexCoords)
    {
        return FALSE;
    }

    //
    // Copy tex coords over into new array, replicating into each slot
    //

    for(int i=0; i<m_nVertices; i++)
    {
        for(int j=0; j<D3DDP_MAXTEXCOORD; j++)
        {
            m_TexCoords[i].Pair[j].tu = m_pVertices[i].tu;
            m_TexCoords[i].Pair[j].tv = m_pVertices[i].tv;
        }
    }

    for(int j=0; j<D3DDP_MAXTEXCOORD; j++)
    {
        m_StrideData.textureCoords[j].lpvData = (LPVOID)(&(m_TexCoords[0].Pair[j].tu));
        m_StrideData.textureCoords[j].dwStride = sizeof(TEXCOORDS);
    }
    m_FVFTypes = dwFlags;
    return TRUE;
}

void
CShapesFVF::ScaleCoords(DWORD dwCoord, D3DVALUE uScale, D3DVALUE vScale)
{
    int i;

    for(i=0; i<m_nVertices; i++)
    {
        m_TexCoords[i].Pair[dwCoord].tu *= uScale;
        m_TexCoords[i].Pair[dwCoord].tv *= vScale;
    }
}

void
CShapesFVF::OffsetCoords(DWORD dwCoord, D3DVALUE uOffset, D3DVALUE vOffset)
{
    int i;

    for(i=0; i<m_nVertices; i++)
    {
        m_TexCoords[i].Pair[dwCoord].tu += uOffset;
        m_TexCoords[i].Pair[dwCoord].tv += vOffset;
    }
}
