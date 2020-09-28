/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    string.c

Abstract:

    Implements NLS and other Rtl string routines.  This avoids a dependency
    on ke.lib, but it is also required since the boot environment library
    need these routines before the NLS tables have been loaded.

Environment:

    boot

Revision History:

--*/

// ------------------------------------------------------------------- Includes

#include <bootlibp.h>

// ---------------------------------------------------------------- Definitions

//
// This is the maximum MaximumLength for a UNICODE_STRING.
//

#define MAX_USTRING ( sizeof(WCHAR) * (MAXUSHORT/sizeof(WCHAR)) )

// --------------------------------------------------------------------- Macros

#define UPCASE(_c) \
        (WCHAR)( ('a' <= (_c) && (_c) <= 'z') ? ((_c) - ('a' - 'A')) : (_c) )

// ------------------------------------------------------------------ Functions

NTSTATUS
BlConcatenateStringToString (
    __in PWSTR SourceString1,
    __in PWSTR SourceString2,
    __deref_out PWSTR *DestinationString
    )

/*++

Routine Description:

    This routine concatenates two wide strings together. It allocates a new
    destination string which the caller must free from the heap.

Arguments:

    SourceString1 - Supplies a string to be at the beginning of the
        concatenation.

    SourceString2 - Supplies a string to be at the end of the concatenation.

Return Value:

    NT status code.

--*/

{

    PWCHAR DestinationBuffer;
    SIZE_T DestinationSize;
    SIZE_T SourceString1Size;
    SIZE_T SourceString2Size;

    //
    // Determine the size of the destination string, adding space for the NULL
    // character.
    //

    SourceString1Size = wcslen(SourceString1) * sizeof(WCHAR);
    SourceString2Size = wcslen(SourceString2) * sizeof(WCHAR);
    DestinationSize = SourceString1Size + SourceString2Size + sizeof(WCHAR);

    //
    // Allocate a destination string and concatinate the strings together.
    //

    DestinationBuffer = BlMmAllocateHeap(DestinationSize);
    if (DestinationBuffer == NULL) {
        return STATUS_NO_MEMORY;
    }

    *DestinationString = DestinationBuffer;
    RtlCopyMemory(DestinationBuffer, SourceString1, SourceString1Size);
    DestinationBuffer = Add2Ptr(DestinationBuffer, SourceString1Size);
    RtlCopyMemory(DestinationBuffer, SourceString2, SourceString2Size);
    DestinationBuffer = Add2Ptr(DestinationBuffer, SourceString2Size);
    *DestinationBuffer = UNICODE_NULL;
    return STATUS_SUCCESS;
}

#if !defined(NTKERNEL) && !defined(WIN_APP)
WCHAR
RtlAnsiCharToUnicodeChar (
    __deref_inout PUCHAR *SourceCharacter
    )

/*++

Routine Description:

    This function translates the specified ansi character to unicode and
    returns the unicode value.  The purpose for this routine is to allow
    for character by character ansi to unicode translation.  The
    translation is done with respect to the current system locale
    information.


Arguments:

    SourceCharacter - Supplies a pointer to an ansi character pointer.
        Through two levels of indirection, this supplies an ansi
        character that is to be translated to unicode.  After
        translation, the ansi character pointer is modified to point to
        the next character to be converted.  This is done to allow for
        dbcs ansi characters.

Return Value:

    Returns the unicode equivalent of the specified ansi character.

--*/

{

    WCHAR UnicodeCharacter;

    BlpInterfaceEnter(InterfaceString);
    UnicodeCharacter = (WCHAR)**SourceCharacter;
    (*SourceCharacter)++;
    BlpInterfaceExit(InterfaceString);
    return UnicodeCharacter;
}

VOID
RtlInitUnicodeString (
    __out PUNICODE_STRING DestinationString,
    __in_opt PCWSTR SourceString
    )

/*++

Routine Description:

    The RtlInitUnicodeString function initializes an NT counted
    unicode string.  The DestinationString is initialized to point to
    the SourceString and the Length and MaximumLength fields of
    DestinationString are initialized to the length of the SourceString,
    which is zero if SourceString is not specified.

Arguments:

    DestinationString - Pointer to the counted string to initialize

    SourceString - Optional pointer to a null terminated unicode string that
        the counted string is to point to.


Return Value:

    None.

--*/

{

    ULONG Length;

    BlpInterfaceEnter(InterfaceString);
    DestinationString->Buffer = (PWSTR)SourceString;
    if (ARGUMENT_PRESENT(SourceString)) {
        Length = (ULONG)wcslen( SourceString ) * sizeof( WCHAR);

        ASSERT(Length < MAX_USTRING);

        if (Length >= MAX_USTRING) {
            Length = MAX_USTRING - sizeof(UNICODE_NULL);
        }

        DestinationString->Length = (USHORT)Length;
        DestinationString->MaximumLength = (USHORT)(Length + sizeof(UNICODE_NULL));

    } else {
        DestinationString->MaximumLength = 0;
        DestinationString->Length = 0;
    }

    BlpInterfaceExit(InterfaceString);
    return;
}

BOOLEAN
RtlEqualUnicodeString (
    __in PCUNICODE_STRING String1,
    __in PCUNICODE_STRING String2,
    __in BOOLEAN CaseInSensitive
    )

/*++

Routine Description:

    The RtlEqualUnicodeString function compares two counted unicode strings for
    equality.

    The CaseInSensitive parameter specifies if case is to be ignored when
    doing the comparison.

Arguments:

    String1 - Pointer to the first string.

    String2 - Pointer to the second string.

    CaseInsensitive - TRUE if case should be ignored when doing the
        comparison.

Return Value:

    Boolean value that is TRUE if String1 equals String2 and FALSE otherwise.

--*/

