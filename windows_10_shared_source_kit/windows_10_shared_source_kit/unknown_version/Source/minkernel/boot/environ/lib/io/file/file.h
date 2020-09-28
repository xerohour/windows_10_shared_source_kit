/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    file.h

Abstract:

    Header file for boot environment file i/o.

    When file's are opened, an entry is created in a file table.  After
    this, all file i/o calls are directed based on function pointers set
    during the specific files open routine.

    This header is to be included by all boot environment file system
    implementations (but not by any application running in the boot
    environment. This file contains definitions for the routines each file
    system must implement and the internal data structures that are used
    by them.

Environment:

    boot

--*/

#pragma once

// ------------------------------------------------------------------- Includes

#include <bootlibp.h>
#include "systemevents.h"

//
// Disable warnings for non-data types as parameters to function.
//

#pragma warning(disable:4218)

//
// Disable warnings for function/data pointer conversion.
//

#pragma warning(disable:4152)

// ---------------------------------------------------------------- Definitions

typedef ULONG FILE_ATTRIBUTES;
typedef FILE_ATTRIBUTES *PFILE_ATTRIBUTES;

#define BOOT_FILE_OPEN                (0x00000001)
#define BOOT_FILE_READ                (0x00000002)
#define BOOT_FILE_WRITE               (0x00000004)
#define BOOT_FILE_CATALOG             (0x00000010)
#define BOOT_FILE_DIRECTORY           (0x00010000)
#define BOOT_FILE_REPARSE             (0x00020000)

// --------------------------------------------------------------------- Macros

#define SET_FILE_OPEN(_x)             SET_FLAGS(_x, BOOT_FILE_OPEN)
#define SET_FILE_READ(_x)             SET_FLAGS(_x, BOOT_FILE_READ)
#define SET_FILE_WRITE(_x)            SET_FLAGS(_x, BOOT_FILE_WRITE)
#define SET_FILE_CATALOG(_x)          SET_FLAGS(_x, BOOT_FILE_CATALOG)
#define SET_FILE_DIRECTORY(_x)        SET_FLAGS(_x, BOOT_FILE_DIRECTORY)
#define SET_FILE_REPARSE(_x)          SET_FLAGS(_x, BOOT_FILE_REPARSE)

#define CLEAR_FILE_OPEN(_x)           CLEAR_FLAGS(_x, BOOT_FILE_OPEN)
#define CLEAR_FILE_READ(_x)           CLEAR_FLAGS(_x, BOOT_FILE_READ)
#define CLEAR_FILE_WRITE(_x)          CLEAR_FLAGS(_x, BOOT_FILE_WRITE)
#define CLEAR_FILE_CATALOG(_x)        CLEAR_FLAGS(_x, BOOT_FILE_CATALOG)
#define CLEAR_FILE_DIRECTORY(_x)      CLEAR_FLAGS(_x, BOOT_FILE_DIRECTORY)
#define CLEAR_FILE_ATTRIBUTES(_x)     ((FILE_ATTRIBUTES)_x = 0)

#define IS_FILE_OPENED(_x)            CHECK_FLAG(_x, BOOT_FILE_OPEN)
#define IS_FILE_READ_SET(_x)          CHECK_FLAG(_x, BOOT_FILE_READ)
#define IS_FILE_WRITE_SET(_x)         CHECK_FLAG(_x, BOOT_FILE_WRITE)
#define IS_FILE_CATALOG_SET(_x)       CHECK_FLAG(_x, BOOT_FILE_CATALOG)
#define IS_FILE_DIRECTORY_SET(_x)     CHECK_FLAG(_x, BOOT_FILE_DIRECTORY)
#define IS_FILE_REPARSE_SET(_x)       CHECK_FLAG(_x, BOOT_FILE_REPARSE)


#define IS_FILE_OPENED_FOR_READ(_x)  (IS_FILE_OPENED(_x) && \
                                      IS_FILE_READ_SET(_x))
#define IS_FILE_OPENED_FOR_WRITE(_x) (IS_FILE_OPENED(_x) && \
                                      IS_FILE_WRITE_SET(_x))

// ------------------------------------------------------------ Data Structures

