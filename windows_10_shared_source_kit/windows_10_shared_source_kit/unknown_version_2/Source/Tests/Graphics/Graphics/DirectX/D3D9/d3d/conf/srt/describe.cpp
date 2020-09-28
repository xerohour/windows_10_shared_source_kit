#define D3D_OVERLOADS
#include <d3d.h>
#include "Srt.h"

void CSRT::DescribeTest(TCHAR * pBuf, int nTexFormat, int nZFormat)
{
    TCHAR tbuf[256],zbuf[256];

    //
    // Texture format
    //
    FmtToString(FORMAT_TEXTURE,&m_pCommonTextureFormats[nTexFormat],tbuf);

    //
    // Depth buffer
    //
    if (m_Options.D3DOptions.bZBuffer)
        FmtToString(FORMAT_ZBUFFER,&m_pCommonZBufferFormats[nZFormat],zbuf);
    else
        _stprintf(zbuf, "Disabled"); 

    _stprintf(pBuf, "Tex(%d): %s Z(%d): %s", nTexFormat, tbuf, nZFormat, zbuf);
}

