/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    registry.cpp

Abstract:

   Contains definitions for functions that retrieve settings
   from the registry

Author:

    Arvind Aiyar March 2011

Environment:

    Kernel-mode Driver Framework

--*/

#include "internal.h"

#include "registry.tmh"

NTSTATUS
_RegistryGetDeviceSettings(
    _In_  WDFDEVICE                     FxDevice,
    _Out_ PREGISTRY_DEVICE_SETTINGS     Settings)
/*++
 
  Routine Description:

    This routine retrieves device-specific settings
    from the registry.

  Arguments:

    FxDevice - a handle to the WDF device
    Settings - pointer to structure that stores the settings

  Return Value:

    NTSTATUS Status indicating success or failure

--*/
{
    NTSTATUS status;
    
    WDFKEY key = NULL;
    WDFKEY subkey = NULL;
    ULONG  compactSequences = 1;
    
    DECLARE_CONST_UNICODE_STRING(compactWriteWriteSequencesName, L"CompactWriteWriteSequences");
    
    NT_ASSERTMSG("Device object is NULL", FxDevice != NULL);
    NT_ASSERTMSG("Registry settings struct is NULL", Settings != NULL);

    status = WdfDeviceOpenRegistryKey(
                            FxDevice,
                            PLUGPLAY_REGKEY_DEVICE,
                            KEY_READ,
                            WDF_NO_OBJECT_ATTRIBUTES,
                            &key);

    if (!NT_SUCCESS(status))
    {
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_REGISTRY,
                "Failed opening device registry key "
                "status:%!STATUS!",
                status);
        goto exit;
    }

    status = WdfRegistryQueryULong(
                        key,
                        &compactWriteWriteSequencesName,
                        &compactSequences);

    if (!NT_SUCCESS(status))
    {
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_REGISTRY,
                "Failed querying registry value for CompactWriteWriteSequences "
                "status:%!STATUS!",
                status);
        goto exit;
    }

exit:
    if (!NT_SUCCESS(status))
    {
        TraceLoggingRegistry(
            status, 
            FxDevice);
    }

    //
    // Cache the values in the device context Registry Settings
    // 
    Settings->CompactWriteWriteSequences = (compactSequences) ? TRUE : FALSE;

    //
    // Close all opened keys
    //
    if (key != NULL)
    {
        WdfRegistryClose(key);
    }

    if (subkey != NULL)
    {   
        WdfRegistryClose(subkey);
    }

    return status;
}