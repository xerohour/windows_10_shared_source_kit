/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    pooltags.h

Abstract:

    This module contains pool tags for PNP subsystem.

Author:

    Santosh Jodh (santoshj) 08-18-2004

Environment:

    KernelMode only.

Revision History:

--*/

#pragma once

//
// This tag is used for allocating the resource request table during rebalance.
//

#define PNP_POOLTAG_REBALANCE_REQUEST_TABLE         '0pnP'

//
// This tag is used for allocating the completion context for asynchronous PNP IRPs.
//

#define PNP_POOLTAG_DEVICE_COMPLETION_REQUEST       '1pnP'

//
// This tag is used for allocating device action requests.
//

#define PNP_POOLTAG_DEVICE_ACTION_REQUEST           '2pnP'

//
// This tag is used for hw profile related stuff.
//

#define PNP_POOLTAG_HWPROFILE                       '3pnP'

//
// This tag is used by the CMAPI component.
//

#define PNP_POOLTAG_CMAPI                           '4pnP'

//
// This tag is used for allocating the context used for building the group of devices that
// need resources assigned.
//

#define PNP_POOLTAG_ASSIGN_RESOURCES_CONTEXT        '5pnP'

//
// This tag is used for allocating the resource request from a device.
//

#define PNP_POOLTAG_RESOURCE_REQUEST                '6pnP'

//
// This tag is used for allocating a deferred notification entry.
//

#define PNP_POOLTAG_DEFERRED_NOTIFY                 '7pnP'

//
// This tag is used for allocating a copy of async target device change
// notification.
//

#define PNP_POOLTAG_ASYNC_TDC_NOTIFY                '8pnP'

//
// This tag is used for allocating the hardware profile change
// notification.
//

#define PNP_POOLTAG_HW_PROFILE_NOTIFY               '9pnP'

//
// This tag is used for PnpRtl operation related allocations.
//

#define PNP_POOLTAG_PNPRTL_OPERATIONS               'ApnP'

//
// This tag is currently unused.
//

#define PNP_POOLTAG_PENDING_OPERATIONS              'BpnP'

//
// This tag is used for allocating the target device
// notification.
//

#define PNP_POOLTAG_TARGET_DEVICE_NOTIFY            'CpnP'

//
// This tag is used for allocating the device class
// notification.
//

#define PNP_POOLTAG_DEVICE_CLASS_NOTIFY             'DpnP'

//
// This tag is used for keeping context of device removal requests for reset
//

#define PNP_POOLTAG_RESET_REMOVAL                    'EpnP'

//
// This tag is used for allocating the eject data used
// by IoRequestDeviceEjectEx.
//

#define PNP_POOLTAG_EJECT_DATA                      'FpnP'

//
// This tag is used for generic temporary allocations.
//

#define PNP_POOLTAG_GENERIC                         'GpnP'

//
// This tag is used for allocating service name in the devnode.
//

#define PNP_POOLTAG_SERVICE_NAME                    'HpnP'

//
// This tag is used for allocating instance path in the devnode.
//

#define PNP_POOLTAG_INSTANCE_PATH                   'IpnP'

//
// This tag is used for device event list allocation.
//

#define PNP_POOLTAG_DEVICE_EVENT_LIST               'JpnP'

//
// This tag is used for device event entry allocation.
//

#define PNP_POOLTAG_DEVICE_EVENT_ENTRY              'KpnP'

//
// This tag is used for device event work item allocation.
//

#define PNP_POOLTAG_DEVICE_EVENT_WORK_ITEM          'LpnP'

//
// This tag is used for internal veto buffer allocation.
//

#define PNP_POOLTAG_INTERNAL_VETO_BUFFER            'MpnP'

//
// This tag is used for PDO array allocation.
//

#define PNP_POOLTAG_PDO_ARRAY                       'NpnP'

//
// This tag is used for allocating the veto process list.
//

#define PNP_POOLTAG_VETO_PROCESS                   'OpnP'

//
// This tag is used for allocating the veto process list.
//

#define PNP_POOLTAG_VETO_DEVICE_OBJECT             'PpnP'

//
// This tag is used for memory resource list in a target partition.
//

#define PNP_POOLTAG_PARTITION_RESOURCE_LIST        'QpnP'

//
// This tag is used for memory bitmap buffer.
//

#define PNP_POOLTAG_MEMORY_BITMAP                   'RpnP'

//
// This tag is used for allocating device dependency management structures.
//

#define PNP_POOLTAG_DEPENDENCY_INFO                 'SpnP'

//
// This tag is used for device removal structures.
//

#define PNP_POOLTAG_DEVICE_REMOVAL                  'TpnP'

//
// This tag is used for allocating a status control block when handling
// a user mode request to set the device status asynchronously.
//

#define PNP_POOLTAG_ASYNC_SET_STATUS                'UpnP'

//
// This tag is used for allocating a notify entry lock to synchronize access
// to a given notification entry.
//

#define PNP_POOLTAG_NOTIFY_ENTRY_LOCK               'VpnP'

//
// This tag is used for allocating Software Device structures
//

#define PNP_POOLTAG_SW_DEVICE                       'WpnP'

//
// This tag is used for allocating for DevQuery structures
//

#define PNP_POOLTAG_DEV_QUERY                       'XpnP'

//
// This tag is used for allocating user-mode event notification engine structures
//

#define PNP_POOLTAG_UMODE_EVENTS                    'YpnP'

//
// This tag is used for allocating data in the Data Model component.
//

#define PNP_POOLTAG_DATA_MODEL                      'ZpnP'


//
// This tag is used for capturing parameters from user mode via NtPlugPlayControl
//

#define PNP_POOLTAG_UMODE_ARGUMENTS                 '  pP'

//
// This tag is used for allocating tracing information analyze rebalance failure
//

#define PNP_POOLTAG_REBALANCE_TRACE_CONTEXT         'beRP'
