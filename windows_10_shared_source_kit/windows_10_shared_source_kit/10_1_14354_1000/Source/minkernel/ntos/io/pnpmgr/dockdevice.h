/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    PpProfile.h

Abstract:

    This header contains prototypes for managing hardware profiles and
    docking stations.

Author:

    Adrian J. Oney (AdriaO) 07/19/2000

Revision History:

--*/

#pragma once

#include <pnppo.h>

typedef enum {

    PROFILE_IN_PNPEVENT,
    PROFILE_NOT_IN_PNPEVENT,
    PROFILE_PERHAPS_IN_PNPEVENT

} PROFILE_NOTIFICATION_TIME;

typedef enum _HARDWARE_PROFILE_BUS_TYPE {

    HardwareProfileBusTypeACPI

} HARDWARE_PROFILE_BUS_TYPE, *PHARDWARE_PROFILE_BUS_TYPE;

VOID
PpProfileInit(
    VOID
    );

VOID
PpProfileBeginHardwareProfileTransition(
    __in BOOLEAN SubsumeExistingDeparture
    );

VOID
PpProfileIncludeInHardwareProfileTransition(
    __in  PDEVICE_NODE    DeviceNode,
    __in  PROFILE_STATUS  ChangeInPresence
    );

NTSTATUS
PpProfileQueryHardwareProfileChange(
    __in  BOOLEAN                     SubsumeExistingDeparture,
    __in  PROFILE_NOTIFICATION_TIME   NotificationTime,
    __out PPNP_VETO_TYPE              VetoType,
    __out_opt PUNICODE_STRING             VetoName
    );

VOID
PpProfileCommitTransitioningDock(
    __in PDEVICE_NODE     DeviceNode,
    __in PROFILE_STATUS   ChangeInPresence
    );

VOID
PpProfileCancelTransitioningDock(
    __in PDEVICE_NODE     DeviceNode,
    __in PROFILE_STATUS   ChangeInPresence
    );

VOID
PpProfileCancelHardwareProfileTransition(
    VOID
    );

VOID
PpProfileMarkAllTransitioningDocksEjected(
    VOID
    );

VOID
PpProfileProcessDockDeviceCapability(
    __in PDEVICE_NODE DeviceNode,
    __in PDEVICE_CAPABILITIES Capabilities
    );

#if DBG
#define ASSERT_SEMA_NOT_SIGNALLED(SemaphoreObject) \
    PNP_ASSERT(KeReadStateSemaphore(SemaphoreObject) == 0)
#else // DBG
#define ASSERT_SEMA_NOT_SIGNALLED(SemaphoreObject)
#endif // DBG

typedef struct {

    ULONG           Depth;
    PDEVICE_OBJECT  PhysicalDeviceObject;

} BEST_DOCK_TO_EJECT, *PBEST_DOCK_TO_EJECT;

VOID
PiProfileSendHardwareProfileCommit(
    VOID
    );

VOID
PiProfileSendHardwareProfileCancel(
    VOID
    );

NTSTATUS
PnpProfileUpdateHardwareProfile(
    __out BOOLEAN     *ProfileChanged
    );

NTSTATUS
PiProfileUpdateDeviceTree(
    VOID
    );

VOID
PiProfileUpdateDeviceTreeWorker(
    __in PVOID Context
    );

NTSTATUS
PiProfileUpdateDeviceTreeCallback(
    __in PDEVICE_NODE DeviceNode,
    __in PVOID Context
    );

//
// Functions not yet ported from dockhwp.c
//

NTSTATUS
IopExecuteHardwareProfileChange(
    __in  HARDWARE_PROFILE_BUS_TYPE   Bus,
    __in_ecount(SerialNumbersCount)  PWCHAR *ProfileSerialNumbers,
    __in  ULONG                       SerialNumbersCount,
    __out HANDLE                     *NewProfile,
    __out BOOLEAN                    *ProfileChanged
    );

NTSTATUS
IopExecuteHwpDefaultSelect (
    __in  PCM_HARDWARE_PROFILE_LIST ProfileList,
    __out PULONG ProfileIndexToUse,
    __in  PVOID Context
    );