{

    PWCHAR s1, s2, Limit;
    LONG n1, n2;
    ULONG c1, c2;
    BOOLEAN ReturnValue;

    BlpInterfaceEnter(InterfaceString);
    n1 = String1->Length;
    n2 = String2->Length;
    if (n1 == n2) {
        s1 = String1->Buffer;
        s2 = String2->Buffer;
        Limit = (PWCHAR)((PCHAR)s1 + n1);
        ReturnValue = TRUE;
        if (CaseInSensitive) {
            while (s1 < Limit) {
                c1 = *s1;
                c2 = *s2;
                if ((c1 != c2) && (UPCASE(c1) != UPCASE(c2))) {
                    ReturnValue = FALSE;
                    break;
                }

                s1 += 1;
                s2 += 1;
            }

        } else {
            while (s1 < Limit) {
                c1 = *s1;
                c2 = *s2;
                if (c1 != c2) {
                    ReturnValue = FALSE;
                    break;
                }

                s1 += 1;
                s2 += 1;
            }
        }

    } else {
        ReturnValue = FALSE;
    }

    BlpInterfaceExit(InterfaceString);
    return ReturnValue;
}

VOID
RtlCopyUnicodeString(
    __inout PUNICODE_STRING DestinationString,
    __in_opt PCUNICODE_STRING SourceString
    )

/*++

Routine Description:

    The RtlCopyUnicodeString function copies the SourceString to the
    DestinationString.  If SourceString is not specified, then
    the Length field of DestinationString is set to zero.  The
    MaximumLength and Buffer fields of DestinationString are not
    modified by this function.

    The number of bytes copied from the SourceString is either the
    Length of SourceString or the MaximumLength of DestinationString,
    whichever is smaller.

    Copied verbatim from minkernel\ntos\rtl\nls.c but with BlpInterface* added.

Arguments:

    DestinationString - Pointer to the destination string.

    SourceString - Optional pointer to the source string.

Return Value:

    None.

--*/

{
    UNALIGNED WCHAR *src, *dst;
    ULONG n;

    BlpInterfaceEnter(InterfaceString);

    if (ARGUMENT_PRESENT(SourceString)) {
        dst = DestinationString->Buffer;
        src = SourceString->Buffer;
        n = SourceString->Length;
        if ((USHORT)n > DestinationString->MaximumLength) {
            n = DestinationString->MaximumLength;
        }

        DestinationString->Length = (USHORT)n;
        RtlCopyMemory(dst, src, n);
        if( (DestinationString->Length + sizeof (WCHAR)) <= DestinationString->MaximumLength) {
            dst[n / sizeof(WCHAR)] = UNICODE_NULL;
        }

    } else {
        DestinationString->Length = 0;
    }

    BlpInterfaceExit(InterfaceString);
    return;
}

#endif !defined(NTKERNEL)


NTSTATUS
RtlAnsiStringToUnicodeString (
    __inout PUNICODE_STRING DestinationString,
    __in PCANSI_STRING SourceString,
    __in BOOLEAN AllocateDestinationString
    )

/*++

Routine Description:

    This functions converts the specified ansi source string into a
    Unicode string. The translation is done with respect to the
    current system locale information.

Arguments:

    DestinationString - Returns a unicode string that is equivalent to
        the ansi source string. The maximum length field is only
        set if AllocateDestinationString is TRUE.

    SourceString - Supplies the ansi source string that is to be
        converted to unicode.

    AllocateDestinationString - Supplies a flag that controls whether or
        not this API allocates the buffer space for the destination
        string.  If it does, then the buffer must be deallocated using
        RtlFreeUnicodeString (note that only storage for
        DestinationString->Buffer is allocated by this API).

Return Value:

    SUCCESS - The conversion was successful

    !SUCCESS - The operation failed.  No storage was allocated and no
        conversion was done.  None.

--*/

{

    ULONG UnicodeLength;
    ULONG Index;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceString);
    UnicodeLength = (SourceString->Length * sizeof(WCHAR)) +
                    sizeof(UNICODE_NULL);

    if (UnicodeLength > MAX_USTRING) {
        Status = STATUS_INVALID_PARAMETER_2;
        goto RtlAnsiStringToUnicodeStringEnd;
    }

    if (AllocateDestinationString != FALSE) {
        DestinationString->MaximumLength = (USHORT)UnicodeLength;
        DestinationString->Buffer = (RtlAllocateStringRoutine)(UnicodeLength);
        if (DestinationString->Buffer == NULL) {
            Status = STATUS_NO_MEMORY;
            goto RtlAnsiStringToUnicodeStringEnd;
        }

    } else {
        if (UnicodeLength > DestinationString->MaximumLength) {
            Status = STATUS_BUFFER_OVERFLOW;
            goto RtlAnsiStringToUnicodeStringEnd;
        }
    }

    DestinationString->Length = (USHORT)(UnicodeLength - sizeof(UNICODE_NULL));
    Index = 0;
    while(Index < SourceString->Length ) {
        DestinationString->Buffer[Index] = (WCHAR)SourceString->Buffer[Index];
        Index++;
    }

    if (DestinationString->Length < DestinationString->MaximumLength) {
        DestinationString->Buffer[Index] = UNICODE_NULL;
    }

    Status = STATUS_SUCCESS;

RtlAnsiStringToUnicodeStringEnd:
   BlpInterfaceExit(InterfaceString);
   return Status;
}

