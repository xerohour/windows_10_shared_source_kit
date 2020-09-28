//
//    Copyright (C) Microsoft.  All rights reserved.
//
/*
    Prototypes for hub driver (NEW)
*/
#include <telemetry\MicrosoftTelemetry.h>

#include "busfunc.h"
#include "pind.h"
#include "overc.h"
#include "dioctl.h"
#include "busm.h"
#include "lock.h"
#include "usbd.h"
#include "msos.h"
#include "bus.h"
#include "busif.h"
#include "dq.h"
#include "ex.h"
#include "hub.h"
#include "usberrata.h"
#include "idstring.h"
#include "ioctl.h"
#include "log.h"
#include "pchange.h"
#include "usbcommon.h"
#include "pdo.h"
#include "pnp.h"
#include "fdopwr.h"
#include "pdopwr.h"
#include "refobj.h"
#include "reg.h"
#include "reset.h"
#include "ssdev.h"
#include "sshub.h"
#include "timer.h"
#include "wmi.h"
#include "uxd.h"
#include "etw.h"
#include "extboot.h"
#include "hrtimer.h"
#include <UCM.h>

#include  "tags.h"

#include "hubutil.h"
#include "busifdef.h"

#include "usbmsg.h"
#include "usbhub.events.h"

//-----------------------------------------------------------------------------
// 4127 -- Conditional Expression is Constant warning
//-----------------------------------------------------------------------------
#define WHILE(constant) \
__pragma(warning(disable: 4127)) while(constant); __pragma(warning(default: 4127))

#define WHILE_TRUE \
__pragma(warning(disable: 4127)) while(1) __pragma(warning(default: 4127))




