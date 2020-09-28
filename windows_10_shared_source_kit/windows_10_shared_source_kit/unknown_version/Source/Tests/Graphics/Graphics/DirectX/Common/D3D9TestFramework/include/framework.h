#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

#ifndef INITGUID
	#define INITGUID
#endif

#ifndef STRICT
	#define STRICT
#endif

// Disabling some errors because they have previously masked by STL includes
// which have disabled them.  Consequently, we have a TON of these failures throughout
// the test source and framework.  We should fix them sometime in the future.

#pragma warning( disable : 4018 )	// signed / unsigned mismatch


#ifdef _IA64_   // Removes IA64 compiler alignment warnings
  #pragma pack(16)
#endif

// --------------------------------------------------------------------------------------
// PREFAST handling

#ifndef _PREFAST_
#pragma warning(disable:4068)
#endif

#pragma prefast(disable: 321, "We want relative path when loading DLLs for testing purposes.") 

// --------------------------------------------------------------------------------------
// Forward declarations

class	CDevice;
class	CD3D;
class	CD3DX;
class	CTestLog;
class	CD3DTestLog;
class	CD3DWindowFramework;
class	CSurface;
class	CBaseTexture;
class	CTexture;
class	CCubeTexture;
class	CVolumeTexture;
class	CImageCompare;
class	CSwapChain;
class	CnSurface;
class	CVertexBuffer;
class	CIndexBuffer;
class	CTest;
class	CD3DTest;
class	CTestManager;
class	CStateblock;
class	CD3DXRenderToSurfaceFW;
class	CD3DXRenderToEnvMapFW;
class	CVertexShader;
class	CnVertexShader;
class	CPShader;
class	CnPixelShader;
class	CVertexDeclaration;
class	CnVertexDeclaration;
class	CEffect;
class	CnEffect;
class   CResource;

#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <commctrl.h>
#include "FrameworkTypes.h"
#include "CObject.h"
#include "CDebugOut.h"
#include "TestUtils.h"
#include "HelperFunctions.h"
#include "CWindow.h"
#include "CTestManager.h"
#include "CCommandline.h"
#include "DXG9Logger.h"
#include "CD3DTest.h"
#include "CDevice.h"
#include "CD3D.h"
#include "CD3DWindowFramework.h"
#include "FrameworkResource.h"

#endif
