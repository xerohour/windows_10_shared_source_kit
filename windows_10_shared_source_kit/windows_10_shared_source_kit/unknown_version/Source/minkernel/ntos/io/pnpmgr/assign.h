/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    assign.h

Abstract:

    This module implements assignment of hardware resources.

Author:

    Santosh Jodh (santoshj) 08-02-2004

Environment:

    KernelMode only.

Revision History:

--*/

#pragma once

//
// This structure is used build the group of devices that need resources to be assigned.
// The group is built by walking the device tree and adding devices to the group as
// appropriate.
//
typedef struct _PNP_ASSIGN_RESOURCES_CONTEXT {

    //
    // If TRUE, we include devices that previously failed to get resources. This is used
    // for retrying devices that have resource conflict when some device gets removed from
    // the system.
    //
    LOGICAL         IncludeFailedDevices;

    //
    // Specifies the number of devices in the group.
    //
    ULONG           DeviceCount;

    //
    // Array containing group of devices that need resources to be assigned.
    //
    PDEVICE_OBJECT  DeviceList[ANYSIZE_ARRAY];

} PNP_ASSIGN_RESOURCES_CONTEXT, *PPNP_ASSIGN_RESOURCES_CONTEXT;

KINLINE
PPNP_ASSIGN_RESOURCES_CONTEXT
PnpCreateAssignResourcesContext (
    __in LOGICAL IncludeFailedDevices
    )

/*++

Description

    This function will allocate and initialize the assign resources context. The context
    will be large enough to include all the existing devices in the system. Caller should
    ensure that the number of devices in the system does not grow while using this context.

Parameters

    IncludeFailed - Specifies if previously failed devices should be included.

Return

    Pointer to the initialized context if successful. Otherwise NULL.

--*/

{
    PPNP_ASSIGN_RESOURCES_CONTEXT context;
    ULONG size, deviceCount;

    deviceCount = PnpGetDeviceCount();

    ASSERT(deviceCount);

    size = sizeof(PNP_ASSIGN_RESOURCES_CONTEXT) + (deviceCount - 1) * sizeof(PDEVICE_OBJECT);

    context = ExAllocatePoolWithTag(PagedPool, size, PNP_POOLTAG_ASSIGN_RESOURCES_CONTEXT);
    if (context) {

        RtlZeroMemory(context, size);

        context->IncludeFailedDevices = IncludeFailedDevices;
    }

    return context;
}

KINLINE
VOID
PnpDestroyAssignResourcesContext (
    __deref_inout PPNP_ASSIGN_RESOURCES_CONTEXT *Context
    )

/*+

Description

    This function frees storage for the assing resources context.

Parameters

    Context - Pointer to the context to be freed.

Return

    None.

--*/

{
    ASSERT(Context);

    if (*Context) {

        ExFreePoolWithTag(*Context, PNP_POOLTAG_ASSIGN_RESOURCES_CONTEXT);

        *Context = NULL;
    }

    return;
}

//
// This structure is used to represent the resource request from a device and gets used
// for assigning resources to the device.
//
typedef struct _PNP_RESOURCE_REQUEST {

    //
    // Requesting device.
    //
    PDEVICE_OBJECT                 PhysicalDevice;

    //
    // Options for this request.
    //
    ULONG                          Flags;

    //
    // Request type.
    //
    ARBITER_REQUEST_SOURCE         AllocationType;

    //
    // Used for ordering requests from different devices. 0 is highest.
    //
    ULONG                          Priority;

    //
    // Index for entries with same priority.
    //
    ULONG                          Position;

    //
    // Requirements (input).
    //
    PIO_RESOURCE_REQUIREMENTS_LIST ResourceRequirements;

    //
    // Internal representation of requirements.
    //
    PVOID                          ReqList;

    //
    // Assigned raw resources (output).
    //
    PCM_RESOURCE_LIST              ResourceAssignment;

    //
    // Assigned translated resources (output).
    //
    PCM_RESOURCE_LIST              TranslatedResourceAssignment;

    //
    // Status of resource assignment.
    //
    NTSTATUS                       Status;

} PNP_RESOURCE_REQUEST, *PPNP_RESOURCE_REQUEST;


#define IOP_ASSIGN_RETRY              0x00000008    // Retry resource allocation later
#define IOP_ASSIGN_EXCLUDE            0x00000010    // internal IopAssign flag
#define IOP_ASSIGN_IGNORE             0x00000020    // ignore this request
#define IOP_ASSIGN_NO_REBALANCE       0x00000080    // no rebal if assign fails
#define IOP_ASSIGN_RESOURCES_RELEASED 0x00000100    // resources are released for rebalancing
#define IOP_ASSIGN_KEEP_CURRENT_CONFIG 0x00000200   // Indicate non-stopped rebalance.  We need to
                                                    //   preserved the current config.
#define IOP_ASSIGN_CLEAR_RESOURCE_REQUIREMENTS_CHANGE_FLAG \
                                      0x00000400

KINLINE
PPNP_RESOURCE_REQUEST
PnpCreateResourceRequest (
    __in ULONG Count
    )

/*++

Description

    This function will allocate specified number of resource requests.

Parameters

    Count - Number of resource requests to be created.

Return

    Pointer to the requests if successful. Otherwise NULL.

--*/

