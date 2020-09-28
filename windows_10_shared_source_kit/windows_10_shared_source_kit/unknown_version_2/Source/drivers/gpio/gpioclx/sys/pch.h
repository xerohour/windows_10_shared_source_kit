/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    pch.h

Abstract:

    This file contains pre-compiled header files.


Environment:

    Kernel mode

--*/

#ifndef NO_PCH_INCLUDES

#pragma once

#define NT_PROCESSOR_GROUPS 1

#include "common.h"

#pragma warning(push)
#pragma warning(disable:4214) /* nonstandard extension used: bit field types other then int */
#pragma warning(disable:4201) /* nonstandard extension used: nameless struct/union */
#include <acpitabl.h>
#include "ntacpi.h"
#include <acpiioct.h>
#include <acpiioct_p.h>
#pragma warning(pop)

#include <gpioclx.h>
#include <gpioclx_p.h>
#include <gpio.h>
#include <gpio_p.h>
#include <ntpoapi.h>
#include <restrans.h>
#define RESHUB_USE_HELPER_ROUTINES
#include <reshub.h>
#include <reshub_p.h>
#include <secintr.h>

#include "GpioClxEvents.h"

#if defined(EVENT_TRACING)

#include <wpprecorder.h>

#else

//
// Mock version of the handle type used everywhere.
// We set all instances of this type to NULL.
//

typedef PVOID RECORDER_LOG;

#endif
#pragma warning(push)
#pragma warning(disable:4214) /* nonstandard extension used: bit field types other then int */
#pragma warning(disable:4201) /* nonstandard extension used: nameless struct/union */

#include "intdebounce.h"
#include "gpioclxi.h"
#include "hub.h"
#include "clientinvk.h"
#include "trace.h"

#pragma warning(pop)

#define INITGUID
#include <guiddef.h>
#include <power.h>

#endif

