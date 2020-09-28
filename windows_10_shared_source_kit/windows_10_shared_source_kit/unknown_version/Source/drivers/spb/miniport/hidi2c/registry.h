/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    registry.h

Abstract:

   Contains declarations for functions that retrieve settings
   from the registry

Author:

    Arvind Aiyar March 2011

Environment:

    Kernel-mode Driver Framework

--*/

#ifndef _REGISTRY_H_
#define _REGISTRY_H_

NTSTATUS
_RegistryGetDeviceSettings(
    _In_  WDFDEVICE                     FxDevice,
    _Out_ PREGISTRY_DEVICE_SETTINGS     Settings);

#endif