#if !defined(NTKERNEL)
_At_(Destination->MaximumLength, _Const_)
_At_(Destination->Buffer, _Const_)
NTSTATUS
RtlAppendUnicodeStringToString (
    __inout PUNICODE_STRING Destination,
    __in PCUNICODE_STRING Source
    )

/*++

Routine Description:

    This routine will concatinate two PSTRINGs together.  It will copy
    bytes from the source up to the MaximumLength of the destination.

Arguments:

    __in PSTRING Destination, - Supplies the destination string
    __in PSTRING Source - Supplies the source for the string copy

Return Value:

    STATUS_SUCCESS - The source string was successfully appended to the
        destination counted string.

    STATUS_BUFFER_TOO_SMALL - The destination string length was not big
        enough to allow the source string to be appended.  The Destination
        string length is not updated.

--*/

{

    USHORT n;
    WCHAR UNALIGNED *dst;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceString);
    Status = STATUS_SUCCESS;
    n = Source->Length;
    if (n) {
        if ((n + Destination->Length) > Destination->MaximumLength) {
            Status = STATUS_BUFFER_TOO_SMALL;

        } else {
            dst = &Destination->Buffer[(Destination->Length / sizeof(WCHAR))];
            RtlMoveMemory(dst, Source->Buffer, n);
            Destination->Length = Destination->Length + n;
            if (Destination->Length < Destination->MaximumLength) {
                dst[n / sizeof(WCHAR)] = UNICODE_NULL;
            }
        }
    }

    BlpInterfaceExit(InterfaceString);
    return Status;
}

_At_(Destination->MaximumLength, _Const_)
_At_(Destination->Buffer, _Const_)
NTSTATUS
RtlAppendUnicodeToString (
    __in PUNICODE_STRING Destination,
    __in_opt PCWSTR Source
    )

/*++

Routine Description:

    This routine appends the supplied UNICODE string to an existing
    PUNICODE_STRING.

    It will copy bytes from the Source PSZ to the destination PSTRING up to
    the destinations PUNICODE_STRING->MaximumLength field.

Arguments:

    __in PUNICODE_STRING Destination, - Supplies a pointer to the destination
                            string
    __in PWSTR Source - Supplies the string to append to the destination

Return Value:

    STATUS_SUCCESS - The source string was successfully appended to the
        destination counted string.

    STATUS_BUFFER_TOO_SMALL - The destination string length was not big
        enough to allow the source string to be appended.  The Destination
        string length is not updated.

--*/

{

    USHORT n;
    WCHAR UNALIGNED *dst;
    NTSTATUS Status;
    UNICODE_STRING UniSource;

    BlpInterfaceEnter(InterfaceString);
    if (ARGUMENT_PRESENT( Source )) {
        if(!NT_SUCCESS(RtlInitUnicodeStringEx(&UniSource, Source))) {
            Status = STATUS_BUFFER_TOO_SMALL;
            goto RtlAppendUnicodeToStringEnd;
        }

        n = UniSource.Length;
        if ((n + Destination->Length) > Destination->MaximumLength) {
            Status = STATUS_BUFFER_TOO_SMALL;
            goto RtlAppendUnicodeToStringEnd;
        }

        dst = &Destination->Buffer[(Destination->Length / sizeof(WCHAR))];
        RtlMoveMemory(dst, Source, n);
        Destination->Length = Destination->Length + n;
        if (Destination->Length < Destination->MaximumLength) {
            dst[n / sizeof(WCHAR)] = UNICODE_NULL;
        }
    }

    Status = STATUS_SUCCESS;

RtlAppendUnicodeToStringEnd:

    BlpInterfaceExit(InterfaceString);
    return Status;
}

LONG
RtlCompareUnicodeStrings (
    __in_ecount(String1Length) PCWCH String1,
    __in SIZE_T String1Length,
    __in_ecount(String2Length) PCWCH String2,
    __in SIZE_T String2Length,
    __in BOOLEAN CaseInSensitive
    )

/*++

Routine Description:

    The RtlCompareUnicodeStrings function compares two counted strings.  The
    return value indicates if the strings are equal or String1 is less than
    String2 or String1 is greater than String2.

    The CaseInSensitive parameter specifies if case is to be ignored when
    doing the comparison.

Arguments:

    String1 - Pointer to the first string.

    String1Length - Supplies the length, in characters, of the first string.

    String2 - Pointer to the second string.

    String2Length - Supplies the length, in characters, of the second string.

    CaseInsensitive - TRUE if case should be ignored when doing the
        comparison.

Return Value:

    Signed value that gives the results of the comparison:

        Zero - String1 equals String2

        < Zero - String1 less than String2

        > Zero - String1 greater than String2


--*/

{

    PCWSTR s1, s2, Limit;
    SIZE_T n1, n2;
    WCHAR c1, c2;
    LONG ReturnValue;

    BlpInterfaceEnter(InterfaceString);
    s1 = String1;
    s2 = String2;
    n1 = String1Length;
    n2 = String2Length;

    ASSERT(!(((((ULONG_PTR)s1 & 1) != 0) || (((ULONG_PTR)s2 & 1) != 0))));

    Limit = s1 + (n1 <= n2 ? n1 : n2);
    ReturnValue = (LONG)(n1 - n2);
    if (CaseInSensitive) {
        while (s1 < Limit) {
            c1 = *s1++;
            c2 = *s2++;
            if (c1 != c2) {

                //
                // Note that this needs to reference the translation table!
                //

                c1 = UPCASE(c1);
                c2 = UPCASE(c2);
                if (c1 != c2) {
                    ReturnValue = (LONG)(c1) - (LONG)(c2);
                    break;
                }
            }
        }

    } else {
        while (s1 < Limit) {
            c1 = *s1++;
            c2 = *s2++;
            if (c1 != c2) {
                ReturnValue = (LONG)(c1) - (LONG)(c2);
                break;
            }
        }
    }

    BlpInterfaceExit(InterfaceString);
    return ReturnValue;
}

