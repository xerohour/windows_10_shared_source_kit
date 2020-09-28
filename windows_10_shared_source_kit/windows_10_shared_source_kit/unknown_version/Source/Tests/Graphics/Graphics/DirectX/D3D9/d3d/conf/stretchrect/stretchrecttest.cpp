#include "stretchrecttest.h"
#include "StretchRectTestMain.h"
#include "StretchRectDepthStencil.h"
#include "StretchRectKeepStates.h"


CD3DWindowFramework_Limit_Failures	TestApp;

CStretchRectTestMainNonMS			MainGroupNonMS;
CStretchRectTestMainMS				MainGroupMS;
CStretchRectTestMainLDDM			MainGroupLDDM;
CStretchRectDepthStencil			tstStretchRectDepthStencil;
CStretchRectColorWriteEnable		tstStretchRectColorWriteEnable;
CStretchRectFillMode				tstStretchRectFillMode;
CStretchRectShadeMode				tstStretchRectShadeMode;
CStretchRectLastPixel				tstStretchRectLastPixel;
CStretchRectSrcBlend				tstStretchRectSrcBlend;
CStretchRectDitherEnable			tstStretchRectDitherEnable;
CStretchRectSpecularEnable			tstStretchRectSpecularEnable;
CStretchRectPixelFog				tstStretchRectPixelFog;
CStretchRectVertexFog				tstStretchRectVertexFog;
CStretchRectAmbient					tstStretchRectAmbient;
