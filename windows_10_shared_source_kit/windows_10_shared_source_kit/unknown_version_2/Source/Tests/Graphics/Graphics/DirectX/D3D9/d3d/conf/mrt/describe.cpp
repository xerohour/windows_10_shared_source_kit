#include <d3d9.h>
#include "MRT.h"

void CMRT::DescribeTest(TCHAR * pBuf, FORMAT **ppTexFormat, FORMAT *pZFormat)
{
    TCHAR tbuf[80],zbuf[80];

    //
    // Texture format
    //
	for (int i=0; i < nNumRTs; i++)
	{
		FmtToString(FORMAT_TEXTURE,ppTexFormat[i],tbuf);

		_stprintf(pBuf, "%sTex%d:%s ", pBuf, i, tbuf);
	}

    //
    // Depth buffer
    //
    FmtToString(FORMAT_ZBUFFER,pZFormat,zbuf);

    _stprintf(pBuf, "%sZ:%s", pBuf, zbuf);
}