LONG
RtlCompareUnicodeString (
    __in PCUNICODE_STRING String1,
    __in PCUNICODE_STRING String2,
    __in BOOLEAN CaseInSensitive
    )

/*++

Routine Description:

    The RtlCompareUnicodeString function compares two counted strings.  The
    return value indicates if the strings are equal or String1 is less than
    String2 or String1 is greater than String2.

    The CaseInSensitive parameter specifies if case is to be ignored when
    doing the comparison.

Arguments:

    String1 - Pointer to the first string.

    String2 - Pointer to the second string.

    CaseInsensitive - TRUE if case should be ignored when doing the
        comparison.

Return Value:

    Signed value that gives the results of the comparison:

        Zero - String1 equals String2

        < Zero - String1 less than String2

        > Zero - String1 greater than String2


--*/

{
    return RtlCompareUnicodeStrings(String1->Buffer,
                                    String1->Length / sizeof(WCHAR),
                                    String2->Buffer,
                                    String2->Length / sizeof(WCHAR),
                                    CaseInSensitive);
}

BOOLEAN
RtlPrefixUnicodeString (
    __in PCUNICODE_STRING String1,
    __in PCUNICODE_STRING String2,
    __in BOOLEAN CaseInSensitive
    )

/*++

Routine Description:

    The RtlPrefixUnicodeString function determines if the String1
    counted string parameter is a prefix of the String2 counted string
    parameter.

    The CaseInSensitive parameter specifies if case is to be ignored when
    doing the comparison.

Arguments:

    String1 - Pointer to the first unicode string.

    String2 - Pointer to the second unicode string.

    CaseInsensitive - TRUE if case should be ignored when doing the
        comparison.

Return Value:

    Boolean value that is TRUE if String1 equals a prefix of String2 and
    FALSE otherwise.

--*/

{
    PWSTR Limit;
    PWSTR s1, s2;
    ULONG n;
    ULONG c1, c2;
    BOOLEAN Result;

    BlpInterfaceEnter(InterfaceString);

    s1 = String1->Buffer;
    s2 = String2->Buffer;
    n = String1->Length;
    if (String2->Length < n) {
        Result = FALSE;
        goto RtlPrefixUnicodeStringEnd;
    }

    Limit = (PWSTR)((PCHAR)s1 + n);
    if (CaseInSensitive) {
        while (s1 < Limit) {
            c1 = *s1;
            c2 = *s2;
            if ((c1 != c2) && (UPCASE(c1) != UPCASE(c2))) {
                Result = FALSE;
                goto RtlPrefixUnicodeStringEnd;
            }

            s1 += 1;
            s2 += 1;
        }

    } else {
        while (s1 < Limit) {
            if (*s1 != *s2) {
                Result = FALSE;
                goto RtlPrefixUnicodeStringEnd;
            }

            s1 += 1;
            s2 += 1;
        }
    }

    Result = TRUE;

RtlPrefixUnicodeStringEnd:

    BlpInterfaceExit(InterfaceString);

    return Result;
}
#endif !defined(NTKERNEL)

#include <ntstrsafe.h>

#include "rtlvalidateunicodestring.c"

