
#ifndef _CSHAPESFVF_H_
#define _CSHAPESFVF_H_

#include "cshapes.h"
#include "d3d.h"
typedef struct 
{
    struct {
        union { 
            D3DVALUE tu; 
            D3DVALUE dvTU; 
        }; 
        union { 
            D3DVALUE tv; 
            D3DVALUE dvTV; 
        }; 
    } Pair[D3DDP_MAXTEXCOORD];
} TEXCOORDS, *LPTEXCOORDS;


class CShapesFVF : public CShapes
{
    public:
    
    // FVF strides

    D3DDRAWPRIMITIVESTRIDEDDATA m_StrideData;
    TEXCOORDS   *m_TexCoords;
    DWORD       m_FVFTypes;
    
    CShapesFVF();
    ~CShapesFVF();

    public:

    // Member functions for conversion to FVF format

    BOOL ConvertToFVF(DWORD dwFlags);
    void ScaleCoords(DWORD dwCoord, D3DVALUE uScale, D3DVALUE vScale);
    void OffsetCoords(DWORD dwCoord, D3DVALUE uOffset, D3DVALUE vOffset);
};

#endif
