// Copyright(C) Microsoft.All rights reserved.

#pragma once

#ifndef _DRMSTRUCT_H_
#define _DRMSTRUCT_H_

#include <windows.h>
#include <ks.h>


DEFINE_GUID(IID_IUnknown,
0x00000000, 0x0000, 0x0000, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);
#if defined(__cplusplus) && _MSC_VER >= 1100
struct __declspec(uuid("00000000-0000-0000-c000-000000000046")) IUnknown;
#endif
#undef INTERFACE
#define INTERFACE IUnknown
#undef INTERFACE
typedef IUnknown *PUNKNOWN;

//
// Unicode strings are counted 16-bit character strings. If they are
// NULL terminated, Length does not include trailing NULL.
//

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
#ifdef MIDL_PASS
    [size_is(MaximumLength / 2), length_is((Length) / 2) ] USHORT * Buffer;
#else // MIDL_PASS
    PWSTR  Buffer;
#endif // MIDL_PASS
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;
#define UNICODE_NULL ((WCHAR)0) // winnt


//
// Structures for use with KSPROPERY_DRMAUDIOSTREAM_CONTENTID
//
typedef struct tagDRMRIGHTS {
    BOOL  CopyProtect;
    ULONG Reserved;
    BOOL  DigitalOutputDisable;
} DRMRIGHTS , *PDRMRIGHTS;
typedef const DRMRIGHTS *PCDRMRIGHTS;

#define DEFINE_DRMRIGHTS_DEFAULT(DrmRights) const DRMRIGHTS DrmRights = {FALSE, 0, FALSE}

//
// Define empty typedefs for the _IRP, _DEVICE_OBJECT, and _DRIVER_OBJECT
// structures so they may be referenced by function types before they are
// actually defined.
//
typedef struct _DEVICE_OBJECT *PDEVICE_OBJECT;
typedef struct _FILE_OBJECT *PFILE_OBJECT;

typedef struct tagDRMFORWARD {
    DWORD          Flags;
    PDEVICE_OBJECT DeviceObject;
    PFILE_OBJECT   FileObject;
    PVOID          Context;
} DRMFORWARD, *PDRMFORWARD;

typedef const DRMFORWARD *PCDRMFORWARD;


typedef
(NTAPI *PFNDRMADDCONTENTHANDLERS)(
    IN ULONG ContentId,
    IN PVOID* paHandlers,
    IN ULONG NumHandlers
    );


typedef
(NTAPI *PFNDRMCREATECONTENTMIXED)(
    IN PULONG paContentId,
    IN ULONG cContentId,
    OUT PULONG pMixedContentId
    );

typedef
(NTAPI *PFNDRMDESTROYCONTENT)(
    IN ULONG ContentId
    );

typedef
(NTAPI *PFNDRMFORWARDCONTENTTODEVICEOBJECT)(
    IN ULONG ContentId,
    IN PVOID Reserved,
    IN PCDRMFORWARD DrmForward
    );

typedef
(NTAPI *PFNDRMFORWARDCONTENTTOFILEOBJECT)(
    IN ULONG ContentId,
    IN PFILE_OBJECT FileObject
    );

typedef
(NTAPI *PFNDRMFORWARDCONTENTTOINTERFACE)(
    ULONG ContentId,
    PUNKNOWN pUnknown,
    ULONG NumMethods);

typedef
(NTAPI *PFNDRMGETCONTENTRIGHTS)(
    IN ULONG ContentId,
    OUT PDRMRIGHTS DrmRights
    );


//
// Structure for use with KSPROPERY_DRMAUDIOSTREAM_CONTENTID
//

typedef struct {
    ULONG     ContentId;
    DRMRIGHTS DrmRights;
} KSDRMAUDIOSTREAM_CONTENTID, *PKSDRMAUDIOSTREAM_CONTENTID;

