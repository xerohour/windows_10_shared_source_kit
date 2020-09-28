/*++
Copyright (c) Microsoft Corporation

Module Name:

    pch.h

Abstract:

    UCX Driver

--*/
#pragma once

#include <ntddk.h>
#include <ntstrsafe.h>
#include <wdf.h>
#include <wdfcx.h>
#include <wdfldr.h>
#include <wdmsec.h>
#include <wdmguid.h>
#include <wdfqueryinterface.h>
#include <ntintsafe.h>
#include <usbspec.h>
#include <usb.h>
#include <usbdlib.h>
#include <usbioctl.h>
#include <usbuser.h>
#include <usbdexinternal.h>
#include <usbbugcode.h>
#include <telemetry\MicrosoftTelemetry.h>

#pragma warning(push)
#include "wdfusb.h"
#pragma warning(pop)

#include "etwenable.h"
#include "ucx.events.h"
#include "usbintrl.h"
#include "ucxdbg.h"
#include "project.h"
#include "ucxclass.h"
#include "hubinterface.h"
#include "ucxversion.h"
#include "ucx.h"
#include "StateMachineUcxEndpoint.h"
#include "StateMachineControllerReset.h"
#include "StateMachineEngine.h"
#include "controller.h"
#include "usbdevice.h"
#include "endpoint.h"
#include "roothub.h"
#include "urb.h"
#include "xrb.h"
#include "streams.h"
#include "usbdi.h"
#include "etw.h"
#include "wmi.h"
#include "registry.h"

//
// Header files for existing usb files
//
#include "hubbusif.h"
#include "usbbusif.h"
#include "wdftriage.h"

#include "UsbSleepStudy.h"
