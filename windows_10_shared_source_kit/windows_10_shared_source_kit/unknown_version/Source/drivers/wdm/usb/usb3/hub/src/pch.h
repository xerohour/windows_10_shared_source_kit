/*++
Copyright (c) Microsoft Corporation

Module Name:

    pch.h

Abstract:

    USB HUB 3  Driver

--*/

#pragma once

#include <ntifs.h>
#include <initguid.h>
#include <wdf.h>
#include <wdmguid.h>
#include <ntstrsafe.h>
#include <ntintsafe.h>
#include <driverspecs.h>
#include <msosdesc.h>
#include <devguid.h>
#include <kshimeng.h>
#include <acpiioct.h>
#include <usbdexinternal.h>
#include <WppRecorder.h>
#include <ntddstor.h>
#include <devpkey.h>
#include <acpitabl.h>
#include <UCM.h>
#include <usbdevpkey.h>
#include <telemetry\MicrosoftTelemetry.h>

#include "usbspec.h"
#include "usb.h"
#define USB20_API
#include "usbioctl.h"
#include "usbintrl.h"
#include "usbcommon.h"
#include "usberrata.h"
#include "usbbusif.h"
#include "usbboot.h"
#include "hubinterface.h"
#include "msos20desc.h"
#include "ucxclass.h"
#include "usb300.h"
#include "dbg.h"
#include "usbbugcode.h"
#include "project.h"
#include "telemetry.h"
#include "hubdef.h"
#include "smstructs.h"
#include "acpi.h"
#include "usbwnfdata.h"
#include "hubfdo.h"
#include "hubidle.h"
#include "hsmpublic_autogen.h"
#include "psmpublic_autogen.h"
#include "dsmpublic_autogen.h"
#include "ismpublic_autogen.h"
#include "hsmmux.h"
#include "hubxfer.h"
#include "hubboot.h"
#include "devicexfer.h"
#include "driver.h"
#include "hubmisc.h"
#include "wnf.h"

#ifdef USBMSG
#include "usbmsg.h"
#endif

#include "usbhub3.Events.h"
#include "tracing.h"
#include "hubpdo.h"
#include "idstring.h"
#include "hubreg.h"
#include "descvalidation.h"
#include "connector.h"
#include "wer.h"
#include "parent.h"
#include "ucx.h"
#include "wmi.h"
#include "wdftriage.h"

#include "usbwnfdata.h"
#include "UsbSleepStudy.h"
#include <pep_x.h> // required by private PO interfaces
#include <ntpoapi_p.h> // private PO interfaces