typedef struct {
    KSPROPERTY                         Property;
    PVOID                              Context;

    PFNDRMADDCONTENTHANDLERS            DrmAddContentHandlers;
    PFNDRMCREATECONTENTMIXED            DrmCreateContentMixed;
    PFNDRMDESTROYCONTENT                DrmDestroyContent;
    PFNDRMFORWARDCONTENTTODEVICEOBJECT  DrmForwardContentToDeviceObject;
    PFNDRMFORWARDCONTENTTOFILEOBJECT    DrmForwardContentToFileObject;
    PFNDRMFORWARDCONTENTTOINTERFACE     DrmForwardContentToInterface;
    PFNDRMGETCONTENTRIGHTS              DrmGetContentRights;

} KSP_DRMAUDIOSTREAM_CONTENTID, *PKSP_DRMAUDIOSTREAM_CONTENTID;

//
// DDK related Definitions
//

#define DEVICE_TYPE ULONG

typedef CCHAR KPROCESSOR_MODE;
typedef struct _IO_TIMER *PIO_TIMER;
typedef short CSHORT;
typedef CSHORT *PCSHORT;
typedef LONG NTSTATUS;



//
// Define the actions that a driver execution routine may request of the
// adapter/controller allocation routines upon return.
//

typedef enum _IO_ALLOCATION_ACTION {
    KeepObject = 1,
    DeallocateObject,
    DeallocateObjectKeepRegisters
} IO_ALLOCATION_ACTION, *PIO_ALLOCATION_ACTION;

//
// DPC routine
//

struct _KDPC;

typedef
VOID
(*PKDEFERRED_ROUTINE) (
    IN struct _KDPC *Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );


typedef struct _KDEVICE_QUEUE_ENTRY {
    LIST_ENTRY DeviceListEntry;
    ULONG SortKey;
    BOOLEAN Inserted;
} KDEVICE_QUEUE_ENTRY, *PKDEVICE_QUEUE_ENTRY, *PRKDEVICE_QUEUE_ENTRY;

//
// Define device driver adapter/controller execution routine.
//

typedef
IO_ALLOCATION_ACTION
(*PDRIVER_CONTROL) (
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN struct _IRP *Irp,
    IN PVOID MapRegisterBase,
    IN PVOID Context
    );

typedef struct _KDPC {
    CSHORT Type;
    UCHAR Number;
    UCHAR Importance;
    LIST_ENTRY DpcListEntry;
    PKDEFERRED_ROUTINE DeferredRoutine;
    PVOID DeferredContext;
    PVOID SystemArgument1;
    PVOID SystemArgument2;
    PVOID DpcData;
} KDPC, *PKDPC, *PRKDPC;

//
// Define Wait Context Block (WCB)
//

typedef struct _WAIT_CONTEXT_BLOCK {
    KDEVICE_QUEUE_ENTRY WaitQueueEntry;
    PDRIVER_CONTROL DeviceRoutine;
    PVOID DeviceContext;
    ULONG NumberOfMapRegisters;
    PVOID DeviceObject;
    PVOID CurrentIrp;
    PKDPC BufferChainingDpc;
} WAIT_CONTEXT_BLOCK, *PWAIT_CONTEXT_BLOCK;


//
// Device Queue object and entry
//

typedef struct _KDEVICE_QUEUE {
    CSHORT Type;
    CSHORT Size;
    LIST_ENTRY DeviceListHead;
    KSPIN_LOCK Lock;
    BOOLEAN Busy;
} KDEVICE_QUEUE, *PKDEVICE_QUEUE, *PRKDEVICE_QUEUE;


//
// Common dispatcher object header
//
// N.B. The size field contains the number of dwords in the structure.
//

typedef struct _DISPATCHER_HEADER {
    union {
        struct {
            UCHAR Type;
            UCHAR Flags;
            UCHAR Size;
            union {
                UCHAR Inserted;
                BOOLEAN DebugActive;
            };
        };

        volatile LONG Lock;
    };

    LONG SignalState;
    LIST_ENTRY WaitListHead;
} DISPATCHER_HEADER;