{
    PPNP_RESOURCE_REQUEST request;
    ULONG size;

    PNP_ASSERT(Count);

    size = sizeof(PNP_RESOURCE_REQUEST) * Count;
    request = ExAllocatePoolWithTag(PagedPool,
                                    size,
                                    PNP_POOLTAG_RESOURCE_REQUEST);
    if (request) {

        RtlZeroMemory(request, size);
    }

    return request;
}

KINLINE
VOID
PnpInitializeResourceRequest (
    __inout PPNP_RESOURCE_REQUEST           Request,
    __in PDEVICE_OBJECT                     Device,
    __in_opt PIO_RESOURCE_REQUIREMENTS_LIST Requirements,
    __in ARBITER_REQUEST_SOURCE             Type
    )

/*++

Description

    This function will initialize a resource request.

Parameters

    Request - Pointer to the resource request that needs to be initialized.

    Device - Pointer to the device requesting resources.

    Requirements - Pointer to the resource requirements.

    Type - Specifies type of resource request (legacy\pnp etc).

Return

    None.

--*/

{
    PNP_ASSERT(Request);

    RtlZeroMemory(Request, sizeof(PNP_RESOURCE_REQUEST));

    Request->PhysicalDevice = Device;
    Request->ResourceRequirements = Requirements;
    Request->AllocationType = Type;

    return;
}

KINLINE
VOID
PnpDestroyResourceRequest (
    __deref_inout PPNP_RESOURCE_REQUEST *Request
    )

/*+

Description

    This function frees storage for the resource request.

Parameters

    Request - Pointer to the request to be freed.

Return

    None.

--*/

{
    PNP_ASSERT(Request);

    if (*Request) {

        ExFreePoolWithTag(*Request, PNP_POOLTAG_RESOURCE_REQUEST);

        *Request = NULL;
    }

    return;
}

NTSTATUS
PnpAssignResourcesToDevices (
    __in ULONG                                         DeviceCount,
    __inout_ecount(DeviceCount) PPNP_RESOURCE_REQUEST  RequestTable,
    __out PLOGICAL                                     RebalancePerformed
    );

NTSTATUS
PnpProcessAssignResourcesWorker (
   __in PDEVICE_NODE                    DeviceNode,
   __in PPNP_ASSIGN_RESOURCES_CONTEXT   ResourceContext
   );

LOGICAL
PnpProcessAssignResources (
   __in PDEVICE_NODE    DeviceNode,
   __in LOGICAL         Reallocation,
   __out PLOGICAL       RebalancePerformed
   );

NTSTATUS
PnpGetResourceRequirementsForAssignTable (
    __in  PPNP_RESOURCE_REQUEST   RequestTable,
    __in_ecount(0) PPNP_RESOURCE_REQUEST   RequestTableEnd,
    __out PULONG                  DeviceCount
    );

VOID
PnpFreeResourceRequirementsForAssignTable (
    __in PPNP_RESOURCE_REQUEST AssignTable,
    __in_ecount(0) PPNP_RESOURCE_REQUEST AssignTableEnd
    );

VOID
PnpBuildCmResourceLists (
    __in PPNP_RESOURCE_REQUEST AssignTable,
    __in_ecount(0) PPNP_RESOURCE_REQUEST AssignTableEnd,
    __in BOOLEAN UpdateDevnode
    );

VOID
PnpBuildCmResourceList (
    __inout PPNP_RESOURCE_REQUEST AssignEntry,
    __in LOGICAL PublishAsResourceMap
    );

VOID
PnpReleaseResourcesInternal (
    __in PDEVICE_NODE DeviceNode
    );

NTSTATUS
PnpRestoreResourcesInternal (
    __in PDEVICE_NODE DeviceNode
    );

VOID
PnpReallocateResources (
    __in PDEVICE_NODE DeviceNode
    );

VOID
PnpRearrangeAssignTable (
    __in_ecount(Count) PPNP_RESOURCE_REQUEST AssignTable,
    __in ULONG Count
    );


int
__cdecl
PnpCompareResourceRequestPriority (
    const void *arg1,
    const void *arg2
    );

NTSTATUS
PnpAllocateResources (
    __in ULONG                                         RequestCount,
    __inout_ecount(RequestCount) PPNP_RESOURCE_REQUEST Request,
    __in LOGICAL                                       ResourceManagerLocked,
    __out_opt PLOGICAL                                 RebalancePerformed
    );

#if DBG

VOID
PnpDumpResourceDescriptor (
    __in PCHAR Indent,
    __in PIO_RESOURCE_DESCRIPTOR Desc
    );

VOID
PnpDumpResourceRequirementsList (
    __in PIO_RESOURCE_REQUIREMENTS_LIST IoResources
    );

VOID
PnpDumpCmResourceDescriptor (
    __in PCHAR Indent,
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Desc
    );

VOID
PnpDumpCmResourceList (
    __in PCM_RESOURCE_LIST CmList
    );

#else

#define PnpDumpResourceRequirementsList(x)
#define PnpDumpResourceDescriptor(x,y)
#define PnpDumpCmResourceList(c)
#define PnpDumpCmResourceDescriptor(i,d)

#endif // DBG

extern const WCHAR PnpWstrRaw[];
extern const WCHAR PnpWstrTranslated[];

