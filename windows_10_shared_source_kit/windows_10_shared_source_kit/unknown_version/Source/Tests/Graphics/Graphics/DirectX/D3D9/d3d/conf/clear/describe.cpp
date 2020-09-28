#define D3D_OVERLOADS
#include <d3d.h>
#include "clear.h"

void CClearTest::DescribeFormat(TCHAR * pBuf, int nTexFormat, int nZFormat)
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



void CClearTest::DescribeTestcase(TCHAR * pBuf, int nTestcase)
{
    TCHAR szScissor[32];

    if (m_bScissor)
        _stprintf(szScissor, "(scissor disabled)");
    else
        _stprintf(szScissor, "(scissor na)");

	switch(nTestcase)
	{
		case 14:
            _stprintf(szScissor, "(scissor enabled)");
		case 0:
			_stprintf(pBuf, "(%2d) Clear RGB full screen %s.", nTestcase, szScissor);
			break;
		case 15:
		case 16:
            _stprintf(szScissor, "(scissor enabled)");
		case 1:
		case 2:
			_stprintf(pBuf, "(%2d) Clear alpha full screen %s.", nTestcase, szScissor);
			break;
		case 17:
		case 18:
            _stprintf(szScissor, "(scissor enabled)");
		case 3:
		case 4:
			_stprintf(pBuf, "(%2d) Clear Z full screen %s.", nTestcase, szScissor);
			break;
		case 19:
		case 20:
            _stprintf(szScissor, "(scissor enabled)");
		case 5:
		case 6:
			_stprintf(pBuf, "(%2d) Clear stencil full screen %s.", nTestcase, szScissor);
			break;
		case 21:
            _stprintf(szScissor, "(scissor enabled)");
		case 7:
			_stprintf(pBuf, "(%2d) Clear Z affect stencil %s.", nTestcase, szScissor);
			break;
		case 22:
            _stprintf(szScissor, "(scissor enabled)");
		case 8:
			_stprintf(pBuf, "(%2d) Clear stencil affect Z %s.", nTestcase, szScissor);
			break;
		case 23:
            _stprintf(szScissor, "(scissor enabled)");
		case 9:
			_stprintf(pBuf, "(%2d) Clear render-target alone affect Z or stencil %s.", nTestcase, szScissor);
			break;
		case 24:
            _stprintf(szScissor, "(scissor enabled)");
		case 10:
			_stprintf(pBuf, "(%2d) Clear RGB - list of rects %s.", nTestcase, szScissor);
			break;
		case 25:
            _stprintf(szScissor, "(scissor enabled)");
		case 11:
			_stprintf(pBuf, "(%2d) Clear alpha - list of rects %s.", nTestcase, szScissor);
			break;
		case 26:
            _stprintf(szScissor, "(scissor enabled)");
		case 12:
			_stprintf(pBuf, "(%2d) Clear Z - list of rects %s.", nTestcase, szScissor);
			break;
		case 27:
            _stprintf(szScissor, "(scissor enabled)");
		case 13:
			_stprintf(pBuf, "(%2d) Clear stencil - list of rects %s.", nTestcase, szScissor);
			break;
		default:
			_stprintf(pBuf, "(%2d) DCT test error!", nTestcase);
			break;
	}

}
