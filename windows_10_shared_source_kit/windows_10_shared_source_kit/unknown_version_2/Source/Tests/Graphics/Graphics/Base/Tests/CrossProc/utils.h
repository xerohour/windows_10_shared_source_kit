#pragma once

#include "CTlLogger.h"


HRESULT UtilReadPixel(CI3DSurface2Ptr pSrf, D3DRECT * pRect, SColor * pColor);
float   UtilCalcDeviation(SColor * pColor, float ExpAlpha, float ExpRed, float ExpGreen, float ExpBlue);
void    UtilPrintMessage(Message * msg,CCTLLogger* _Log);
HRESULT UtilCreateUserProcess();