typedef struct _KEVENT {
    DISPATCHER_HEADER Header;
} KEVENT, *PKEVENT, *PRKEVENT;


//
// Device Object structure definition
//

typedef struct DECLSPEC_ALIGN(MEMORY_ALLOCATION_ALIGNMENT) _DEVICE_OBJECT {
    CSHORT Type;
    USHORT Size;
    LONG ReferenceCount;
    struct _DRIVER_OBJECT *DriverObject;
    struct _DEVICE_OBJECT *NextDevice;
    struct _DEVICE_OBJECT *AttachedDevice;
    struct _IRP *CurrentIrp;
    PIO_TIMER Timer;
    ULONG Flags;                                // See above:  DO_...
    ULONG Characteristics;                      // See ntioapi:  FILE_...
    PVOID DoNotUse1;
    PVOID DeviceExtension;
    DEVICE_TYPE DeviceType;
    CCHAR StackSize;
    union {
        LIST_ENTRY ListEntry;
        WAIT_CONTEXT_BLOCK Wcb;
    } Queue;
    ULONG AlignmentRequirement;
    KDEVICE_QUEUE DeviceQueue;
    KDPC Dpc;

    //
    //  The following field is for exclusive use by the filesystem to keep
    //  track of the number of Fsp threads currently using the device
    //

    ULONG ActiveThreadCount;
    PSECURITY_DESCRIPTOR SecurityDescriptor;
    KEVENT DeviceLock;

    USHORT SectorSize;
    USHORT Spare1;

    struct _DEVOBJ_EXTENSION  *DeviceObjectExtension;
    PVOID  Reserved;
} DEVICE_OBJECT;

typedef struct _DEVICE_OBJECT *PDEVICE_OBJECT;


//
// The following structure is pointed to by the SectionObject pointer field
// of a file object, and is allocated by the various NT file systems.
//

typedef struct _SECTION_OBJECT_POINTERS {
    PVOID DataSectionObject;
    PVOID SharedCacheMap;
    PVOID ImageSectionObject;
} SECTION_OBJECT_POINTERS;
typedef SECTION_OBJECT_POINTERS *PSECTION_OBJECT_POINTERS;

//
// Define the format of a completion message.
//

typedef struct _IO_COMPLETION_CONTEXT {
    PVOID Port;
    PVOID Key;
} IO_COMPLETION_CONTEXT, *PIO_COMPLETION_CONTEXT;


typedef struct _FILE_OBJECT {
    CSHORT Type;
    CSHORT Size;
    PDEVICE_OBJECT DeviceObject;
    PVOID DoNotUse1;
    PVOID FsContext;
    PVOID FsContext2;
    PSECTION_OBJECT_POINTERS SectionObjectPointer;
    PVOID PrivateCacheMap;
    NTSTATUS FinalStatus;
    union {
        struct _FILE_OBJECT *RelatedFileObject;
        PVOID               AvioInformation;
    };
    BOOLEAN LockOperation;
    BOOLEAN DeletePending;
    BOOLEAN ReadAccess;
    BOOLEAN WriteAccess;
    BOOLEAN DeleteAccess;
    BOOLEAN SharedRead;
    BOOLEAN SharedWrite;
    BOOLEAN SharedDelete;
    ULONG Flags;
    UNICODE_STRING FileName;
    LARGE_INTEGER CurrentByteOffset;
    ULONG Waiters;
    ULONG Busy;
    PVOID LastLock;
    KEVENT Lock;
    KEVENT Event;
    PIO_COMPLETION_CONTEXT CompletionContext;
    KSPIN_LOCK IrpListLock;
    LIST_ENTRY   IrpList;
} FILE_OBJECT;
typedef struct _FILE_OBJECT *PFILE_OBJECT;

#endif //_DRMSTRUCT_H_