NTSTATUS
RtlDuplicateUnicodeString(
    __in ULONG Flags,
    __in PCUNICODE_STRING StringIn,
    __out __drv_at(StringOut->Buffer, __drv_allocatesMem(Mem))
        PUNICODE_STRING StringOut
    )
{
    NTSTATUS Status = STATUS_SUCCESS;
    USHORT Length = 0;
    USHORT NewMaximumLength = 0;
    PWSTR Buffer = NULL;

    if (((Flags & ~(
              RTL_DUPLICATE_UNICODE_STRING_NULL_TERMINATE |
              RTL_DUPLICATE_UNICODE_STRING_ALLOCATE_NULL_STRING)) != 0) ||
        (StringOut == NULL)) {

        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    // It doesn't make sense to force allocation of a null string unless you
    // want null termination.
    if ((Flags & RTL_DUPLICATE_UNICODE_STRING_ALLOCATE_NULL_STRING) &&
        !(Flags & RTL_DUPLICATE_UNICODE_STRING_NULL_TERMINATE)) {

        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    Status = RtlValidateUnicodeString(0, StringIn);
    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    if (StringIn != NULL) {
        Length = StringIn->Length;
    }

    if ((Flags & RTL_DUPLICATE_UNICODE_STRING_NULL_TERMINATE) &&
        (Length == UNICODE_STRING_MAX_BYTES)) {

        Status = STATUS_NAME_TOO_LONG;
        goto Cleanup;
    }

    if (Flags & RTL_DUPLICATE_UNICODE_STRING_NULL_TERMINATE) {
        NewMaximumLength = (USHORT) (Length + sizeof(WCHAR));

    } else {
        NewMaximumLength = Length;
    }

    // If it's a zero length string in, force the allocation length to zero
    // unless the caller said that they want zero length strings allocated.
    if (((Flags & RTL_DUPLICATE_UNICODE_STRING_ALLOCATE_NULL_STRING) == 0) &&
        (Length == 0)) {

        NewMaximumLength = 0;
    }

    if (NewMaximumLength != 0) {
        Buffer = (RtlAllocateStringRoutine)(NewMaximumLength);
        if (Buffer == NULL) {
            Status = STATUS_NO_MEMORY;
            goto Cleanup;
        }

        // If there's anything to copy, copy it.  We explicitly test Length because
        // StringIn could be a NULL pointer, so dereferencing it to get the Buffer
        // pointer would access violate.
        if (Length != 0) {
            RtlCopyMemory(
                Buffer,
                StringIn->Buffer,
                Length);

        }

        if (Flags & RTL_DUPLICATE_UNICODE_STRING_NULL_TERMINATE) {
            Buffer[Length / sizeof(WCHAR)] = L'\0';
        }
    }

    StringOut->MaximumLength = NewMaximumLength;
    StringOut->Length = Length;
    StringOut->Buffer = Buffer;
    Status = STATUS_SUCCESS;

Cleanup:
    if (!NT_SUCCESS(Status)) {
        if (Buffer != NULL) {
            (RtlFreeStringRoutine)(Buffer);
        }
    }

    return Status;
}

_When_(AllocateDestinationString,
       _At_(DestinationString->MaximumLength,
            __out_range(<=, (SourceString->MaximumLength / sizeof(WCHAR)))))
_When_(!AllocateDestinationString,
       _At_(DestinationString->Buffer, _Const_)
       _At_(DestinationString->MaximumLength, _Const_))
NTSTATUS
RtlUnicodeStringToAnsiString (
    __inout PANSI_STRING DestinationString,
    __in PCUNICODE_STRING SourceString,
    __in BOOLEAN AllocateDestinationString
    )

/*++

Routine Description:

    This functions converts the specified unicode source string into a
    ansi string. The translation is done WITHOUT respect to the
    current system locale information.

    TODO: This function can probably be removed when arc path usage is removed.

Arguments:

    DestinationString - Returns a ansi string that is equivalent to
        the unicode source string. The maximum length field is only
        set if AllocateDestinationString is TRUE.

    SourceString - Supplies the unicode source string that is to be
        converted to ansi.

    AllocateDestinationString - Supplies a flag that controls whether or
        not this API allocates the buffer space for the destination
        string.  If it does, then the buffer must be deallocated using
        RtlFreeStringRoutine (note that only storage for
        DestinationString->Buffer is allocated by this API).

Return Value:

    SUCCESS - The conversion was successful

    !SUCCESS - The operation failed.  No storage was allocated and no
        conversion was done.  None.

--*/

{

    ULONG Length;
    ULONG Index;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceString);
    Length = (SourceString->Length / sizeof(WCHAR)) + 1;
    if (Length > MAXUSHORT) {
        Status = STATUS_INVALID_PARAMETER_2;
        goto RtlUnicodeStringToAnsiStringEnd;
    }

    DestinationString->Length = (USHORT)(Length - 1);
    if (AllocateDestinationString) {
        DestinationString->MaximumLength = (USHORT)Length;
        DestinationString->Buffer = (RtlAllocateStringRoutine)(Length);
        if (!DestinationString->Buffer) {
            Status = STATUS_NO_MEMORY;
            goto RtlUnicodeStringToAnsiStringEnd;
        }

    } else {
        if (DestinationString->Length >= DestinationString->MaximumLength) {
            Status = STATUS_BUFFER_OVERFLOW;
            goto RtlUnicodeStringToAnsiStringEnd;
        }
    }

    Index = 0;
    while(Index < DestinationString->Length ) {
        DestinationString->Buffer[Index] = (CHAR)SourceString->Buffer[Index];
        Index++;
    }

    if (DestinationString->Length < DestinationString->MaximumLength) {
        DestinationString->Buffer[Index] = 0;
    }

    Status = STATUS_SUCCESS;

RtlUnicodeStringToAnsiStringEnd:
    BlpInterfaceExit(InterfaceString);
    return Status;
}

#if !defined(NTKERNEL)
WCHAR
RtlUpcaseUnicodeChar (
    __in WCHAR SourceCharacter
    )

/*++

Routine Description:

    This function translates the specified unicode character to its
    equivalent upcased unicode chararacter.  The purpose for this routine
    is to allow for character by character upcase translation.  The
    translation is done with respect to the current system locale
    information.


Arguments:

    SourceCharacter - Supplies the unicode character to be upcased.

Return Value:

    Returns the upcased unicode equivalent of the specified input character.

--*/

{

    WCHAR UnicodeCharacter;

    //
    // Note that this needs to reference the translation table !
    //

    BlpInterfaceEnter(InterfaceString);
    UnicodeCharacter = (WCHAR)UPCASE(SourceCharacter);
    BlpInterfaceExit(InterfaceString);
    return UnicodeCharacter;
}

ULONG
RtlxAnsiStringToUnicodeSize (
    __in PCANSI_STRING AnsiString
    )

/*++

Routine Description:

    This function computes the number of bytes required to store a NULL
    terminated unicode string that is equivalent to the specified ansi
    string.

Arguments:

    AnsiString - Supplies an ansi string whose equivalent size as a
        unicode string is to be calculated.  The ansi string is
        interpreted relative to the current system locale.

Return Value:

    The return value specifies the number of bytes required to hold a
    NULL terminated unicode string equivalent to the specified ansi
    string.

--*/

{

    ULONG Size;

    BlpInterfaceEnter(InterfaceString);
    Size = (AnsiString->Length * sizeof(WCHAR)) + sizeof(UNICODE_NULL);
    BlpInterfaceExit(InterfaceString);
    return Size;
}

VOID
RtlInitAnsiString (
    __out PANSI_STRING DestinationString,
    __in_opt PCSZ SourceString
    )

/*++

Routine Description:

    The RtlInitAnsiString function initializes an NT counted string.
    The DestinationString is initialized to point to the SourceString
    and the Length and MaximumLength fields of DestinationString are
    initialized to the length of the SourceString, which is zero if
    SourceString is not specified.

Arguments:

    DestinationString - Pointer to the counted string to initialize

    SourceString - Optional pointer to a null terminated string that
        the counted string is to point to.


Return Value:

    None.

--*/

{

    ULONG Length;

    BlpInterfaceEnter(InterfaceString);
    DestinationString->Buffer = (PCHAR)SourceString;
    if (ARGUMENT_PRESENT( SourceString )) {
        Length = (ULONG)strlen(SourceString);

        ASSERT( Length < MAXUSHORT);

        if (Length >= MAXUSHORT) {
            Length = MAXUSHORT - 1;
        }

        DestinationString->Length = (USHORT)Length;
        DestinationString->MaximumLength = (USHORT)(Length+1);

    } else {
        DestinationString->Length = 0;
        DestinationString->MaximumLength = 0;
    }

    BlpInterfaceExit(InterfaceString);
    return;
}

NTSTATUS
RtlInitUnicodeStringEx (
    __out PUNICODE_STRING DestinationString,
    __in_opt PCWSTR SourceString
    )

{

    SIZE_T Length;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceString);
    if (SourceString != NULL) {
        Length = wcslen(SourceString);

        //
        // We are actually limited to 32765 characters since we want to store
        // a meaningful MaximumLength also.
        //

        if (Length > (UNICODE_STRING_MAX_CHARS - 1)) {
            Status = STATUS_NAME_TOO_LONG;

        } else {
            Length *= sizeof(WCHAR);
            DestinationString->Length = (USHORT) Length;
            DestinationString->MaximumLength = (USHORT)(Length + sizeof(WCHAR));
            DestinationString->Buffer = (PWSTR) SourceString;
            Status = STATUS_SUCCESS;
        }

    } else {
        DestinationString->Length = 0;
        DestinationString->MaximumLength = 0;
        DestinationString->Buffer = NULL;
        Status = STATUS_SUCCESS;
    }

    BlpInterfaceExit(InterfaceString);
    return Status;
}
#endif !defined(NTKERNEL)

