/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    ioaccess.c

Abstract:

    This module contains ARM stubs for I/O port access.  ARM doe

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include <bootlibp.h>

UCHAR
BL_READ_PORT_UCHAR (
    __in PUCHAR Port
    )

/*++

Routine Description:

    Reads a byte from the specified I/O port.

Arguments:

    Port - Supplies the port address from which to read.

Return Value:

    The byte read from the specified port.

--*/

{
    UCHAR PortData;

    UNREFERENCED_PARAMETER(Port);

    ASSERT(FALSE);

    PortData = (UCHAR)-1;

    return PortData;
}

VOID
BL_WRITE_PORT_UCHAR (
    __in PUCHAR Port,
    __in UCHAR Value
    )

/*++

Routine Description:

    Writes a byte to the specified I/O port.

Arguments:

    Port - Supplies the port address to which to write.

    Value - The value to be written to the I/O port.

Return Value:

    None.

--*/

{
    UNREFERENCED_PARAMETER(Port);
    UNREFERENCED_PARAMETER(Value);

    ASSERT(FALSE);

    return;
}

USHORT
BL_READ_PORT_USHORT (
    __in PUSHORT Port
    )

/*++

Routine Description:

    Reads a word from the specified I/O port.

Arguments:

    Port - Supplies the port address from which to read.

Return Value:

    The word read from the specified port.

--*/

{

    USHORT PortData;

    UNREFERENCED_PARAMETER(Port);

    ASSERT(FALSE);

    PortData = (USHORT)-1;

    return PortData;
}

VOID
BL_WRITE_PORT_USHORT (
    __in PUSHORT Port,
    __in USHORT Value
    )

/*++

Routine Description:

    Writes a word to the specified I/O port.

Arguments:

    Port - Supplies the port address to which to write.

    Value - The value to be written to the I/O port.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Port);
    UNREFERENCED_PARAMETER(Value);

    ASSERT(FALSE);

    return;
}

ULONG
BL_READ_PORT_ULONG (
    __in PULONG Port
    )

/*++

Routine Description:

    Reads a dword from the specified I/O port.

Arguments:

    Port - Supplies the port address from which to read.

Return Value:

    The dword read from the specified port.

--*/

{

    ULONG PortData;

    UNREFERENCED_PARAMETER(Port);

    ASSERT(FALSE);

    PortData = (ULONG)-1;

    return PortData;
}

VOID
BL_WRITE_PORT_ULONG (
    __in PULONG Port,
    __in ULONG Value
    )

/*++

Routine Description:

    Writes a dword to the specified I/O port.

Arguments:

    Port - Supplies the port address to which to write.

    Value - The value to be written to the I/O port.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Port);
    UNREFERENCED_PARAMETER(Value);

    ASSERT(FALSE);

    return;
}
