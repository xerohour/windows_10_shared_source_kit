/*****************************************************************************
 * minihelp.c - services provided by the port class driver to miniports
 *****************************************************************************
 * Copyright (c) Microsoft.  All rights reserved.
 */

#define _NO_SYS_GUID_OPERATOR_EQ_        // So operator == / != in guiddef.h isn't included
#include "private.h"
#include <kcom.h>

#ifdef DRM_PORTCLS
#include <drmk.h>
#endif  // DRM_PORTCLS


NTSTATUS
GetClassInfo
(
    IN      REFCLSID            ClassId,
    OUT     PFNCREATEINSTANCE * Create
);

#pragma code_seg("PAGE")

/*****************************************************************************
 * PcNewPort()
 *****************************************************************************
 * Creates an instance of a port driver.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcNewPort
(
    __out   PPORT *     OutPort,
    __in    REFCLSID    ClassId
)
{
    PAGED_CODE();

    ASSERT(OutPort);

    //
    // Validate Parameters.
    //
    if (NULL == OutPort)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcNewPort : Invalid Parameter."));
        return STATUS_INVALID_PARAMETER;
    }

    PUNKNOWN            unknown;
    PFNCREATEINSTANCE   create;

    NTSTATUS ntStatus =
        GetClassInfo
        (
            ClassId,
            &create
        );

    if (NT_SUCCESS(ntStatus))
    {
        ntStatus =
            create
            (
                &unknown,
                ClassId,
                NULL,
                NonPagedPoolNx
            );

        if (NT_SUCCESS(ntStatus))
        {
            ntStatus = unknown->QueryInterface(IID_IPort,(PVOID *) OutPort);

            unknown->Release();
        }
    }

    return ntStatus;
}

/*****************************************************************************
 * PcNewMiniport()
 *****************************************************************************
 * Creates an instance of a miniport driver.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcNewMiniport
(
    __out   PMINIPORT * OutMiniport,
    __in    REFCLSID    ClassId
)
{
    PAGED_CODE();

    ASSERT(OutMiniport);

    //
    // Validate Parameters.
    //
    if (NULL == OutMiniport)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcNewMiniport : Invalid Parameter."));
        return STATUS_INVALID_PARAMETER;
    }

    PUNKNOWN            unknown;
    PFNCREATEINSTANCE   create;

    NTSTATUS ntStatus =
        GetClassInfo
        (
            ClassId,
            &create
        );

    if (NT_SUCCESS(ntStatus))
    {
        ntStatus =
            create
            (
                &unknown,
                ClassId,
                NULL,
                NonPagedPoolNx
            );

        if (NT_SUCCESS(ntStatus))
        {
            ntStatus =
                unknown->QueryInterface
                (
                    IID_IMiniport,
                    (PVOID *) OutMiniport
                );

            unknown->Release();
        }
    }

    return ntStatus;
}

#pragma code_seg()

/*****************************************************************************
 * PcGetTimeInterval
 *****************************************************************************
 * Gets the system time interval
 */
PORTCLASSAPI
ULONGLONG
NTAPI
PcGetTimeInterval
(
    __in    ULONGLONG   Since
)
{
    LARGE_INTEGER deltaTime;
    KeQuerySystemTime(&deltaTime);
    return deltaTime.QuadPart - Since;
}


#pragma code_seg("PAGE")

#ifdef DRM_PORTCLS

PORTCLASSAPI
NTSTATUS
NTAPI
PcAddContentHandlers(__in ULONG ContentId,__in_ecount(NumHandlers) PVOID* paHandlers,__in ULONG NumHandlers)
{
    PAGED_CODE();
    return DrmAddContentHandlers(ContentId,paHandlers,NumHandlers);
}

PORTCLASSAPI
NTSTATUS
NTAPI
PcCreateContentMixed(__in  PULONG paContentId,__in ULONG cContentId,__out PULONG pMixedContentId)
{
    PAGED_CODE();
    return DrmCreateContentMixed(paContentId,cContentId,pMixedContentId);
}

PORTCLASSAPI
NTSTATUS
NTAPI
PcDestroyContent(__in ULONG ContentId)
{
    PAGED_CODE();
    return DrmDestroyContent(ContentId);
}

PORTCLASSAPI
NTSTATUS
NTAPI
PcForwardContentToDeviceObject(__in ULONG ContentId,__in PVOID Reserved,__in PCDRMFORWARD DrmForward)
{
    PAGED_CODE();
    return DrmForwardContentToDeviceObject(ContentId,Reserved,DrmForward);
}

PORTCLASSAPI
NTSTATUS
NTAPI
PcForwardContentToFileObject(__in ULONG ContentId,__in PFILE_OBJECT FileObject)
{
    PAGED_CODE();
    return DrmForwardContentToFileObject(ContentId,FileObject);
}

PORTCLASSAPI 
NTSTATUS 
NTAPI
PcForwardContentToInterface(__in ULONG    ContentId,
                            __in PUNKNOWN pUnknown,
                            __in ULONG    NumMethods)
{
    PAGED_CODE();
    return DrmForwardContentToInterface(ContentId,pUnknown,NumMethods);
}

PORTCLASSAPI
NTSTATUS
NTAPI
PcGetContentRights(__in ULONG ContentId,__out PDRMRIGHTS DrmRights)
{
    PAGED_CODE();
    return DrmGetContentRights(ContentId,DrmRights);
}


#else   // DRM_PORTCLS

PORTCLASSAPI
NTSTATUS
NTAPI
PcAddContentHandlers(__in ULONG ContentId,__in_ecount(NuMHandlers) PVOID* paHandlers,__in ULONG NumHandlers)
{
    PAGED_CODE();
    return STATUS_UNSUCCESSFUL;
}

PORTCLASSAPI
NTSTATUS
NTAPI
PcCreateContentMixed(IN  PULONG paContentId,
                     IN  ULONG  cContentId,
                     OUT PULONG pMixedContentId)
{
    PAGED_CODE();
    return STATUS_UNSUCCESSFUL;
}

PORTCLASSAPI
NTSTATUS
NTAPI
PcDestroyContent(IN ULONG ContentId)
{
    PAGED_CODE();
    return STATUS_UNSUCCESSFUL;
}

PORTCLASSAPI
NTSTATUS
NTAPI
PcForwardContentToDeviceObject(IN ULONG ContentId,IN PVOID Reserved,IN PCDRMFORWARD DrmForward)
{
    PAGED_CODE();
    return STATUS_UNSUCCESSFUL;
}

PORTCLASSAPI
NTSTATUS
NTAPI
PcForwardContentToFileObject(IN ULONG ContentId,IN PFILE_OBJECT FileObject)
{
    PAGED_CODE();
    return STATUS_UNSUCCESSFUL;
}

PORTCLASSAPI 
NTSTATUS 
NTAPI
PcForwardContentToInterface(IN ULONG    ContentId,
                            IN PUNKNOWN pUnknown,
                            IN ULONG    NumMethods)
{
    PAGED_CODE();
    return STATUS_UNSUCCESSFUL;
}

PORTCLASSAPI
NTSTATUS
NTAPI
PcGetContentRights(IN ULONG ContentId,OUT PDRMRIGHTS DrmRights)
{
    PAGED_CODE();
    return STATUS_UNSUCCESSFUL;
}

#endif  // DRM_PORTCLS