BOOLEAN
RtlCreateUnicodeString (
    __out PUNICODE_STRING DestinationString,
    __in PCWSTR SourceString
    )

{

    ULONG cb;
    BOOLEAN ReturnValue;

    BlpInterfaceEnter(InterfaceString);
    cb = (ULONG)(wcslen( SourceString ) + 1) * sizeof( WCHAR);
    if(cb > MAX_USTRING) {
        ReturnValue = FALSE;

    } else {
        DestinationString->Buffer = (RtlAllocateStringRoutine)( cb);
        if (DestinationString->Buffer) {
            RtlCopyMemory( DestinationString->Buffer, SourceString, cb);
            DestinationString->MaximumLength = (USHORT)cb;
            DestinationString->Length = (USHORT)(cb - sizeof( UNICODE_NULL ));
            ReturnValue = TRUE;

        } else {
            ReturnValue = FALSE;
        }
    }

    BlpInterfaceExit(InterfaceString);
    return ReturnValue;
}

#if !defined(NTKERNEL)
NTSTATUS
RtlUnicodeToMultiByteN(
    __out_bcount_part(MaxBytesInMultiByteString, *BytesInMultiByteString) PCHAR MultiByteString,
    __in ULONG MaxBytesInMultiByteString,
    __out_opt PULONG BytesInMultiByteString,
    __in_bcount(BytesInUnicodeString) PCWCH UnicodeString,
    __in ULONG BytesInUnicodeString
    )

{

    UNREFERENCED_PARAMETER(MultiByteString);
    UNREFERENCED_PARAMETER(MaxBytesInMultiByteString);
    UNREFERENCED_PARAMETER(BytesInMultiByteString);
    UNREFERENCED_PARAMETER(UnicodeString);
    UNREFERENCED_PARAMETER(BytesInUnicodeString);

    return STATUS_NOT_SUPPORTED;
}
#endif !defined(NTKERNEL)

BOOLEAN
BlCreateAnsiString (
    __out PANSI_STRING DestinationString,
    __in PCSTR SourceString
    )

/*++

Routine Description:

    Ansi string equivalent to RtlCreateUnicodeString.

Arguments:

    DestinationString - ANSI_STRING stucture to initialize.

    SourceString - Ansi string.

Return Value:

    TRUE when successful.
    FALSE otherwise.

--*/

{

    ULONG cb;
    BOOLEAN ReturnValue;

    BlpInterfaceEnter(InterfaceString);
    cb = (ULONG)(strlen(SourceString) + 1);
    if (cb > MAX_USTRING) {
        ReturnValue = FALSE;

    } else {
        DestinationString->Buffer = (RtlAllocateStringRoutine)(cb);
        if (DestinationString->Buffer) {
            RtlCopyMemory( DestinationString->Buffer, SourceString, cb);
            DestinationString->MaximumLength = (USHORT)cb;
            DestinationString->Length = (USHORT)(cb - 1);
            ReturnValue = TRUE;

        } else {
            ReturnValue = FALSE;
        }
    }

    BlpInterfaceExit(InterfaceString);
    return ReturnValue;
}

