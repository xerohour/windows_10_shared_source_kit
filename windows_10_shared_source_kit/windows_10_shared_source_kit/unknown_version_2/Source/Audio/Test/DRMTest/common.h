// Copyright (C) Microsoft. All rights reserved.
#ifndef _COMMON_H
#define _COMMON_H

#define __AUDIOCORE_MSSAP

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#undef ASSERT

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <dsound.h>
#include <dsprv.h>
#include <mmddkp.h>
DEFINE_GUID(GUID_NULL, 0L, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
#include <atlstr.h>

#include <ks.h>
#include <ksmedia.h>
#include <list.h>
#include <kslib.h>
#include <debug.h>

#include <wincrypt.h>

#ifndef BUILD_TAEF
#include <shlflags.h>
#include <shltypes.h>
#include <shell98.h>
#include <module98.h>
#else
#include <module_taef.h>
#include "wexstring.h"
#endif
#include <TaefModuleAdapter.h>
#include <sndanlyz.h>
#include <signal-source.h>
#include <dithering.h>
#include <sine-signal-source.h>
#include <QueryMachineInfo.h>

#include <drmkmain.h>

#include "ksutil.hxx"
#include "cmixer.hxx"    

#include "testflag.h"
#include "device.h"
#include "drmapi.h"
#include "util.h"
#include "kshlp.h"
#include "drmtest.h"
#include "resource.h"
#include "testcases.h"
#include "KSTest.h"
#include "ntversion.h"


#include <map>
#include <string>

#include "PropertyStore.h"

#endif
