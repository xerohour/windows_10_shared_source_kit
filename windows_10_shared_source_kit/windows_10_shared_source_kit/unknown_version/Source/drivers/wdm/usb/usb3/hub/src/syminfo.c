/*++

Copyright (c) 2011  Microsoft Corporation

Module Name:

    syminfo.c

--*/

#include "pch.h"
#include "DSM_AutoGen.h"
#include "HSM_AutoGen.h"
#include "ISM_AutoGen.h"
#include "PSM20_AutoGen.h"
#include "PSM30_AutoGen.h"
#include "WppStruct.h"

#define DECLARE_TYPE(Name) Name _DECL_##Name

//
// Keep the following list sorted
//

DECLARE_TYPE (CONTROL_REQUEST);
DECLARE_TYPE (DEVICE_CONTEXT);
DECLARE_TYPE (DEVICE_HW_VERIFIER_FLAGS);
DECLARE_TYPE (DSM_EVENT);
DECLARE_TYPE (DSM_STATE);
DECLARE_TYPE (EVENT);
DECLARE_TYPE (HSM_EVENT);
DECLARE_TYPE (HSM_MUX_CONTEXT);
DECLARE_TYPE (HSM_STATE);
DECLARE_TYPE (HUB_FDO_CONTEXT);
DECLARE_TYPE (HUB_HW_VERIFIER_FLAGS);
DECLARE_TYPE (HUB_INFORMATION);
DECLARE_TYPE (HUB_PDO_CONTEXT);
DECLARE_TYPE (ISM_CONTEXT);
DECLARE_TYPE (ISM_EVENT);	
DECLARE_TYPE (ISM_STATE);
DECLARE_TYPE (ISM_STATE_LOG);
DECLARE_TYPE (LIST_ENTRY);
DECLARE_TYPE (PORT_CONTEXT);
DECLARE_TYPE (PSM_EVENT);
DECLARE_TYPE (PSM20_STATE);
DECLARE_TYPE (PSM30_STATE);
DECLARE_TYPE (SM_CONTEXT);
DECLARE_TYPE (STATE);
DECLARE_TYPE (STATE_LOG);
DECLARE_TYPE (UCX_IOCTL);
DECLARE_TYPE (USB_30_HUB_DESCRIPTOR);
DECLARE_TYPE (USB_BOS_DESCRIPTOR);
DECLARE_TYPE (USB_COMMON_DESCRIPTOR);
DECLARE_TYPE (USB_CONFIGURATION_DESCRIPTOR);
DECLARE_TYPE (USB_DEVICE_CAPABILITY_CONTAINER_ID_DESCRIPTOR);
DECLARE_TYPE (USB_DEVICE_CAPABILITY_DESCRIPTOR);
DECLARE_TYPE (USB_DEVICE_CAPABILITY_PD_CONSUMER_PORT_DESCRIPTOR);
DECLARE_TYPE (USB_DEVICE_CAPABILITY_PLATFORM_DESCRIPTOR);
DECLARE_TYPE (USB_DEVICE_CAPABILITY_POWER_DELIVERY_DESCRIPTOR);
DECLARE_TYPE (USB_DEVICE_CAPABILITY_SUPERSPEED_USB_DESCRIPTOR);
DECLARE_TYPE (USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_USB_DESCRIPTOR);
DECLARE_TYPE (USB_DEVICE_CAPABILITY_USB20_EXTENSION_DESCRIPTOR);
DECLARE_TYPE (USB_DEVICE_DESCRIPTOR);
DECLARE_TYPE (USB_DEVICE_STATUS);
DECLARE_TYPE (USB_ENDPOINT_DESCRIPTOR);
DECLARE_TYPE (USB_HUB_DESCRIPTOR);
DECLARE_TYPE (USB_ID_STRING);
DECLARE_TYPE (USB_INTERFACE_ASSOCIATION_DESCRIPTOR);
DECLARE_TYPE (USB_INTERFACE_DESCRIPTOR);
DECLARE_TYPE (USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR);
DECLARE_TYPE (USB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR);
DECLARE_TYPE (USB_PORT_STATUS_AND_CHANGE);
DECLARE_TYPE (WPP_TRACE_CONTROL_BLOCK);

//
// Make it build
//

int __cdecl main() {
    return 0;
}