VOID
RtlFreeAnsiString (
    _Inout_ PANSI_STRING AnsiString
    )

/*++

Routine Description:

    This API is used to free storage allocated by
    RtlUnicodeStringToAnsiString.  Note that only AnsiString->Buffer
    is free'd by this routine.

Arguments:

    AnsiString - Supplies the address of the ansi string whose buffer
        was previously allocated by RtlUnicodeStringToAnsiString.

Return Value:

    None.

--*/

{

    BlpInterfaceEnter(InterfaceString);
    if (AnsiString->Buffer) {
       (RtlFreeStringRoutine)(AnsiString->Buffer);
        RtlZeroMemory(AnsiString, sizeof(*AnsiString));
    }

    BlpInterfaceExit(InterfaceString);
    return;
}

VOID
RtlFreeUnicodeString (
    _Inout_ PUNICODE_STRING UnicodeString
    )

/*++

Routine Description:

    This API is used to free storage allocated by
    RtlAnsiStringToUnicodeString.  Note that only UnicodeString->Buffer
    is free'd by this routine.

Arguments:

    UnicodeString - Supplies the address of the unicode string whose
        buffer was previously allocated by RtlAnsiStringToUnicodeString.

Return Value:

    None.

--*/

{

    BlpInterfaceEnter(InterfaceString);
    if (UnicodeString->Buffer) {
       (RtlFreeStringRoutine)(UnicodeString->Buffer);
        RtlZeroMemory(UnicodeString, sizeof(*UnicodeString));
    }

    BlpInterfaceExit(InterfaceString);
    return;
}

VOID
BlCopyStringToWcharString (
    __out PWSTR Dest,
    __in PSTR Source
    )

/*++

Routine Description:

    Copies an ascii string to a unicode string.

Arguments:

    Dest - Destination buffer (WCHAR).

    Source - Source buffer (CHAR).

Return Value:

    None.

--*/

{

    ANSI_STRING AsciiString;
    NTSTATUS Status;
    UNICODE_STRING UnicodeString;

    BlpInterfaceEnter(InterfaceString);
    RtlInitAnsiString(&AsciiString, Source);
    UnicodeString.Length = 0;
    UnicodeString.MaximumLength = AsciiString.MaximumLength * 2;
    UnicodeString.Buffer = Dest;
#pragma prefast(suppress:__WARNING_INCORRECT_ANNOTATION_STRING, "Dest follows unsafe contract.");
    Status = RtlAnsiStringToUnicodeString(&UnicodeString, &AsciiString, FALSE);

    ASSERT(Status == STATUS_SUCCESS);

    BlpInterfaceExit(InterfaceString);
    return;
}

VOID
BlCopyWcharStringToString (
    __out PSTR Dest,
    __in PWSTR Source
    )

/*++

Routine Description:

    Copies an wide character string to a single character string.
    TODO: This function can probably be removed when arc path usage is removed.

Arguments:

    Dest - Destination buffer (CHAR).

    Source - Source buffer (WCHAR).

Return Value:

    None.

--*/

{

    ANSI_STRING AsciiString;
    NTSTATUS Status;
    UNICODE_STRING UnicodeString;

    __assume_validated(Dest);

    BlpInterfaceEnter(InterfaceString);
    RtlInitUnicodeString(&UnicodeString, Source);
    AsciiString.Length = UnicodeString.Length / 2;
    AsciiString.MaximumLength = UnicodeString.MaximumLength / 2;
    AsciiString.Buffer = Dest;
#pragma prefast(suppress:__WARNING_INCORRECT_ANNOTATION_STRING, "Dest follows unsafe contract.");
    Status = RtlUnicodeStringToAnsiString(&AsciiString, &UnicodeString, FALSE);

    ASSERT(Status == STATUS_SUCCESS);

    BlpInterfaceExit(InterfaceString);
    return;
}

NTSTATUS
BlCopyStringToUnicodeString (
    __out PUNICODE_STRING UnicodeString,
    __in PCSTR String
    )

/*++

Routine Description:

    Copies an ascii string to a unicode string.  If the destination buffer
    is too small for the requested copy, this routine will allocate a new
    buffer.

Arguments:

    UnicodeString - Destination string.

    String - Ascii string to copy.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY when there is not enough memory for the allocation.

--*/

{

    ANSI_STRING AsciiString;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceString);
    RtlInitAnsiString(&AsciiString, String);

    //
    // Attempt to copy string without allocation.
    //

    Status = RtlAnsiStringToUnicodeString(UnicodeString, &AsciiString, FALSE);
    if (Status == STATUS_BUFFER_OVERFLOW) {

        //
        // Grow buffer.
        //

        if (UnicodeString->MaximumLength != 0) {
            RtlFreeStringRoutine(UnicodeString->Buffer);
        }

        Status = RtlAnsiStringToUnicodeString(UnicodeString,
                                              &AsciiString,
                                              TRUE);
    }

    BlpInterfaceExit(InterfaceString);
    return Status;
}

NTSTATUS
BlCopyUnicodeStringToUnicodeString (
    __inout PUNICODE_STRING Destination,
    __inout PCUNICODE_STRING Source
    )

/*++

Routine Description:

    This routine copies a unicode string to another unicode string.  If the
    destination buffer is too small for the requested copy, this routine will
    allocate a new buffer.

Arguments:

    Destination - Supplies the destination string.

    Source - Supplies the source string.

Return Value:

    NTSTATUS.

--*/

