// Copyright(C) Microsoft.All rights reserved.

#pragma once

// Header files for Shell98
#include <windows.h>
#include <tchar.h>
#ifndef BUILD_TAEF
#include <shlflags.h>
#include <shell98.h>
#include <module98.h>
#else //#ifndef BUILD_TAEF
#include <module_taef.h>
#endif //#ifndef BUILD_TAEF
#include <TaefModuleAdapter.h>
#include <dvmodule.h>

//#include "ksimport.h"
#include "resource.h"

// Header files for KS specific functions
// Change warning level to 3 for ks.h since it has compile
// problems at warning level 4
#pragma warning (push, 3)
#include <ks.h>
#pragma warning (pop)
// we are using the old header
// so we need ksmedia.h
#if defined(_KSMEDIAP_H) 
#include <ksmedia.h>
#endif //#if defined(_KSMEDIAP_H) 
#include <mmsystem.h>
#include <mmdeviceapi.h>

#include <devioctl.h>

#include <atlbase.h>
#include <atlcoll.h>    // For the CAtlList template class
#include <atlconv.h>
#include <stdio.h>
#include <time.h>

#include <strstream>
#include <vector>
#include <map>

#include <TopoLib.h>

////////
// NOTE: These are taken from //depot/<branch>/minkernel/published/internal/ksmedia.w
////////
#if (NTDDI_VERSION >= NTDDI_WINBLUE)
#define KSPROPERTY_AUDIO_LAST KSPROPERTY_AUDIO_VOLUMELIMIT_ENGAGED
#elif (NTDDI_VERSION >= NTDDI_WIN8)
#define KSPROPERTY_AUDIO_LAST KSPROPERTY_AUDIO_PEAKMETER2
#elif (NTDDI_VERSION >= NTDDI_VISTA)
#define KSPROPERTY_AUDIO_LAST KSPROPERTY_AUDIO_MIC_ARRAY_GEOMETRY
#endif

// Include project specific headers here
#include "KsTopTest.h"
#include "TestCases.h"
#include "proptests.h"
#include "nodetests.h"
#include "pintests.h"
#include "filtertests.h"
#include "KsTopGraph.h"
#include "FilterTopGraph.h"
#define IF_FAILED_HR_LOG(hr,XLOGTYPE,EErrorLevel,msg) \
    if (FAILED(hr)) \
    { \
        XLOG(XLOGTYPE, EErrorLevel, "%s: hr = 0x%08x", msg, hr); \
         \
    } else {0;}
