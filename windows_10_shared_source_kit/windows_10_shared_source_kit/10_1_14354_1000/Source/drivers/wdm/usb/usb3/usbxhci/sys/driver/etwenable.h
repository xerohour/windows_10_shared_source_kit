//
//    Copyright (C) Microsoft.  All rights reserved.
//
#pragma once

__drv_sameIRQL
VOID
Etw_EnableCallback(
    __in
        LPCGUID                     SourceId,
    __in
        ULONG                       IsEnabled,
    __in
        UCHAR                       Level,
    __in
        ULONGLONG                   MatchAnyKeyword,
    __in
        ULONGLONG                   MatchAllKeyword,
    __in_opt
        PEVENT_FILTER_DESCRIPTOR    FilterData,
    __inout_opt
        PVOID                       CallbackContext
    );

#define MCGEN_PRIVATE_ENABLE_CALLBACK_V2    Etw_EnableCallback
