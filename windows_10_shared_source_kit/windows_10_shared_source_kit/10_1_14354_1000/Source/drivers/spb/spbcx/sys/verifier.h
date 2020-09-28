/*++

Copyright (c) Microsoft Corporation

Module Name:

    verifier.h

Abstract:

    This module contains the private verifier function declarations
    for the SPB class extension.

Environment:

    kernel mode only

Revision History:


--*/

#ifndef _SCXVERIFIER_H_
#define _SCXVERIFIER_H_

//
// Verifier functions.
//

NTSTATUS
ScxVerifyNotNull(
    _In_ PVOID Parameter
    );

NTSTATUS
ScxVerifyIrqlLessThanOrEqual(
    _In_ KIRQL ExpectedIrql
    );

NTSTATUS
ScxVerifyIrqlEquals(
    _In_ KIRQL ExpectedIrql
    );

NTSTATUS
ScxVerifyPrivateGlobals(
    _In_ PSCX_CLIENT_GLOBALS  Globals
    );

#endif // _SCXVERIFIER_H_
