/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    macro.h

Abstract:

    This header defines various generic macros for the Plug and Play subsystem.

Author:

    Adrian J. Oney (AdriaO) July 26, 2000.

Revision History:

    Santosh S. Jodh (SantoshJ) August 09, 2004  Consolidate macros.

--*/

//
// This is to make all the TEXT(...) macros come out right. As of 07/27/2000,
// UNICODE isn't defined in kernel space by default.
//
#define UNICODE

//
// This macro is used to convert HKLM relative paths from user-mode accessable
// headers into a form usable by kernel mode. Eventually this macro should be
// moved to somewhere like cm.h so the entire kernel can use it.
//
#define CM_REGISTRY_MACHINE(x) L"\\Registry\\Machine\\"##x

//
// This macro returns the pointer to the beginning of the data
// area of KEY_VALUE_FULL_INFORMATION structure.
// In the macro, k is a pointer to KEY_VALUE_FULL_INFORMATION structure.
//
#define KEY_VALUE_DATA(k) ((PCHAR)(k) + (k)->DataOffset)

//
// This macros calculates the size in bytes of a constant string
//
//  ULONG
//  IopConstStringSize(
//      IN CONST PWSTR String
//      );
//
#define IopConstStringSize(String)          ( sizeof(String) - sizeof(UNICODE_NULL) )

//
// This macros calculates the number of characters of a constant string
//
//  ULONG
//  IopConstStringLength(
//      IN CONST PWSTR String
//      );
//
#define IopConstStringLength(String)        ( ( sizeof(String) - sizeof(UNICODE_NULL) ) / sizeof(WCHAR) )

#ifndef FIELD_SIZE
#define FIELD_SIZE(type, field) (sizeof(((type *)0)->field))
#endif

//
// This macro takes a value and an alignment and rounds the entry up
// appropriately. The alignment MUST be a power of two!
//
#define ALIGN_UP_ULONG(value, alignment) (((value)+(alignment)-1)&(~(alignment-1)))

//
// The following macros convert between a Count of Wide Characters (CWC) and a Count
// of Bytes (CB).
//
#define CWC_TO_CB(c)    ((c) * sizeof(WCHAR))
#define CB_TO_CWC(c)    ((c) / sizeof(WCHAR))

//
// Macro to determine the number of elements in a statically
// initialized array.
//
#define ELEMENT_COUNT(x) (sizeof(x)/sizeof((x)[0]))

//++
//
// VOID
// PiWstrToUnicodeString(
//     OUT PUNICODE_STRING u,
//     IN  PCWSTR p
//     )
//
//--
#define PiWstrToUnicodeString(u, p) {                                       \
    if (p) {                                                                \
        (u)->Length = ((u)->MaximumLength = sizeof((p))) - sizeof(WCHAR);   \
    } else {                                                                \
        (u)->Length = (u)->MaximumLength = 0;                               \
    }                                                                       \
    (u)->Buffer = (p);                                                      \
}

//++
//
// VOID
// PiUlongToUnicodeString(
//     OUT    PUNICODE_STRING u,
//     IN OUT PWCHAR ub,
//     IN     ULONG ubl,
//     IN     ULONG i
//     )
//
//--
#define PiUlongToUnicodeString(u, ub, ubl, i)                                                                               \
    {                                                                                                                       \
        PWCHAR end;                                                                                                         \
        LONG len;                                                                                                           \
                                                                                                                            \
        end = (PWCHAR)(ub);                                                                                                 \
        RtlStringCchPrintfExW((PWCHAR)(ub), (ubl) / sizeof(WCHAR), &end, NULL, 0, REGSTR_VALUE_STANDARD_ULONG_FORMAT, (i)); \
        len = (LONG)(end - (PWCHAR)(ub));                                                                                   \
        (u)->MaximumLength = (USHORT)(ubl);                                                                                 \
        (u)->Length = (len == -1) ? (USHORT)(ubl) : (USHORT)len * sizeof(WCHAR);                                            \
        (u)->Buffer = (PWSTR)(ub);                                                                                          \
    }

//++
//
// VOID
// PiUlongToInstanceKeyUnicodeString(
//     OUT    PUNICODE_STRING u,
//     IN OUT PWCHAR ub,
//     IN     ULONG ubl,
//     IN     ULONG i
//     )
//
//--
#define PiUlongToInstanceKeyUnicodeString(u, ub, ubl, i)                                                                \
    {                                                                                                                   \
        PWCHAR end;                                                                                                     \
        LONG len;                                                                                                       \
                                                                                                                        \
        end = (PWCHAR)(ub);                                                                                             \
        RtlStringCchPrintfExW((PWCHAR)(ub), (ubl) / sizeof(WCHAR), &end, NULL, 0, REGSTR_KEY_INSTANCE_KEY_FORMAT, (i)); \
        len = (LONG)(end - (PWCHAR)(ub));                                                                               \
        (u)->MaximumLength = (USHORT)(ubl);                                                                             \
        (u)->Length = (len == -1) ? (USHORT)(ubl) : (USHORT)len * sizeof(WCHAR);                                        \
        (u)->Buffer = (PWSTR)(ub);                                                                                      \
    }

//  BOOLEAN
//  IopCompareGuid(
//      IN LPGUID guid1,
//      IN LPGUID guid2
//      );

#define IopCompareGuid(g1, g2)  ( (g1) == (g2) \
                                    ? TRUE \
                                    : RtlCompareMemory( (g1), (g2), sizeof(GUID) ) == sizeof(GUID) \
                                    )

//++
//
// VOID
// IopRegistryDataToUnicodeString(
//     OUT PUNICODE_STRING u,
//     IN  PWCHAR p,
//     IN  ULONG l
//     )
//
//--
#define IopRegistryDataToUnicodeString(u, p, l)  \
    {                                            \
        ULONG len;                               \
                                                 \
        PnpRegSzToString((p), (l), &len, NULL);   \
        (u)->Length = (USHORT)len;               \
        (u)->MaximumLength = (USHORT)(l);        \
        (u)->Buffer = (p);                       \
    }

//
// These macros abstract the notify lock routines.
//

#define IopAcquireResourceListLock(Lock)     KeAcquireGuardedMutex(Lock);
#define IopReleaseResourceListLock(Lock)     KeReleaseGuardedMutex(Lock);