typedef struct _REGISTERED_FILESYSTEM {
    LIST_ENTRY                         ListEntry;
    FILESYSTEM_REGISTER_FUNCTION_TABLE FunctionTable;
} REGISTERED_FILESYSTEM, *PREGISTERED_FILESYSTEM;

/*++

Registered Filesystem Description :

    The File I/O Library maintains a list of filesystem that have been
    registered with the library.  If registered, the mount routine for the
    filesystem will be called when determining the filesystem on a particular
    device.

Fields:

    ListEntry - List entry pointing to the previous and next entries in the
        linked list.

    FunctionTable - Registered filesystem function table.

--*/

typedef struct _FILE_ENTRY {

    PWCHAR                     FilePath;

    DEVICE_ID                  DeviceId;
    FILE_ID                    FileId;

    FILE_ATTRIBUTES            Attributes;
    ULONG                      ReferenceCount;
    ULONG                      FilterLevel;
    ULONG                      Reserved;
    ULONGLONG                  BytesRead;
    ULONGLONG                  BytesWritten;

    FILESYSTEM_FUNCTION_TABLE  FunctionTable;
    PVOID                      FileData;

} FILE_ENTRY, *PFILE_ENTRY;

/*++

File Entry Description:

    A file entry is the internal representation for a file, describing the
    file that is opened, the attribute flags the file was opened with,
    a function table for I/O operations, etc.

Fields:

    FilePath - Saved copy of the file path.  The buffer must be allocated
        when the file entry is created and freed when the file entry is
        destroyed.

    DeviceId - Index for the device the file is located.

    FileId - Index into a file table.

    Attributes - Attribute flags for the file.  (Includes open/read/write).

    ReferenceCount - A reference count to the number of open instances of the
        file.

    FilterLevel - The File I/O Library supports filesystem filters by
        allowing a filesystem to mount a device and then forward the mount
        request, allowing a second filesystem to mount on the device.
        For the File I/O Library to fully support this functionality, it
        requires an additional level of granularity when describing a file (in
        addition to file name and device).  Each filesystem mounted on the
        same device will be assigned a different filter level (with the top
        level filesystem assigned level 0).  Therefore, a file entry can
        be defined for each filesystem in the stack.

    BytesRead - The number of bytes read from this file so far.

    BytesWritten - The number of bytes written to this file so far.

    FunctionTable - Function table for I/O operations on the file.

    FileData - A pointer to a file specific data buffer.  (Allocated
        during open and freed during close).  There are two types of data
        that are contained in this buffer,

        - File Information : Data that would be interesting to the user.  This
          could very well be the information passed using BlFileGetInformation
          and BlFileSetInformation.

        - Filesystem specific information : Data unique to the underlying
          filesystem.

        The representation of the above data is dependent on the implementation
        of the filesystem.  But it is recommended that this structure be
        defined as two distinct structure (one for each of the above cases).
        See FAT_FILE or NTFS_FILE for examples of this.

--*/

// -------------------------------------------------------------------- Globals

//
// Function Tables for known filesystems.
//

extern const FILESYSTEM_REGISTER_FUNCTION_TABLE FatRegisterFunctionTable;
extern const FILESYSTEM_REGISTER_FUNCTION_TABLE FppRegisterFunctionTable;
extern const FILESYSTEM_REGISTER_FUNCTION_TABLE NtfsRegisterFunctionTable;
#ifdef BLDR_REFS_SUPPORT
extern const FILESYSTEM_REGISTER_FUNCTION_TABLE RefsRegisterFunctionTable;
#endif
extern const FILESYSTEM_REGISTER_FUNCTION_TABLE EtfsRegisterFunctionTable;
extern const FILESYSTEM_REGISTER_FUNCTION_TABLE UdfsRegisterFunctionTable;
extern const FILESYSTEM_REGISTER_FUNCTION_TABLE WimRegisterFunctionTable;
extern const FILESYSTEM_REGISTER_FUNCTION_TABLE NetRegisterFunctionTable;
extern const FILESYSTEM_REGISTER_FUNCTION_TABLE VmbfsRegisterFunctionTable;
extern const PCFILESYSTEM_REGISTER_FUNCTION_TABLE FsTable[];