{
    PWCHAR NewBuffer;
    ULONG RequiredSize;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceString);

    //
    // Calculate the required size and allocate more memory if needed.
    //

    RequiredSize = Source->Length + sizeof(UNICODE_NULL);
    if (RequiredSize > MAX_USTRING) {
        Status = STATUS_INVALID_PARAMETER_1;
        goto BlCopyUnicodeStringToUnicodeStringEnd;
    }

    if (RequiredSize > Destination->MaximumLength) {
        NewBuffer = RtlAllocateStringRoutine(RequiredSize);
        if (NewBuffer == NULL) {
            Status = STATUS_NO_MEMORY;
            goto BlCopyUnicodeStringToUnicodeStringEnd;
        }

        if (Destination->MaximumLength != 0) {
            RtlFreeStringRoutine(Destination->Buffer);
        }

        Destination->Buffer = NewBuffer;
        Destination->MaximumLength = (USHORT)RequiredSize;
    }

    //
    // Copy the string, null terminate it, and update the length.
    //

    RtlCopyMemory(Destination->Buffer, Source->Buffer, Source->Length);

    Destination->Buffer[Source->Length / sizeof(WCHAR)] = UNICODE_NULL;
    Destination->Length = Source->Length;

    Status = STATUS_SUCCESS;

BlCopyUnicodeStringToUnicodeStringEnd:

    BlpInterfaceExit(InterfaceString);
    return Status;
}

_Success_(return != 0)
BOOLEAN
BlAppendUnicodeToString (
    _At_((PWSTR)(UnicodeString->Buffer), _Inout_ _Post_z_) PUNICODE_STRING UnicodeString,
    _In_ PCWSTR String
    )

/*++

Routine Description:

    Wrapper for RtlAppendUnicodeToString.  If the current buffer is too
    small for append, this routine will allocate a larger buffer.

Arguments:

    UnicodeString - Unicode string.

    String - String to append.

Return Value:

    TRUE when successful.
    FALSE if operation fails (allocation fails).

--*/

{

    UINTN Size;
    NTSTATUS Status;
    PWCHAR p;
    BOOLEAN ReturnValue;

    BlpInterfaceEnter(InterfaceString);

    //
    // If the destination buffer is empty, allocate a buffer that can hold the
    // entire source string.
    //

    if (UnicodeString->MaximumLength == 0) {
        Size = (wcslen(String) + 1) * sizeof(WCHAR);
        UnicodeString->Buffer = BlMmAllocateHeap(Size);
        if (UnicodeString->Buffer == NULL) {
            ReturnValue = FALSE;
            goto BlAppendUnicodeToStringEnd;
        }

        UnicodeString->MaximumLength = (USHORT)Size;
    }

    ASSERT(UnicodeString->MaximumLength != UnicodeString->Length);

    //
    // Guarantee that all calls to this routine will result in a zero
    // terminated unicode string.  Do this by lying to RtlAppendUnicodeToString
    // about the size of Buffer.
    //

    UnicodeString->MaximumLength -= sizeof(UNICODE_NULL);
    Status = RtlAppendUnicodeToString(UnicodeString, String);
    UnicodeString->MaximumLength += sizeof(UNICODE_NULL);
    while (Status == STATUS_BUFFER_TOO_SMALL) {

        //
        // Double the size of the buffer.
        //

        Status = RtlUShortMult(UnicodeString->MaximumLength,
                               2,
                               &(UnicodeString->MaximumLength));
        if (!NT_SUCCESS(Status)) {
            ASSERT (FALSE);
            ReturnValue = FALSE;
            goto BlAppendUnicodeToStringEnd;
        }

        p = BlMmAllocateHeap(UnicodeString->MaximumLength);
        if (p == NULL) {
            ReturnValue = FALSE;
            goto BlAppendUnicodeToStringEnd;
        }

        RtlCopyMemory(p, UnicodeString->Buffer, UnicodeString->Length);
        p[UnicodeString->Length] = UNICODE_NULL;
        BlMmFreeHeap(UnicodeString->Buffer);
        UnicodeString->Buffer = p;

        //
        // Try to append the string again.
        //

        UnicodeString->MaximumLength -= sizeof(UNICODE_NULL);
        Status = RtlAppendUnicodeToString(UnicodeString, String);
        UnicodeString->MaximumLength += sizeof(UNICODE_NULL);
    }

    ASSERT(UnicodeString->Length <=
           (UnicodeString->MaximumLength - sizeof(UNICODE_NULL)));

    if (UnicodeString->Length <=
        (UnicodeString->MaximumLength - sizeof(UNICODE_NULL))) {

        UnicodeString->Buffer[UnicodeString->Length / sizeof(WCHAR)] = UNICODE_NULL;
    }

    ReturnValue = TRUE;

BlAppendUnicodeToStringEnd:
    BlpInterfaceExit(InterfaceString);
    return ReturnValue;
}

#if !defined(NTKERNEL)

extern
FORCEINLINE
NTSTATUS
BlDuplicateUnicodeString(
    __in ULONG Flags,
    __in PCUNICODE_STRING StringIn,
    __out __drv_at(StringOut->Buffer, __drv_allocatesMem(Mem))
        PUNICODE_STRING StringOut
    )
{

    return RtlDuplicateUnicodeString(Flags, StringIn, StringOut);
}

extern
FORCEINLINE
NTSTATUS
BlAnsiStringToUnicodeString (
    __inout PUNICODE_STRING DestinationString,
    __in PCANSI_STRING SourceString,
    __in BOOLEAN AllocateDestinationString
    )
{

    return RtlAnsiStringToUnicodeString(DestinationString,
                                        SourceString,
                                        AllocateDestinationString);
}

#endif //!defined(NTKERNEL)

