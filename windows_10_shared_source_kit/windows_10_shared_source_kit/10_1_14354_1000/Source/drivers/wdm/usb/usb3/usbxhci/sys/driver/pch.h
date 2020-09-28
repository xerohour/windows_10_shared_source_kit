/*++
Copyright (c) Microsoft Corporation

Module Name:

    pch.h

Abstract:

    USB XHCI Driver

--*/
#define COMMAND_FLTR


#include <initguid.h>
#include <ntosp.h>

#undef NT_PROCESSOR_GROUPS  // ntosp.h defines this, but doesn't currently work without driver.

#include <nt.h>
#include <ntrtl.h>
#include <zwapi.h>
#include <wdf.h>

#include <ntstrsafe.h>
#include <ntintsafe.h>
#include <driverspecs.h>
#include <limits.h>

typedef GUID *PGUID;        // guiddef.h does not define PGUID, wdm.h does. And kshimeng.h depends on PGUID.

#include <kshimeng.h>
#include <wdfcx.h>          // Needed for WDF_REQUEST_REUSE_MUST_COMPLETE definition
#include <devpkey.h>
#include <acpiioct.h>
#include <dsfhrmports.h>
#include <wdmguid.h>
#include <ursinterface.h>
#include <telemetry\MicrosoftTelemetry.h>
#include <lpe.h>

#include "usbspec.h"
#include "usb.h"
#include "usbioctl.h"
#include "hubbusif.h"
#include "usbbusif.h"
#include "usbuser.h"
#include "usbboot.h"
#include "usberrata.h"
#include "usbintrl.h"

#pragma warning(push)
#include "wdfusb.h"
#pragma warning(pop)
#include "project.h"
#include "dbg.h"
#include "usbbugcode.h"
#include "tracing.h"
#include "usb300.h"
#include "ucxclass.h"
#include "telemetry.h"
#include "xhciwmi.h"

#include "etwenable.h"
#include "usbxhci.events.h"

#include "usbxhci.h"

#include "smstructs.h"
#include "ESMPublic_AutoGen.h"

#include "tags.h"
#include "crashdumpcommon.h"
#include "driver.h"
#include "controller.h"
#include "iocontrol.h"
#include "register.h"
#include "commonbuffer.h"
#include "interrupter.h"
#include "command.h"
#ifdef COMMAND_FLTR
#include "commandfilter.h"



#endif
#include "deviceslot.h"
#include "roothub.h"
#include "usbdevice.h"
#include "endpoint.h"
#include "StageQueue.h"
#include "TR.h"
#include "Control.h"
#include "Isoch.h"
#include "Bulk.h"
#include "etw.h"
#include "wmi.h"
#include "requestdata.h"

#include "crashdumpeventring.h"
#include "crashdumpregister.h"
#include "crashdumpcommand.h"
#include "crashdumpendpoint.h"
#include "crashdumpusbdevice.h"
#include "crashdump.h"
#include "wdftriage.h"

#include "usbxhci.counters.h"
#include "counter.h"

#ifdef ICE_CAP
//
// See README.txt in the \icecap directory to find where to get ice.h from.
//
#include "ice.h"
#else
#define IceSetTraceState(x)
#endif

#define ICE_START_TRACE() IceSetTraceState(TRUE)
#define ICE_STOP_TRACE()  IceSetTraceState(FALSE)

