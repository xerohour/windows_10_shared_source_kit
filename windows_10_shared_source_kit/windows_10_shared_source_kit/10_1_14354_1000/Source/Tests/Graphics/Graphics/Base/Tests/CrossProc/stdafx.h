// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <ddraw.h>
#include <WTTLogger.h>

#include <string>
#include <exception>

using namespace std;


#include "BaseVid.h"
#include "GenericI3DComponent.h"

//#include "../../common/VidWTTLog/CVidWttLog.h"

//Required for getting the unique adapter handles
#include "initguid.h"
#include <ntddvdeo.h>
#include "d3dkmthk.h"

#include "common.h"
#include "utils.h"
#include "AdapterDisplayInfo.h"
#include "owner.h"
#include "user.h"

#include <commctrl.h>
#include <objbase.h>
#include <comdef.h>



// TODO: reference additional headers your program requires here
