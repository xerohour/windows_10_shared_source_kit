/*++
Copyright (c) Microsoft Corporation

Module Name:

    parent.h

Abstract:

    IOCTLs and bus interface calls to the hub's parent device

--*/


#pragma once


VOID
HUBPARENT_GetHubSymbolicLinkName (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext,
    __in
        PUNICODE_STRING    HubSymbolicLinkName
    );
