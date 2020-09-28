/*****************************************************************************
 * portcls.cpp - WDM Streaming port class driver
 *****************************************************************************
 * Copyright (c) Microsoft.  All rights reserved.
 */

#define KSDEBUG_INIT
#include "private.h"
#include <wdmsec.h>
#include <ksmediap.h>
#include <process.h>

#include "pcwpptr.h"
#include "portcls.tmh"

#include "portclsEvents.h"

#include "verifier.h"

#define IDS_POWER_AUDIO_IN_USE 900  // power diagnostic string resource ID - see mmres.dll

/*****************************************************************************
 * PortCls devobj security descriptor
 */
    // SY ALL   (System)
    // BA ALL   (Built-In Admins)
    // LS RWX   (Local Service)
    // IU RWX   (Interactive User)
    // S-1-5-80-2676549577-1911656217-2625096541-4178041876-1366760775 RWX (AudioSrv)
DECLARE_CONST_UNICODE_STRING(
    SDDL_PORTCLS_DEVOBJ,
    L"D:P(A;;GA;;;SY)(A;;GA;;;BA)(A;;GRGWGX;;;LS)(A;;GRGWGX;;;IU)"
    );

/*****************************************************************************
 * Referenced forward.
 */

DRIVER_UNLOAD           KsoNullDriverUnload;
IO_TIMER_ROUTINE        IoTimeoutRoutine;

static IO_COMPLETION_ROUTINE   ForwardIrpCompletionRoutine;

#define PORTCLS_DRIVER_EXTENSION_ID 0x0ABADCAFE

/*****************************************************************************
 * Globals
 */

ULONG gBufferDuration=0;
KAFFINITY gInterruptAffinity=0;
PUNICODE_STRING g_RegistryPath = NULL;

/*****************************************************************************
 * ENUMS FOR PnP Notify 
 */
 
typedef enum{
    PcPnpInvalid = 0,
    PcPnpStop,
    PcPnpQueryStop,
    PcPnpCancelStop,
    // possibly others in future.
} PC_PNPNOTIFICATION_TYPE;


/*****************************************************************************
 * Functions.
 */

#pragma code_seg("PAGE")


NTSTATUS
QueryRegistryValueEx(
    __in ULONG Hive,
    __in PWSTR pwstrRegistryPath,
    __in PWSTR pwstrRegistryValue,
    __in ULONG uValueType,
    __out PVOID *ppValue,
    __out_bcount(DefaultDataLength) PVOID pDefaultData,
    __in ULONG DefaultDataLength
)
{
    PRTL_QUERY_REGISTRY_TABLE pRegistryValueTable = NULL;
    UNICODE_STRING usString = {0};
    DWORD dwValue = 0;
    NTSTATUS Status = STATUS_SUCCESS;
    usString.Buffer = NULL;

    PAGED_CODE();

    pRegistryValueTable = (PRTL_QUERY_REGISTRY_TABLE) ExAllocatePoolWithTag(
                            PagedPool,
                            (sizeof(RTL_QUERY_REGISTRY_TABLE)*2),
                            'vRcP');

    if(!pRegistryValueTable) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    RtlZeroMemory(pRegistryValueTable, (sizeof(RTL_QUERY_REGISTRY_TABLE)*2));

    pRegistryValueTable[0].Flags = RTL_QUERY_REGISTRY_DIRECT | RTL_QUERY_REGISTRY_TYPECHECK;
    pRegistryValueTable[0].Name = pwstrRegistryValue;
    pRegistryValueTable[0].DefaultType = (uValueType << RTL_QUERY_REGISTRY_TYPECHECK_SHIFT) | uValueType;
    pRegistryValueTable[0].DefaultLength = DefaultDataLength;
    pRegistryValueTable[0].DefaultData = pDefaultData;

    switch (uValueType) {
        case REG_SZ:
            pRegistryValueTable[0].EntryContext = &usString;
            break;
        case REG_DWORD:
            pRegistryValueTable[0].EntryContext = &dwValue;
            break;
        default:
            Status = STATUS_INVALID_PARAMETER ;
            goto exit;
    }

    Status = RtlQueryRegistryValues(
      Hive,
      pwstrRegistryPath,
      pRegistryValueTable,
      NULL,
      NULL);

    if(!NT_SUCCESS(Status)) {
        goto exit;
    }

    switch (uValueType) {
        case REG_SZ:
            *ppValue = ExAllocatePoolWithTag(
                        PagedPool,
                        usString.Length + sizeof(UNICODE_NULL),
                        'sUcP');
            if(!(*ppValue)) {
                RtlFreeUnicodeString(&usString);
                Status = STATUS_INSUFFICIENT_RESOURCES;
                goto exit;
            }
            memcpy(*ppValue, usString.Buffer, usString.Length);
            ((PWCHAR)*ppValue)[usString.Length/sizeof(WCHAR)] = UNICODE_NULL;

            RtlFreeUnicodeString(&usString);
            break;

        case REG_DWORD:
            *ppValue = ExAllocatePoolWithTag(
                        PagedPool,
                        sizeof(DWORD),
                        'WDcP');
            if(!(*ppValue)) {
                Status = STATUS_INSUFFICIENT_RESOURCES;
                goto exit;
            }
            *((DWORD *)(*ppValue)) = dwValue;
            break;

        default:
            Status = STATUS_INVALID_PARAMETER ;
            goto exit;
    }
exit:
    if (pRegistryValueTable) {
        ExFreePool(pRegistryValueTable);
    }
    return(Status);
}


ULONG
GetUlongFromRegistry(
    __in PWSTR pwstrRegistryPath,
    __in PWSTR pwstrRegistryValue,
    __in ULONG DefaultValue
)
{
    PVOID      pulValue ;
    ULONG       ulValue ;
    NTSTATUS    Status ;

    PAGED_CODE();

    Status = QueryRegistryValueEx(RTL_REGISTRY_ABSOLUTE,
                         pwstrRegistryPath,
                         pwstrRegistryValue,
                         REG_DWORD,
                         &pulValue,
                         &DefaultValue,
                         sizeof(DWORD));
    if (NT_SUCCESS(Status)) {
        ulValue = *((PULONG)pulValue);
        ExFreePool(pulValue);
    }
    else {
        ulValue = DefaultValue;
    }
    return ( ulValue ) ;
}


/*****************************************************************************
 * DriverEntry()
 *****************************************************************************
 * Never called.  All drivers must have one of these, so...
 */
extern "C"
NTSTATUS
DriverEntry
(
    IN      PDRIVER_OBJECT  DriverObject,
    IN      PUNICODE_STRING RegistryPath
)
{
    PAGED_CODE();

    ASSERT(! "Port Class DriverEntry was called");

    return STATUS_SUCCESS;
}

/*****************************************************************************
 * DupUnicodeString()
 *****************************************************************************
 * Duplicates a unicode string.
 */
NTSTATUS
DupUnicodeString
(
    OUT     PUNICODE_STRING *   ppUnicodeString,
    IN      PUNICODE_STRING     pUnicodeString  OPTIONAL
)
{
    PAGED_CODE();

    ASSERT(ppUnicodeString);

    NTSTATUS ntStatus = STATUS_SUCCESS;

    if (pUnicodeString)
    {
        PUNICODE_STRING pUnicodeStringNew =
            new(PagedPool,'sUcP') UNICODE_STRING;

        if (pUnicodeStringNew)
        {
            pUnicodeStringNew->Length        = pUnicodeString->Length;
            pUnicodeStringNew->MaximumLength = pUnicodeString->MaximumLength;

            if (pUnicodeString->Buffer)
            {
                pUnicodeStringNew->Buffer =
                    new(PagedPool,'sUcP')
                        WCHAR[pUnicodeString->MaximumLength / sizeof(WCHAR)];

                if (pUnicodeStringNew->Buffer)
                {
                    RtlCopyMemory
                    (
                        pUnicodeStringNew->Buffer,
                        pUnicodeString->Buffer,
                        pUnicodeString->Length
                    );

                    *ppUnicodeString = pUnicodeStringNew;
                }
                else
                {
                    ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                    delete pUnicodeStringNew;
                }
            }
            else
            {
                pUnicodeStringNew->Buffer = NULL;

                *ppUnicodeString = pUnicodeStringNew;
            }
        }
        else
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }
    }
    else
    {
        *ppUnicodeString = NULL;
    }

    return ntStatus;
}

/*****************************************************************************
 * DllInitialize()
 *****************************************************************************
 * Entry point for export library drivers.
 */
extern "C"
NTSTATUS DllInitialize(PUNICODE_STRING RegistryPath)
{

    __security_init_cookie();

    NTSTATUS  status;

    PAGED_CODE();

    _DbgPrintF(DEBUGLVL_VERBOSE,("DllInitialize"));

    DupUnicodeString( &g_RegistryPath, RegistryPath );

    WPP_INIT_TRACING(NULL, RegistryPath);

    TraceLoggingRegister(g_PortclsProvider);

    TraceLoggingWrite(g_PortclsProvider,
        "DllInitialize",
        TraceLoggingLevel(TRACE_LEVEL_INFORMATION));

    // Query the registry for the default audio buffer duration.

    gBufferDuration = GetUlongFromRegistry( CORE_AUDIO_BUFFER_DURATION_PATH,
                                            CORE_AUDIO_BUFFER_DURATION_VALUE,
                                            DEFAULT_CORE_AUDIO_BUFFER_DURATION );

    gInterruptAffinity = GetUlongFromRegistry( CORE_AUDIO_BUFFER_DURATION_PATH,
                                               L"InterruptAffinity",
                                               0 );

    // Limit duration maximum.

    if ( gBufferDuration > MAX_CORE_AUDIO_BUFFER_DURATION ) {

        gBufferDuration = MAX_CORE_AUDIO_BUFFER_DURATION;

    }

    // Limit duration minimum.

    if ( gBufferDuration < MIN_CORE_AUDIO_BUFFER_DURATION ) {

        gBufferDuration = MIN_CORE_AUDIO_BUFFER_DURATION;

    }

#if !(MIN_CORE_AUDIO_BUFFER_DURATION/1000)
#error MIN_CORE_AUDIO_BUFFER_DURATION less than 1ms not yet supported in portcls!
#endif

    gBufferDuration /= 1000;


#if kEnableDebugLogging

    if (!gPcDebugLog)
    {
        gPcDebugLog = (ULONG_PTR *)ExAllocatePoolWithTag(NonPagedPoolNx,(kNumDebugLogEntries * kNumULONG_PTRsPerEntry * sizeof(ULONG_PTR)),'lDcP');   //  'PcDl'
        if (gPcDebugLog)
        {
            RtlZeroMemory(PVOID(gPcDebugLog),kNumDebugLogEntries * kNumULONG_PTRsPerEntry * sizeof(ULONG_PTR));
        }
        gPcDebugLogIndex = 0;
    }

    DebugLog(1,0,0,0);
#endif // kEnableDebugLogging

    //
    // Initialize the global cpu-set/stream-resources tracking context.
    // If this call fails, we continue as nothing happened. A failure here only means that
    // we will not be able to affinitize the stream-resources to a specific cpu-set. 
    // On some system this may possible result in some glitching, but it is better than 
    // no audio at all.
    //
    status = StreamResourceManager_Initialize();
    if (!NT_SUCCESS(status))
    {
        DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                             PCTRACE_PNP,
                             "Portcls stream-resource manager failed to initialize, %!STATUS!",
                             status );
    }

    if (!PcVerifierInitialize())
    {
        DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                             PCTRACE_PNP,
                             "Portcls XDV extension failed to initialize." );
    }

    return STATUS_SUCCESS;
}

/*****************************************************************************
 * DllUnload()
 *****************************************************************************
 * Allow unload.
 */
extern "C"
NTSTATUS
DllUnload
(   void
)
{
    NTSTATUS  status;
    
    PAGED_CODE();

    TraceLoggingWrite(g_PortclsProvider,
        "DllUnload",
        TraceLoggingLevel(TRACE_LEVEL_INFORMATION));

    //
    // Unload/cleanup the global cpu-set/stream-resources tracking context.
    //
    status = StreamResourceManager_Cleanup();
    if (!NT_SUCCESS(status))
    {
        DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                             PCTRACE_PNP,
                             "Portcls stream-resource manager failed to cleanup, %!STATUS!",
                             status );
    }

#if kEnableDebugLogging

    if (gPcDebugLog)
    {
        ExFreePool(gPcDebugLog);
        gPcDebugLog = NULL;
    }

#endif // kEnableDebugLogging

    TraceLoggingUnregister(g_PortclsProvider);

    WPP_CLEANUP(NULL);

    if( NULL != g_RegistryPath )
    {
        if (g_RegistryPath->Buffer)
        {
            delete g_RegistryPath->Buffer;     
            g_RegistryPath->Buffer = NULL;
        }
        delete g_RegistryPath;
	    g_RegistryPath = NULL;
    }

    PcVerifierUninitialize();

    return STATUS_SUCCESS;
}

#if kEnableDebugLogging

ULONG_PTR *gPcDebugLog = NULL;
DWORD      gPcDebugLogIndex = 0;

void PcDebugLog(ULONG_PTR param1,ULONG_PTR param2,ULONG_PTR param3,ULONG_PTR param4)
{
    if (gPcDebugLog)
    {
        gPcDebugLog[(gPcDebugLogIndex * kNumULONG_PTRsPerEntry)] = param1;
        gPcDebugLog[(gPcDebugLogIndex * kNumULONG_PTRsPerEntry) + 1] = param2;
        gPcDebugLog[(gPcDebugLogIndex * kNumULONG_PTRsPerEntry) + 2] = param3;
        gPcDebugLog[(gPcDebugLogIndex * kNumULONG_PTRsPerEntry) + 3] = param4;
        if (InterlockedIncrement(PLONG(&gPcDebugLogIndex)) >= kNumDebugLogEntries)
        {
            InterlockedExchange(PLONG(&gPcDebugLogIndex), 0);
        }
    }
}

#endif // kEnableDebugLogging

/*****************************************************************************
 * DelUnicodeString()
 *****************************************************************************
 * Deletes a unicode string that was allocated using ExAllocatePool().
 */
VOID
DelUnicodeString
(
    IN      PUNICODE_STRING     pUnicodeString  OPTIONAL
)
{
    PAGED_CODE();

    if (pUnicodeString)
    {
        if (pUnicodeString->Buffer)
        {
            delete [] pUnicodeString->Buffer;
        }

        delete pUnicodeString;
    }
}

VOID
KsoNullDriverUnload(
    IN PDRIVER_OBJECT   DriverObject
    )
/*++

Routine Description:

    Default function which drivers can use when they do not have anything to do
    in their unload function, but must still allow the device to be unloaded by
    its presence.

Arguments:

    DriverObject -
        Contains the driver object for this device.

Return Values:

    Nothing.

--*/
{
    PAGED_CODE();

    _DbgPrintF(DEBUGLVL_VERBOSE,("KsoNullDriverUnload"));
    if (DriverObject->DeviceObject)
    {
        _DbgPrintF(DEBUGLVL_TERSE,("KsoNullDriverUnload  DEVICES EXIST"));
    }
}

/*****************************************************************************
 * PcInitializeAdapterDriver()
 *****************************************************************************
 * Initializes an adapter driver.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcInitializeAdapterDriver
(
    __in    PDRIVER_OBJECT      DriverObject,
    __in    PUNICODE_STRING     RegistryPathName,
    __in    PDRIVER_ADD_DEVICE  AddDevice
)
{
    PAGED_CODE();

    ASSERT(DriverObject);
    ASSERT(RegistryPathName);
    ASSERT(AddDevice);

    TraceLoggingWrite(g_PortclsProvider,
        "PcInitializeAdapterDriver",
        TraceLoggingLevel(TRACE_LEVEL_INFORMATION));

    //
    // Validate Parameters.
    //
    if (NULL == DriverObject ||
        NULL == RegistryPathName ||
        NULL == AddDevice)
    {
        TraceLoggingWrite(g_PortclsProvider,
            "PcInitializeAdapterDriver : Invalid Parameter.",
            TraceLoggingLevel(TRACE_LEVEL_WARNING));

        return STATUS_INVALID_PARAMETER;
    }

    DriverObject->DriverExtension->AddDevice           = AddDevice;
    DriverObject->DriverUnload                         = KsoNullDriverUnload;

    DriverObject->MajorFunction[IRP_MJ_PNP]            = DispatchPnp;
    DriverObject->MajorFunction[IRP_MJ_POWER]          = DispatchPower;
    DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = PcWmiSystemControl;
    DriverObject->MajorFunction[IRP_MJ_CREATE]         = DispatchCreate;

    KsSetMajorFunctionHandler(DriverObject,IRP_MJ_DEVICE_CONTROL);
    KsSetMajorFunctionHandler(DriverObject,IRP_MJ_READ);
    KsSetMajorFunctionHandler(DriverObject,IRP_MJ_WRITE);
    KsSetMajorFunctionHandler(DriverObject,IRP_MJ_FLUSH_BUFFERS);
    KsSetMajorFunctionHandler(DriverObject,IRP_MJ_CLOSE);
    KsSetMajorFunctionHandler(DriverObject,IRP_MJ_QUERY_SECURITY);
    KsSetMajorFunctionHandler(DriverObject,IRP_MJ_SET_SECURITY);

    return STATUS_SUCCESS;
}

/*****************************************************************************
 * PcDispatchIrp()
 *****************************************************************************
 * Dispatch an IRP.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcDispatchIrp
(
    __in    PDEVICE_OBJECT  pDeviceObject,
    __in    PIRP            pIrp
)
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pIrp);

    NTSTATUS ntStatus;

    //
    // Validate parameters.
    //
    if (NULL == pDeviceObject ||
        NULL == pIrp)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcDispatchIrp : Invalid Parameter"));

        ntStatus = STATUS_INVALID_PARAMETER;
        if (pIrp)
        {
            pIrp->IoStatus.Status = ntStatus;
            IoCompleteRequest( pIrp, IO_NO_INCREMENT );
        }

        return ntStatus;
    }

    switch (IoGetCurrentIrpStackLocation(pIrp)->MajorFunction)
    {
        case IRP_MJ_PNP:
            ntStatus = DispatchPnp(pDeviceObject,pIrp);
            break;
        case IRP_MJ_POWER:
            ntStatus = DispatchPower(pDeviceObject,pIrp);
            break;
        case IRP_MJ_SYSTEM_CONTROL:
            ntStatus = PcWmiSystemControl(pDeviceObject,pIrp);
            break;
        default:
            ntStatus = KsoDispatchIrp(pDeviceObject,pIrp);
            break;
    }

    return ntStatus;
}

// Disable the prefast warnings that are erroneous.
#pragma prefast(push)
#pragma prefast(disable: __WARNING_NEED_SIDE_EFFECT, "Prefast confused on behavior")
#pragma prefast(disable: __WARNING_INFERRED_IRQ_TOO_HIGH, "Prefast confused on behavior")

#pragma code_seg()
/*****************************************************************************
 * AcquireMutex()
 *****************************************************************************
 * Acquire exclusive access to the specified lock. This function has the 
 * semantics of a mutex, ie the device must be released on the same thread it 
 * was acquired from.
 */
NTSTATUS
AcquireMutex
(
    _In_ PRKMUTEX Lock
)
{
    LARGE_INTEGER liTimeout;

    liTimeout.QuadPart = 0;

    return KeWaitForSingleObject( 
                    Lock,
                    Executive,
                    KernelMode,
                    FALSE,
                    (KeGetCurrentIrql() < DISPATCH_LEVEL) ? NULL : &liTimeout );
}

#pragma code_seg()
/*****************************************************************************
 * ReleaseMutex()
 *****************************************************************************
 * Release exclusive access to the specified lock.
 */
VOID
ReleaseMutex
(
    _In_ PRKMUTEX Lock
)
{
    KeReleaseMutex(Lock,FALSE);
}

#pragma code_seg()
/*****************************************************************************
 * AcquireDevice()
 *****************************************************************************
 * Acquire exclusive access to the device. This function has the semantics of
 * a mutex, ie the device must be released on the same thread it was acquired
 * from.
 */
NTSTATUS
AcquireDevice
(
    _In_ PDEVICE_CONTEXT pDeviceContext
)
{
    NTSTATUS status;

    status = AcquireMutex(&pDeviceContext->DeviceInUse);

    DoTraceLevelMessage(
        TRACE_LEVEL_VERBOSE,
        PCTRACE_IRPSTRM,
        "Device context %p, acquired the global lock, %!STATUS!",
        pDeviceContext,
        status);
    
    return status;
}

#pragma code_seg()
/*****************************************************************************
 * ReleaseDevice()
 *****************************************************************************
 * Release exclusive access to the device.
 */
VOID
ReleaseDevice
(
    _In_ PDEVICE_CONTEXT pDeviceContext
)
{
    DoTraceLevelMessage(
        TRACE_LEVEL_VERBOSE,
        PCTRACE_IRPSTRM,
        "Device context %p, releasing the global lock",
        pDeviceContext);
    
    ReleaseMutex(&pDeviceContext->DeviceInUse);
}

#pragma code_seg()
/*****************************************************************************
 * AcquirePositionLock()
 *****************************************************************************
 * Acquire exclusive access to the position lock. 
 * This function has the semantics of a mutex, ie the lock must be released
 * on the same thread it was acquired from.
 */
NTSTATUS
AcquirePositionLock
(
    _In_ PDEVICE_CONTEXT pDeviceContext
)
{
    NTSTATUS status;

    ASSERT(pDeviceContext->pExtension);
    status = AcquireMutex(&pDeviceContext->pExtension->PositionLock);

    DoTraceLevelMessage(
        TRACE_LEVEL_VERBOSE,
        PCTRACE_IRPSTRM,
        "Device context %p, acquired the position lock, %!STATUS!",
        pDeviceContext,
        status);
    
    return status;
}

#pragma code_seg()
/*****************************************************************************
 * ReleasePositionLock()
 *****************************************************************************
 * Release exclusive access to the position lock.
 */
VOID
ReleasePositionLock
(
    _In_ PDEVICE_CONTEXT pDeviceContext
)
{
    DoTraceLevelMessage(
        TRACE_LEVEL_VERBOSE,
        PCTRACE_IRPSTRM,
        "Device context %p, releasing the position lock",
        pDeviceContext);
    
    ASSERT(pDeviceContext->pExtension);
    ReleaseMutex(&pDeviceContext->pExtension->PositionLock);
}

// Restore original prefast settings.
#pragma prefast(pop)

#pragma code_seg()
/*****************************************************************************
 * CompleteIrp()
 *****************************************************************************
 * Complete an IRP unless status is STATUS_PENDING.
 */
NTSTATUS
CompleteIrp
(
    IN      PDEVICE_CONTEXT pDeviceContext,
    IN      PIRP            pIrp,
    IN      NTSTATUS        ntStatus
)
{
    ASSERT(pDeviceContext);
    ASSERT(pIrp);

    if (ntStatus != STATUS_PENDING)
    {
        pIrp->IoStatus.Status = ntStatus;
        IoCompleteRequest(pIrp,IO_NO_INCREMENT);
        ReleaseRemoveLock( pDeviceContext, pIrp );
    }

    return ntStatus;
}

#pragma code_seg()
/*****************************************************************************
 * PcCompleteIrp()
 *****************************************************************************
 * Complete an IRP unless status is STATUS_PENDING.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcCompleteIrp
(
    __in    PDEVICE_OBJECT  pDeviceObject,
    __in    PIRP            pIrp,
    __in    NTSTATUS        ntStatus
)
{
    ASSERT(pDeviceObject);
    ASSERT(pIrp);

    if (pIrp == NULL)
        return STATUS_INVALID_PARAMETER;
      
    if (NULL == pDeviceObject ||
        NULL == pDeviceObject->DeviceExtension)
    {
        // don't know what to do, so we'll fail the IRP
        ntStatus = STATUS_INVALID_PARAMETER;
        pIrp->IoStatus.Status = ntStatus;
        IoCompleteRequest( pIrp, IO_NO_INCREMENT );
        return ntStatus;
    }

    return
        CompleteIrp
        (
            PDEVICE_CONTEXT(pDeviceObject->DeviceExtension),
            pIrp,
            ntStatus
        );
}

#pragma code_seg("PAGE")
NTSTATUS QueryReferenceBusInterface(
    IN  PDEVICE_OBJECT PnpDeviceObject,
    OUT PBUS_INTERFACE_REFERENCE BusInterface
)
/*++

Routine Description:

    Queries the bus for the standard information interface.

Arguments:

    PnpDeviceObject -
        Contains the next device object on the Pnp stack.

    PhysicalDeviceObject -
        Contains the physical device object which was passed to the FDO during
        the Add Device.

    BusInterface -
        The place in which to return the Reference interface.

Return Value:

    Returns STATUS_SUCCESS if the interface was retrieved, else an error.

--*/
{
    NTSTATUS            Status;
    KEVENT              Event;
    IO_STATUS_BLOCK     IoStatusBlock;
    PIRP                Irp;
    PIO_STACK_LOCATION  IrpStackNext;

    PAGED_CODE();
    //
    // There is no file object associated with this Irp, so the event may be located
    // on the stack as a non-object manager object.
    //
    KeInitializeEvent(&Event, NotificationEvent, FALSE);
    Irp = IoBuildSynchronousFsdRequest(
                                      IRP_MJ_PNP,
                                      PnpDeviceObject,
                                      NULL,
                                      0,
                                      NULL,
                                      &Event,
                                      &IoStatusBlock);
    if (Irp)
    {
        Irp->RequestorMode = KernelMode;
        Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
        IrpStackNext = IoGetNextIrpStackLocation(Irp);
        //
        // Create an interface query out of the Irp.
        //
        IrpStackNext->MinorFunction = IRP_MN_QUERY_INTERFACE;
        IrpStackNext->Parameters.QueryInterface.InterfaceType = (GUID*)&REFERENCE_BUS_INTERFACE;
        IrpStackNext->Parameters.QueryInterface.Size = sizeof(*BusInterface);
        IrpStackNext->Parameters.QueryInterface.Version = BUS_INTERFACE_REFERENCE_VERSION;
        IrpStackNext->Parameters.QueryInterface.Interface = (PINTERFACE)BusInterface;
        IrpStackNext->Parameters.QueryInterface.InterfaceSpecificData = NULL;
        Status = IoCallDriver(PnpDeviceObject, Irp);
        if (Status == STATUS_PENDING)
        {
            //
            // This waits using KernelMode, so that the stack, and therefore the
            // event on that stack, is not paged out.
            //
            KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
            Status = IoStatusBlock.Status;
        }
    }
    else
    {
        Status = STATUS_INSUFFICIENT_RESOURCES;
    }
    return Status;
}

#pragma code_seg()
/*****************************************************************************
 * IoTimeoutRoutine()
 *****************************************************************************
 * Called by IoTimer for timeout purposes
 */
VOID
IoTimeoutRoutine
(
    IN  PDEVICE_OBJECT  pDeviceObject,
    IN  PVOID           pContext
)
{
    ASSERT(pDeviceObject);
    ASSERT(pContext);

    KIRQL               OldIrql;
    PDEVICE_CONTEXT     pDeviceContext = PDEVICE_CONTEXT(pContext);

    if(pDeviceContext != NULL)
    {
        // grab the list spinlock
        KeAcquireSpinLock( &(pDeviceContext->TimeoutLock), &OldIrql );

        // walk the list if it's not empty
        if( !IsListEmpty( &(pDeviceContext->TimeoutList) ) )
        {
            PLIST_ENTRY         ListEntry;
            PTIMEOUTCALLBACK    pCallback;

            for( ListEntry = pDeviceContext->TimeoutList.Flink;
                 ListEntry != &(pDeviceContext->TimeoutList);
                 ListEntry = ListEntry->Flink )
            {
                pCallback = (PTIMEOUTCALLBACK) CONTAINING_RECORD( ListEntry,
                                                                  TIMEOUTCALLBACK,
                                                                  ListEntry );

                // call the callback
                pCallback->TimerRoutine(pDeviceObject,pCallback->Context);
            }
        }

        // release the spinlock
        KeReleaseSpinLock( &(pDeviceContext->TimeoutLock), OldIrql );
    }
}

/*****************************************************************************
 * StartTimer()
 *****************************************************************************
 * Start the timer if needed
 */
VOID
StartTimer
(
    IN  PDEVICE_OBJECT pDeviceObject
)
{
    ASSERT(pDeviceObject);
    if( NULL == pDeviceObject )
    {
        return;
    }

    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);
    ASSERT(pDeviceContext);
    if( NULL == pDeviceContext )
    {
        return;
    }

    KIRQL OldIrql;
    KeAcquireSpinLock( &(pDeviceContext->TimeoutLock), &OldIrql );
            
    // If timer initialization succeeded, transition it into paused or running
    if( TimerUninitialized != pDeviceContext->TimeoutState )
    {
        // if the list isn't empty, then we want to start the timer
        if(!IsListEmpty( &(pDeviceContext->TimeoutList)))
        {
#if !defined(_ARM_) && !defined(_ARM64_)
            IoStartTimer( pDeviceObject );
#endif
            pDeviceContext->TimeoutState = TimerRunning;
        }
        // the list is empty, so the timer is paused (meaning that adding a
        // callback can start it).
        else
        {
            pDeviceContext->TimeoutState = TimerPaused;
        }
    }
    KeReleaseSpinLock( &(pDeviceContext->TimeoutLock), OldIrql );
}

/*****************************************************************************
 * StopTimer()
 *****************************************************************************
 * Stop the timer
 */
VOID
StopTimer
(
    IN  PDEVICE_OBJECT pDeviceObject
)
{
    ASSERT(pDeviceObject);
    if( NULL == pDeviceObject )
    {
        return;
    }

    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);
    ASSERT(pDeviceContext);
    if( NULL == pDeviceContext )
    {
        return;
    }

    KIRQL OldIrql;
    KeAcquireSpinLock( &(pDeviceContext->TimeoutLock), &OldIrql );

    // if the timer was initialized, then force it into the stopped state, regardless
    // of the current state.
    if( TimerUninitialized != pDeviceContext->TimeoutState)
    {
#if !defined(_ARM_) && !defined(_ARM64_)
        IoStopTimer( pDeviceObject );
#endif
        pDeviceContext->TimeoutState = TimerStopped;
    }
    
    KeReleaseSpinLock( &(pDeviceContext->TimeoutLock), OldIrql );
}

/*****************************************************************************
 * ResetTimerState()
 *****************************************************************************
 * Set the timer run state to match the driver requested behavior, run if the list isn't empty, pause if it is.
 * Called whenever items are added or removed from the TimeoutList.
 * NOTE: assumes the caller is already holding the spin lock for the TimeoutList
 */
VOID
ResetTimerState
(
    IN  PDEVICE_OBJECT pDeviceObject
)
{
    ASSERT(pDeviceObject);
    if( NULL == pDeviceObject )
    {
        return;
    }

    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);
    ASSERT(pDeviceContext);
    if( NULL == pDeviceContext )
    {
        return;
    }

    // if the state is uninitialized, or stopped, do nothing, because PNP start/stop 
    // have control. Otherwise, transition to driver requested state.

    // if the list is not empty, then we need to transition to running
    if( !IsListEmpty( &(pDeviceContext->TimeoutList) ) )
    {
        if( TimerPaused == pDeviceContext->TimeoutState)
        {
#if !defined(_ARM_) && !defined(_ARM64_)
            IoStartTimer( pDeviceObject );
#endif
            pDeviceContext->TimeoutState = TimerRunning;
        }
    }
    else
    {
        if( TimerRunning == pDeviceContext->TimeoutState)
        {
#if !defined(_ARM_) && !defined(_ARM64_)
            IoStopTimer( pDeviceObject );
#endif
            pDeviceContext->TimeoutState = TimerPaused;
        }
    }
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * PcAddAdapterDevice()
 *****************************************************************************
 * Adds an adapter device.
 */
__drv_clearDoInit(__yes)
PORTCLASSAPI
NTSTATUS
NTAPI
PcAddAdapterDevice
(
    _In_    PDRIVER_OBJECT      DriverObject,
    _In_    PDEVICE_OBJECT      PhysicalDeviceObject,
    _In_    PCPFNSTARTDEVICE    StartDevice,
    _In_    ULONG               MaxObjects,
    _In_    ULONG               DeviceExtensionSize
)
{
    PAGED_CODE();

    ASSERT(DriverObject);
    ASSERT(PhysicalDeviceObject);
    ASSERT(StartDevice);
    ASSERT(MaxObjects);

    _DbgPrintF(DEBUGLVL_VERBOSE,("PcAddAdapterDevice"));

    //
    // Validate Parameters.
    //
    if (NULL == DriverObject ||
        NULL == PhysicalDeviceObject ||
        NULL == StartDevice ||
        0    == MaxObjects)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcAddAdapterDevice : Invalid Parameter"));
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Extension size may be zero or >= required size.
    //
    if (DeviceExtensionSize == 0)
    {
        DeviceExtensionSize = sizeof(DEVICE_CONTEXT);
    }
    else
    if (DeviceExtensionSize < sizeof(DEVICE_CONTEXT))
    {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Create the device object.
    //
    PDEVICE_OBJECT pDeviceObject;
    NTSTATUS ntStatus = IoCreateDeviceSecure( DriverObject,
                                              DeviceExtensionSize,
                                              NULL,
                                              FILE_DEVICE_KS,
                                              FILE_DEVICE_SECURE_OPEN |
                                                FILE_AUTOGENERATED_DEVICE_NAME,
                                              FALSE,
                                              &SDDL_PORTCLS_DEVOBJ,
                                              NULL,
                                              &pDeviceObject );
    if (NT_SUCCESS(ntStatus))
    {
        //
        // Initialize the device context.
        //

        PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

        RtlZeroMemory(pDeviceContext,DeviceExtensionSize);

        pDeviceContext->Signature = PORTCLS_DEVICE_EXTENSION_SIGNATURE;

        pDeviceContext->MaxObjects            = MaxObjects;
        pDeviceContext->PhysicalDeviceObject  = PhysicalDeviceObject;
        pDeviceContext->CreateItems           =
            new(NonPagedPoolNx,'iCcP') KSOBJECT_CREATE_ITEM[MaxObjects];
        pDeviceContext->SymbolicLinkNames     =
            new(NonPagedPoolNx,'lScP') UNICODE_STRING[MaxObjects];
        pDeviceContext->StartDevice           = StartDevice;

        // set the current power states
        pDeviceContext->CurrentDeviceState = PowerDeviceUnspecified;
        pDeviceContext->CurrentSystemState = PowerSystemWorking;

        // register the power request
        COUNTED_REASON_CONTEXT PowerRequestContext;
        PowerRequestContext.Version = DIAGNOSTIC_REASON_VERSION;
        PowerRequestContext.Flags = DIAGNOSTIC_REASON_DETAILED_STRING;
        PowerRequestContext.ResourceReasonId = IDS_POWER_AUDIO_IN_USE;
        PowerRequestContext.StringCount = 0;
        PowerRequestContext.ReasonStrings = NULL;
        if(NT_SUCCESS(RtlUnicodeStringInit(&PowerRequestContext.ResourceFileName, L"mmres.dll")))
        {
            (void)PoCreatePowerRequest(&pDeviceContext->SystemStateHandle, pDeviceObject, &PowerRequestContext);
        }

        // set device stop/remove states
        pDeviceContext->DeviceStopState    = DeviceStartPending;

        pDeviceContext->DeviceRemoveState  = DeviceAdded;


















        pDeviceContext->PauseForRebalance  = FALSE;

        //
        // Initialize list of device interfaces.
        //
        InitializeListHead(&pDeviceContext->DeviceInterfaceList);

        //
        // Initialize list of physical connections.
        //
        InitializeListHead(&pDeviceContext->PhysicalConnectionList);

        //
        // Initialize object to protect device interface and physical connection lists.
        //
        KeInitializeMutex(&pDeviceContext->ListProtect,0);

        //
        // Initialize objects for device synchronization and removal.
        //
        KeInitializeMutex(&pDeviceContext->DeviceInUse,0);
        pDeviceContext->RemoveLock = (PIO_REMOVE_LOCK)ExAllocatePoolWithTag( NonPagedPoolNx, sizeof(IO_REMOVE_LOCK), 'RLcP' );
        if( NULL == pDeviceContext->RemoveLock )
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        } else
        {
            IoInitializeRemoveLock(pDeviceContext->RemoveLock,'LRcP',0,0);
        }

        //
        // Initialize the WMILIB_CONTEXT for WMI logging
        //
        pDeviceContext->WmiLibContext = (PPC_WMI_PWMILIB_CONTEXT)ExAllocatePoolWithTag( NonPagedPoolNx, sizeof(PC_WMI_PWMILIB_CONTEXT), 'CWcP' );
        if( NULL == pDeviceContext->WmiLibContext )
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        } else
        {
            RtlZeroMemory(pDeviceContext->WmiLibContext, sizeof(WMILIB_CONTEXT));
        }

        //
        // Set the idle timeouts to the defaults.  Note that the
        // actual value will be read from the registry later.
        //
        pDeviceContext->ConservationIdleTime = DEFAULT_CONSERVATION_IDLE_TIME;
        pDeviceContext->PerformanceIdleTime = DEFAULT_PERFORMANCE_IDLE_TIME;
        pDeviceContext->IdleDeviceState = DEFAULT_IDLE_DEVICE_POWER_STATE;
        pDeviceContext->EnableIdlePowerManagement = DEFAULT_IDLE_ENABLED_STATE;

        //
        // The device global lock is the default serialization lock.
        // Check the registry for any overrides.
        //
        pDeviceContext->LockFlags = 0;
        (void)GetLockInfoFromRegistry(pDeviceContext);

        //
        // Initialize the runtime power management framework extension
        //
        pDeviceContext->RuntimePowerInfo = (PRUNTIME_POWER_INFO)ExAllocatePoolWithTag( NonPagedPoolNx, sizeof(RUNTIME_POWER_INFO), 'PRcP' );
        if( NULL == pDeviceContext->RuntimePowerInfo )
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }
        else
        {
            RtlZeroMemory(pDeviceContext->RuntimePowerInfo, sizeof(RUNTIME_POWER_INFO));
            
            pDeviceContext->RuntimePowerInfo->pDeviceObject = pDeviceObject;
            pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle = NULL;
            pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted = FALSE;
            pDeviceContext->RuntimePowerInfo->SingleComponentMultiFxStates = FALSE;
            pDeviceContext->RuntimePowerInfo->DeviceActive = FALSE;
            pDeviceContext->RuntimePowerInfo->pWorkQueueItemSystemPower = NULL;
            pDeviceContext->RuntimePowerInfo->ImmediateIdle = DEFAULT_IMMEDIATE_IDLE_STATE;
            KeInitializeMutex(&pDeviceContext->RuntimePowerInfo->RuntimeFrameworkMutex,0);
            KeInitializeEvent(&pDeviceContext->RuntimePowerInfo->RuntimeFrameworkStartedEvent, NotificationEvent, FALSE);
            KeInitializeEvent(&pDeviceContext->RuntimePowerInfo->RuntimeFrameworkUnregisteredEvent, NotificationEvent, FALSE);
            KeInitializeSpinLock(&pDeviceContext->RuntimePowerInfo->PowerStateLock); 
            pDeviceContext->RuntimePowerInfo->PowerCondition = PoAc;
            pDeviceContext->RuntimePowerInfo->PowerConditionCallbackHandle = NULL;
            pDeviceContext->RuntimePowerInfo->PowerPersonality = PersonalityTypicalPowerSavings;
            pDeviceContext->RuntimePowerInfo->PowerPersonalityCallbackHandle = NULL;
            pDeviceContext->RuntimePowerInfo->LowPowerEpoch = FALSE;
            pDeviceContext->RuntimePowerInfo->LowPowerEpochCallbackHandle = NULL;
            pDeviceContext->RuntimePowerInfo->RuntimeSystemWorking = TRUE;

            //
            // Get runtime power overrides.
            //
            (void)GetFxIdleInfoFromRegistry(pDeviceContext, &pDeviceContext->RuntimePowerInfo->SingleComponentMultiFxStates);
        }
        
        //
        // Allocate VerifierContext
        //
        if( NT_SUCCESS(ntStatus) )
        {
            if (PC_VERIFIER_ON && MmIsDriverVerifying(DriverObject))
            {
                pDeviceContext->VerifierContext = NULL;

                XDV_ALLOCATE_CONTEXT(
                    pDeviceObject, 
                    DriverObject, 
                    &pDeviceContext->VerifierContext);
            }
        }

        //
        // Register for power callbacks when the system power personality, AC/DC, or
        // low power epoch settings change.
        //
        if( NT_SUCCESS(ntStatus) )
        {
            ntStatus = PoRegisterPowerSettingCallback(
                pDeviceObject,
                &GUID_POWERSCHEME_PERSONALITY,
                PcPowerSettingCallback,
                pDeviceContext,
                &pDeviceContext->RuntimePowerInfo->PowerPersonalityCallbackHandle);
        }

        if( NT_SUCCESS(ntStatus) )
        {
            ntStatus = PoRegisterPowerSettingCallback(
                pDeviceObject,
                &GUID_ACDC_POWER_SOURCE,
                PcPowerSettingCallback,
                pDeviceContext,
                &pDeviceContext->RuntimePowerInfo->PowerConditionCallbackHandle);
        }

        if( NT_SUCCESS(ntStatus) )
        {
            ntStatus = PoRegisterPowerSettingCallback(
                pDeviceObject,
                &GUID_LOW_POWER_EPOCH,
                PcLowPowerEpochCallback,
                pDeviceObject,
                &pDeviceContext->RuntimePowerInfo->LowPowerEpochCallbackHandle);
        }

        // Create and initialize the driver object's synchronized DMA queue.
        pDeviceContext->pExtension = new DCExtension;
        if (pDeviceContext->pExtension)
        {
            KeInitializeMutex(&pDeviceContext->pExtension->PositionLock, 0);
        }
        else
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }

        if( NT_SUCCESS(ntStatus) )
        {
            if( ( !pDeviceContext->CreateItems ) || ( !pDeviceContext->SymbolicLinkNames) )
            {
                ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            } else
            {

                //
                // Initialize the IoTimer - Only on non-ARM
                //
                InitializeListHead(&pDeviceContext->TimeoutList);
                KeInitializeSpinLock(&pDeviceContext->TimeoutLock);
                pDeviceContext->TimeoutState = TimerUninitialized;
#if !defined(_ARM_) && !defined(_ARM64_)
                if( NT_SUCCESS(IoInitializeTimer(pDeviceObject,IoTimeoutRoutine,pDeviceContext)) )
                {
                    pDeviceContext->TimeoutState = TimerStopped;
                }
#endif // !defined(_ARM_) && !defined(_ARM64_)

                //
                // Allocate the KS device header
                //
                ntStatus = KsAllocateDeviceHeader( &pDeviceContext->pDeviceHeader,
                                                   MaxObjects,
                                                   pDeviceContext->CreateItems );
                if( NT_SUCCESS(ntStatus) )
                {
                    pDeviceContext->NextDeviceInStack = IoAttachDeviceToDeviceStack( pDeviceObject,
                                                                                     PhysicalDeviceObject );

                    if (! pDeviceContext->NextDeviceInStack)
                    {
                        // free the KS device header
                        KsFreeDeviceHeader( pDeviceContext->pDeviceHeader );
                        pDeviceContext->pDeviceHeader = NULL;

                        ntStatus = STATUS_UNSUCCESSFUL;
                    }
                    else
                    {
                        BUS_INTERFACE_REFERENCE BusInterface;

                        KsSetDevicePnpAndBaseObject(pDeviceContext->pDeviceHeader,
                                                    pDeviceContext->NextDeviceInStack,
                                                    pDeviceObject );

                        //
                        // Here we try to detect the case where we really aren't
                        // an audio miniport, but rather helping out an swenum
                        // dude like dmusic. In the later case, we disallow
                        // (nonsensical) registration.
                        //
                        pDeviceContext->AllowRegisterDeviceInterface=TRUE;
                        if (NT_SUCCESS(QueryReferenceBusInterface(pDeviceContext->NextDeviceInStack,&BusInterface)))
                        {
                            BusInterface.Interface.InterfaceDereference( BusInterface.Interface.Context );
                            pDeviceContext->AllowRegisterDeviceInterface=FALSE;
                        }
                    }


                    pDeviceObject->Flags |= DO_DIRECT_IO;
                    pDeviceObject->Flags |= DO_POWER_PAGABLE;
                    pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
                }
            }
        }

        if (!NT_SUCCESS(ntStatus))
        {
            if (pDeviceContext->CreateItems)
            {
                delete [] pDeviceContext->CreateItems;
            }

            if (pDeviceContext->SymbolicLinkNames)
            {
                delete [] pDeviceContext->SymbolicLinkNames;
            }

            if  (pDeviceContext->pExtension)
            {
                delete pDeviceContext->pExtension;
            }

            if (pDeviceContext->RuntimePowerInfo)
            {
                //
                // Unregister the power condition setting callback if it was previously registered.
                //
                if (pDeviceContext->RuntimePowerInfo->PowerConditionCallbackHandle)
                {
                    (void)PoUnregisterPowerSettingCallback(pDeviceContext->RuntimePowerInfo->PowerConditionCallbackHandle);
                    pDeviceContext->RuntimePowerInfo->PowerConditionCallbackHandle = NULL;
                }

                //
                // Unregister the power personality setting callback if it was previously registered.
                //
                if (pDeviceContext->RuntimePowerInfo->PowerPersonalityCallbackHandle)
                {
                    (void)PoUnregisterPowerSettingCallback(pDeviceContext->RuntimePowerInfo->PowerPersonalityCallbackHandle);
                    pDeviceContext->RuntimePowerInfo->PowerPersonalityCallbackHandle = NULL;
                }

                //
                // Unregister the low power epoch setting callback if it was previously registered.
                //
                if (pDeviceContext->RuntimePowerInfo->LowPowerEpochCallbackHandle)
                {
                    (void)PoUnregisterPowerSettingCallback(pDeviceContext->RuntimePowerInfo->LowPowerEpochCallbackHandle);
                    pDeviceContext->RuntimePowerInfo->LowPowerEpochCallbackHandle = NULL;
                }

                ExFreePool(pDeviceContext->RuntimePowerInfo);
                pDeviceContext->RuntimePowerInfo = NULL;
            }

            if (pDeviceContext->WmiLibContext)
            {
                ExFreePool( pDeviceContext->WmiLibContext );
                pDeviceContext->WmiLibContext = NULL;
            }

            if (pDeviceContext->RemoveLock)
            {
                ExFreePool( pDeviceContext->RemoveLock );
                pDeviceContext->RemoveLock = NULL;
            }

            IoDeleteDevice(pDeviceObject);
        }
        else
        {
            InternalPcWmiRegisterProvider(pDeviceObject);
            EventRegisterPortcls();
        }
    }
    else
    {
        _DbgPrintF(DEBUGLVL_TERSE,("PcAddAdapterDevice IoCreateDevice failed with status 0x%08x",ntStatus));
    }

    return ntStatus;
}

#pragma code_seg()

/*****************************************************************************
 * ForwardIrpCompletionRoutine()
 *****************************************************************************
 * Completion routine for ForwardIrp.
 */
static
NTSTATUS
ForwardIrpCompletionRoutine
(
    IN      PDEVICE_OBJECT  DeviceObject,
    IN      PIRP            Irp,
    IN      PVOID           Context
)
{
    ASSERT(Irp);
    ASSERT(Context);

    if(Context)
    {
        KeSetEvent((PKEVENT) Context,0,FALSE);
    }
    return STATUS_MORE_PROCESSING_REQUIRED;
}

/*****************************************************************************
 * ForwardIrpAsynchronous()
 *****************************************************************************
 * Forward a PnP IRP to the PDO.  The IRP is completed at this level
 * regardless of the outcome, this function returns immediately regardless of
 * whether the IRP is pending in the lower driver, and
 * ReleaseRemoveLock() is called in all cases.
 */
NTSTATUS
ForwardIrpAsynchronous
(
    IN      PDEVICE_CONTEXT pDeviceContext,
    IN      PIRP            pIrp
)
{
    ASSERT(pDeviceContext);
    ASSERT(pIrp);

    NTSTATUS ntStatus;

    if (pDeviceContext->DeviceRemoveState == DeviceRemoved)
    {
        _DbgPrintF(DEBUGLVL_VERBOSE,("ForwardIrpAsynchronous delete pending"));
        ntStatus = CompleteIrp(pDeviceContext,pIrp,STATUS_DELETE_PENDING);
    }
    else
    {
        _DbgPrintF(DEBUGLVL_VERBOSE,("ForwardIrpAsynchronous"));

        PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(pIrp);

        IoSkipCurrentIrpStackLocation(pIrp);

        if (irpSp->MajorFunction == IRP_MJ_POWER)
        {
            ntStatus = PoCallDriver(pDeviceContext->NextDeviceInStack,pIrp);
        }
        else
        {
            ntStatus = IoCallDriver(pDeviceContext->NextDeviceInStack,pIrp);
        }
    }

    ReleaseRemoveLock( pDeviceContext, pIrp );

    return ntStatus;
}

/*****************************************************************************
 * ForwardIrpSynchronous()
 *****************************************************************************
 * Forward a PnP IRP to the PDO.  The IRP is not completed at this level,
 * this function does not return until the lower driver has completed the IRP,
 * and ReleaseRemoveLock() is not called.
 */
NTSTATUS
ForwardIrpSynchronous
(
    IN      PDEVICE_CONTEXT pDeviceContext,
    IN      PIRP            pIrp
)
{
    ASSERT(pDeviceContext);
    ASSERT(pIrp);

    NTSTATUS ntStatus;

    if (pDeviceContext->DeviceRemoveState == DeviceRemoved)
    {
        ntStatus = STATUS_DELETE_PENDING;

        _DbgPrintF(DEBUGLVL_VERBOSE,("ForwardIrpSynchronous delete pending"));
    }
    else
    {
        _DbgPrintF(DEBUGLVL_VERBOSE,("ForwardIrpSynchronous"));

        PIO_STACK_LOCATION irpStackPointer;

        //
        // If the IRP was created in a miniport we get the next
        // irp stack location, otherwise the current irp stack
        // location.
        //
        if (pIrp->CurrentLocation >= pIrp->StackCount + 1)
        {
            irpStackPointer = IoGetNextIrpStackLocation (pIrp);
        }
        else
        {
            irpStackPointer = IoGetCurrentIrpStackLocation(pIrp);

            // setup next stack location
            IoCopyCurrentIrpStackLocationToNext( pIrp );
        }

        KEVENT kEvent;
        KeInitializeEvent(&kEvent,NotificationEvent,FALSE);

        IoSetCompletionRoutine
        (
            pIrp,
            ForwardIrpCompletionRoutine,
            &kEvent,                        // Context
            TRUE,                           // InvokeOnSuccess
            TRUE,                           // InvokeOnError
            TRUE                            // InvokeOnCancel
        );

        if (irpStackPointer->MajorFunction == IRP_MJ_POWER)
        {
            ntStatus = PoCallDriver(pDeviceContext->NextDeviceInStack,pIrp);
        }
        else
        {
            ntStatus = IoCallDriver(pDeviceContext->NextDeviceInStack,pIrp);
        }

        if (ntStatus == STATUS_PENDING)
        {
            LARGE_INTEGER Timeout = RtlConvertLongToLargeInteger( 0L );

            _DbgPrintF(DEBUGLVL_VERBOSE,("ForwardIrpSynchronous pending..."));
            KeWaitForSingleObject
            (
                &kEvent,
                Suspended,
                KernelMode,
                FALSE,
                (KeGetCurrentIrql() < DISPATCH_LEVEL) ? NULL : &Timeout
            );
            ntStatus = pIrp->IoStatus.Status;
            _DbgPrintF(DEBUGLVL_VERBOSE,("ForwardIrpSynchronous complete"));
        }
    }
    ASSERT(ntStatus != STATUS_PENDING);
    return ntStatus;
}

#pragma code_seg("PAGE")

/*****************************************************************************
 * PcForwardIrpSynchronous()
 *****************************************************************************
 * Forward a PnP IRP to the PDO.  The IRP is not completed at this level,
 * this function does not return until the lower driver has completed the IRP,
 * and ReleaseRemoveLock() is not called.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcForwardIrpSynchronous
(
    __in    PDEVICE_OBJECT  DeviceObject,
    __in    PIRP            Irp
)
{
    ASSERT(DeviceObject);
    ASSERT(Irp);

    PAGED_CODE();

    //
    // Validate Parameters.
    //
    if (Irp == NULL)
        return STATUS_INVALID_PARAMETER;

    if (NULL == DeviceObject)
    {
        // don't know what to do, so we'll fail the IRP
        Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
        IoCompleteRequest( Irp, IO_NO_INCREMENT );
        return STATUS_INVALID_PARAMETER;
    }

    return
        ForwardIrpSynchronous
        (
            PDEVICE_CONTEXT(DeviceObject->DeviceExtension),
            Irp
        );
}

/*****************************************************************************
 * DispatchSystemControl()
 *****************************************************************************
 * Device objects that do not handle this IRP should leave it untouched.
 */
NTSTATUS
PcDispatchSystemControl
(
    IN      PDEVICE_OBJECT  pDeviceObject,
    IN      PIRP            pIrp
)
{
    ASSERT(pDeviceObject);
    ASSERT(pIrp);

    PAGED_CODE();

    _DbgPrintF(DEBUGLVL_VERBOSE,("DispatchSystemControl"));

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    NTSTATUS ntStatus = PcValidateDeviceContext(pDeviceContext, pIrp);
    if (!NT_SUCCESS(ntStatus))
    {












        ntStatus = pIrp->IoStatus.Status;
        IoCompleteRequest( pIrp, IO_NO_INCREMENT );
        return ntStatus;
    }

    ntStatus = AcquireRemoveLock( pDeviceContext, pIrp );
    if(!NT_SUCCESS(ntStatus))
    {
        pIrp->IoStatus.Status = ntStatus;
        IoCompleteRequest( pIrp, IO_NO_INCREMENT );
        return ntStatus;
    }

    return ForwardIrpAsynchronous(pDeviceContext,pIrp);
}

/*****************************************************************************
 * PnpStopDevice()
 *****************************************************************************
 * Stop the device.
 */
NTSTATUS
PnpStopDevice
(
    IN      PDEVICE_OBJECT  pDeviceObject,
    IN      PNPSTOP_STYLE   StopStyle
)
{



    //  The lists and items being deleted here
    //  are ceated at device start time.  Device start and stop IRPs can only 
    //  come from the configuration manager, and it will never issue them 
    //  simultaneously.  In addition, no other IRPs can access these structures
    //  while the start or stop irp is executing- all of this is by design.  So
    //  no synchronization of list access is needed.  This was verified by the
    //  owners of the CM code.
    PAGED_CODE();

    EventWritePORTCLS_DEVICE_STOP_ENTER(NULL);

    ASSERT(pDeviceObject);
    UNREFERENCED_PARAMETER(StopStyle);

    _DbgPrintF(DEBUGLVL_VERBOSE,("PnpStopDevice stopping"));

    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    ASSERT(pDeviceContext);
    ASSERT(pDeviceContext->StartDevice);

    pDeviceContext->PendCreates = TRUE;
























    
    pDeviceContext->DeviceStopState = DeviceStopped;

    //
    // Unregister with the runtime power management framework.
    //
    UnregisterRuntimeManagement( pDeviceContext );

#if !defined(_ARM_) && !defined(_ARM64_)
    // Stop the IoTimeout timer in case the miniport didn't unregister
    // it's callback.  Note that it's safe to call IoStopTimer while the
    // timer is stopped.
    StopTimer( pDeviceObject );
#endif // !defined(_ARM_) && !defined(_ARM64_)

    POWER_STATE newPowerState;

    newPowerState.DeviceState = PowerDeviceD3;
    PoSetPowerState(pDeviceObject,
                    DevicePowerState,
                    newPowerState
                    );
    pDeviceContext->CurrentDeviceState = PowerDeviceD3;

    //
    // Delete all physical connections.
    //
    KeWaitForSingleObject(&pDeviceContext->ListProtect,Executive,KernelMode,FALSE,NULL);
    while (! IsListEmpty(&pDeviceContext->PhysicalConnectionList))
    {
        PPHYSICALCONNECTION pPhysicalConnection =
            (PPHYSICALCONNECTION)RemoveHeadList(&pDeviceContext->PhysicalConnectionList);

        ASSERT(pPhysicalConnection);
        ASSERT(pPhysicalConnection->FromSubdevice);
        ASSERT(pPhysicalConnection->ToSubdevice);

        if (pPhysicalConnection->FromSubdevice)
        {
            pPhysicalConnection->FromSubdevice->Release();
        }
        if (pPhysicalConnection->ToSubdevice)
        {
            pPhysicalConnection->ToSubdevice->Release();
        }
        if (pPhysicalConnection->FromString)
        {
            DelUnicodeString(pPhysicalConnection->FromString);
        }
        if (pPhysicalConnection->ToString)
        {
            DelUnicodeString(pPhysicalConnection->ToString);
        }

        delete pPhysicalConnection;
    }
    KeReleaseMutex(&pDeviceContext->ListProtect,FALSE);

    //
    // Disable and delete all the device interfaces.
    //
    KeWaitForSingleObject(&pDeviceContext->ListProtect,Executive,KernelMode,FALSE,NULL);
    while (! IsListEmpty(&pDeviceContext->DeviceInterfaceList))
    {
        PDEVICEINTERFACE pDeviceInterface =
            (PDEVICEINTERFACE)
                RemoveHeadList(&pDeviceContext->DeviceInterfaceList);

        ASSERT(pDeviceInterface);
        ASSERT(pDeviceInterface->SymbolicLinkName.Buffer);

        NTSTATUS ntStatus = STATUS_SUCCESS;
        if (pDeviceContext->AllowRegisterDeviceInterface)
        {
            ntStatus = IoSetDeviceInterfaceState(&pDeviceInterface->SymbolicLinkName,FALSE);
        }

#if DBG
        if (NT_SUCCESS(ntStatus))
        {
            _DbgPrintF(DEBUGLVL_VERBOSE,("PnpStopDevice disabled device interface %S",pDeviceInterface->SymbolicLinkName.Buffer));
        }
        else
        {
            _DbgPrintF(DEBUGLVL_TERSE,("PnpStopDevice failed to disable device interface %S (0x%08x)",pDeviceInterface->SymbolicLinkName.Buffer,ntStatus));
        }
#endif

        RtlFreeUnicodeString(&pDeviceInterface->SymbolicLinkName);

        delete pDeviceInterface;
    }
    KeReleaseMutex(&pDeviceContext->ListProtect,FALSE);

    //
    // Clear the symbolic link names table.
    //
    RtlZeroMemory
        (   pDeviceContext->SymbolicLinkNames
            ,   sizeof(UNICODE_STRING) * pDeviceContext->MaxObjects
        );

    //
    // Unload each subdevice for this device.
    //
    PKSOBJECT_CREATE_ITEM pKsObjectCreateItem =
        pDeviceContext->CreateItems;
    for
             (   ULONG ul = pDeviceContext->MaxObjects;
         ul--;
             pKsObjectCreateItem++
             )
    {
        if (pKsObjectCreateItem->Create)
        {
            //
            // Zero the create function so we won't get creates.
            //
            pKsObjectCreateItem->Create = NULL;

            //
            // Release the subdevice referenced by this create item.
            //
            ASSERT(pKsObjectCreateItem->Context);
            PSUBDEVICE(pKsObjectCreateItem->Context)->ReleaseChildren();
            PSUBDEVICE(pKsObjectCreateItem->Context)->Release();
        }
    }

    //
    // If the Adapter registered a Power Management interface
    //
    if( NULL != pDeviceContext->pAdapterPower )
    {
        // Release it
        pDeviceContext->pAdapterPower->Release();
        pDeviceContext->pAdapterPower = NULL;
    }
    
    //
    // If the Adapter registered a Pnp Management interface, release it
    //
    if (NULL != pDeviceContext->RuntimePowerInfo &&
        NULL != pDeviceContext->RuntimePowerInfo->pAdapterPnp)
    {
        // Release it
        pDeviceContext->RuntimePowerInfo->pAdapterPnp->Release();
        pDeviceContext->RuntimePowerInfo->pAdapterPnp = NULL;
    }

    EventWritePORTCLS_DEVICE_STOP_EXIT(NULL,STATUS_SUCCESS);

    _DbgPrintF(DEBUGLVL_VERBOSE,("PnpStopDevice exiting"));
    return STATUS_SUCCESS;
}

/*****************************************************************************
 * PnpStartDevice()
 *****************************************************************************
 * Start the device in the PnP style.
 */
QUEUED_CALLBACK_RETURN
PnpStartDevice
(
    IN  PDEVICE_OBJECT  pDeviceObject,
    IN  PVOID           pNotUsed
)
{
    PAGED_CODE();

    EventWritePORTCLS_DEVICE_START_ENTER(NULL);

    ASSERT(pDeviceObject);
    _DbgPrintF(DEBUGLVL_VERBOSE,("PnpStartDevice starting (0x%p)",pDeviceObject));

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    ASSERT(pDeviceContext);
    ASSERT(pDeviceContext->StartDevice);
    ASSERT(pDeviceContext->DeviceStopState != DeviceStarted);
    ASSERT(pDeviceContext->DeviceRemoveState == DeviceAdded);

    pDeviceContext->DeviceStopState = DeviceStartPending;

    PIRP pIrp = pDeviceContext->IrpStart;
    ASSERT(pIrp);

    PIO_STACK_LOCATION pIrpStack =
        IoGetCurrentIrpStackLocation(pIrp);

    //
    // Encapsulate the resource lists.
    //
    PRESOURCELIST pResourceList;
    NTSTATUS ntStatus;
    BOOL bCompletePendedIrps=FALSE;

    // in case there is no resource list in IO_STACK_LOCATION, PcNewResourceList
    // just creates an empty resource list.
    ntStatus = PcNewResourceList
               (
               &pResourceList,
               NULL,
               PagedPool,
               pIrpStack->Parameters.StartDevice.AllocatedResourcesTranslated,
               pIrpStack->Parameters.StartDevice.AllocatedResources
               );

    if (NT_SUCCESS(ntStatus))
    {
        ASSERT(pResourceList);

        //
        // Acquire the device to prevent creates during interface registration.
        //
        AcquireDevice(pDeviceContext);

        //
        // Register for runtime power management. 
        // Default is single component/single state.
        //
        if (pDeviceContext->RuntimePowerInfo && !pDeviceContext->RuntimePowerInfo->SingleComponentMultiFxStates)
        {
            ntStatus = RegisterRuntimeManagement(pDeviceObject);
        }

        if (NT_SUCCESS(ntStatus))
        {
            //
            // Start the adapter.
            //
            ntStatus = pDeviceContext->StartDevice(pDeviceObject,
                                                   pIrp,
                                                   pResourceList);
            ASSERT(ntStatus != STATUS_PENDING);

            pResourceList->Release();

            pDeviceContext->DeviceStopState = DeviceStarted;

            if (NT_SUCCESS(ntStatus))
            {
                // Start is always an implicit power up
                POWER_STATE newPowerState;

                pDeviceContext->CurrentDeviceState = PowerDeviceD0;
                newPowerState.DeviceState = PowerDeviceD0;
                PoSetPowerState(pDeviceObject,
                                DevicePowerState,
                                newPowerState
                                );

#if !defined(_ARM_) && !defined(_ARM64_)
                // Start the IoTimeout timer if a callback has already been registered
                // by the miniport.
                StartTimer( pDeviceObject );
#endif // !defined(_ARM_) && !defined(_ARM64_)

                //
                // For single component/multi-fx states, register after the miniport had an opportunity
                // to call the PcAssignPowerFrameworkSettings for the device.
                //
                if (pDeviceContext->RuntimePowerInfo && pDeviceContext->RuntimePowerInfo->SingleComponentMultiFxStates)
                {
                    ntStatus = RegisterRuntimeManagement(pDeviceObject);
                }
            }


            if (!NT_SUCCESS(ntStatus))
            {
                _DbgPrintF(DEBUGLVL_TERSE,("PnpStartDevice adapter failed to start (0x%08x)",ntStatus));

                // stop the device (note: this will set DeviceStopState back to DeviceStopped)
                PnpStopDevice(pDeviceObject, STOPSTYLE_DISABLE);
            }
            else 
            {
                // allow create
                pDeviceContext->PendCreates = FALSE;

                // Can't actually complete pended irps until we call ReleaseDevice, or we might deadlock
                bCompletePendedIrps=TRUE;
            }
        }
        
        //
        // Release the device to allow creates.
        //
        ReleaseDevice(pDeviceContext);

        // Now we can complete pended irps
        if (bCompletePendedIrps)
        {
            CompletePendedIrps( pDeviceObject,
                                pDeviceContext,
                                EMPTY_QUEUE_AND_PROCESS );
        }
    }
    else
    {
        _DbgPrintF(DEBUGLVL_TERSE,("PnpStartDevice failed to create resource list object (0x%08x)",ntStatus));
    }

    CompleteIrp(pDeviceContext,pIrp,ntStatus);

    EventWritePORTCLS_DEVICE_START_EXIT(NULL,ntStatus);

    _DbgPrintF(DEBUGLVL_VERBOSE,("PnPStartDevice completing with 0x%X status for 0x%p",ntStatus,pDeviceObject));
    return QUEUED_CALLBACK_FREE;
}

/*****************************************************************************
 * PnpRemoveDevice()
 *****************************************************************************
 * Dispatch IRP_MJ_PNP/IRP_MN_REMOVE_DEVICE.
 */
NTSTATUS
PnpRemoveDevice
(
    IN      PDEVICE_OBJECT  pDeviceObject
)
{
    PAGED_CODE();

    ASSERT(pDeviceObject);

    _DbgPrintF(DEBUGLVL_VERBOSE,("PnpRemoveDevice"));

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    ASSERT( pDeviceContext );

    //
    // If verifier is enabled, clean up the context
    //
    if (IsVerifierEnabled(pDeviceContext->VerifierContext))
    {

        XdvPcPnpRemoveDevice(pDeviceObject);
        
        XDV_FREE_CONTEXT(pDeviceContext->VerifierContext);

    }

    if(pDeviceContext->SystemStateHandle)
    {
        PoDeletePowerRequest(pDeviceContext->SystemStateHandle);
        pDeviceContext->SystemStateHandle = NULL;
    }

    pDeviceContext->DeviceRemoveState = DeviceRemoved;

    _DbgPrintF(DEBUGLVL_VERBOSE,("PnpRemoveDevice pending irp count is 0"));

    IoDetachDevice(pDeviceContext->NextDeviceInStack);

    _DbgPrintF(DEBUGLVL_VERBOSE,("PnpRemoveDevice detached"));

    if (pDeviceContext->CreateItems)
    {
        delete [] pDeviceContext->CreateItems;
    }

    if (pDeviceContext->SymbolicLinkNames)
    {
        delete [] pDeviceContext->SymbolicLinkNames;
    }

    if (pDeviceContext->pExtension)
    {
        delete pDeviceContext->pExtension;
    }

    if (pDeviceContext->RemoveLock)
    {
        ExFreePool( pDeviceContext->RemoveLock );
        pDeviceContext->RemoveLock = NULL;
    }

    PDRIVER_OBJECT pDriverObject = pDeviceObject->DriverObject;

    if (pDeviceObject->NextDevice)
    {
        _DbgPrintF(DEBUGLVL_VERBOSE,("PnpRemoveDevice there is a next device"));
    }

    if (pDriverObject->DeviceObject != pDeviceObject)
    {
        _DbgPrintF(DEBUGLVL_VERBOSE,("PnpRemoveDevice there is a previous device"));
    }

    InternalPcWmiUnregisterProvider(pDeviceObject);
    EventUnregisterPortcls();

    if (pDeviceContext->WmiLibContext)
    {
        if (pDeviceContext->WmiLibContext->MiniPortWmiLibContext)
        {
		    ExFreePool(pDeviceContext->WmiLibContext->PcWmiLibContext.GuidList);
            pDeviceContext->WmiLibContext->PcWmiLibContext.GuidList = NULL;
            pDeviceContext->WmiLibContext->MiniPortWmiLibContext = NULL;
        }
		ExFreePool(pDeviceContext->WmiLibContext);
		pDeviceContext->WmiLibContext = NULL;
    }

    if (pDeviceContext->RuntimePowerInfo)
    {
        //
        // Unregister the power condition setting callback if it was previously registered.
        //
        if (pDeviceContext->RuntimePowerInfo->PowerConditionCallbackHandle)
        {
            (void)PoUnregisterPowerSettingCallback(pDeviceContext->RuntimePowerInfo->PowerConditionCallbackHandle);
            pDeviceContext->RuntimePowerInfo->PowerConditionCallbackHandle = NULL;
        }

        //
        // Unregister the power personality setting callback if it was previously registered.
        //
        if (pDeviceContext->RuntimePowerInfo->PowerPersonalityCallbackHandle)
        {
            (void)PoUnregisterPowerSettingCallback(pDeviceContext->RuntimePowerInfo->PowerPersonalityCallbackHandle);
            pDeviceContext->RuntimePowerInfo->PowerPersonalityCallbackHandle = NULL;
        }

        //
        // Unregister the low power epoch setting callback if it was previously registered.
        //
        if (pDeviceContext->RuntimePowerInfo->LowPowerEpochCallbackHandle)
        {
            (void)PoUnregisterPowerSettingCallback(pDeviceContext->RuntimePowerInfo->LowPowerEpochCallbackHandle);
            pDeviceContext->RuntimePowerInfo->LowPowerEpochCallbackHandle = NULL;
        }

        //
        // Free Single-Component/Multi-State buffer.
        //
        if (pDeviceContext->RuntimePowerInfo->SingleComponentMultiFxStates && 
            pDeviceContext->RuntimePowerInfo->ScMs.PoFxComponent) 
        {
            ExFreePoolWithTag(pDeviceContext->RuntimePowerInfo->ScMs.PoFxComponent->IdleStates, 'MRcP');
            pDeviceContext->RuntimePowerInfo->ScMs.PoFxComponent = NULL;
        }

        ExFreePool(pDeviceContext->RuntimePowerInfo);
        pDeviceContext->RuntimePowerInfo = NULL;
    }

    IoDeleteDevice(pDeviceObject);
    _DbgPrintF(DEBUGLVL_VERBOSE,("PnpRemoveDevice device deleted"));

    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
/*****************************************************************************
* GetAdapterRebalanceType()
*****************************************************************************
* Get the rebalance type supported by the miniport.
*
*/
PC_REBALANCE_TYPE
GetAdapterRebalanceType
(
    _In_  PDEVICE_CONTEXT   pDeviceContext   
)
{
    PAGED_CODE();

    ASSERT(pDeviceContext);

    NTSTATUS            status          = STATUS_SUCCESS;
    PVOID               pnpAdapter      = NULL;
    PC_REBALANCE_TYPE   rebalanceType   = PcRebalanceNotSupported;

    DoTraceLevelMessage(TRACE_LEVEL_VERBOSE,
                        PCTRACE_PNP,
                        "Device context %p",
                        pDeviceContext);

    if (NULL != pDeviceContext && 
        NULL != pDeviceContext->RuntimePowerInfo &&
        NULL != pDeviceContext->RuntimePowerInfo->pAdapterPnp)
    {
        status = pDeviceContext->RuntimePowerInfo->pAdapterPnp->QueryInterface
            (
            IID_IAdapterPnpManagement,
            &pnpAdapter
            );

        if (NT_SUCCESS(status))
        {
            // We got an IAdapterPnpManagement interface.
            rebalanceType = PADAPTERPNPMANAGEMENT(pnpAdapter)->GetSupportedRebalanceType();
            PUNKNOWN(pnpAdapter)->Release();
        }
        
        pnpAdapter = NULL;
    }

    DoTraceLevelMessage(TRACE_LEVEL_INFORMATION,
                        PCTRACE_PNP,
                        "Device context %p, adapter rebalance type %d",
                        pDeviceContext,
                        rebalanceType);

    return rebalanceType;
}

#pragma code_seg("PAGE")
/*****************************************************************************
* PnpNotifyAdapter()
*****************************************************************************
* Called to notify the device's adapter about the pnp events.
*
*/
void
PnpNotifyAdapter
(
    _In_  PDEVICE_CONTEXT           pDeviceContext,
    _In_  PC_PNPNOTIFICATION_TYPE   PnpNotification    
)
{
    PAGED_CODE();

    ASSERT(pDeviceContext);

    NTSTATUS    status      = STATUS_SUCCESS;
    PVOID       pnpAdapter  = NULL;

    DoTraceLevelMessage(TRACE_LEVEL_VERBOSE,
                        PCTRACE_PNP,
                        "Device context %p, PNP notification %d",
                        pDeviceContext,
                        PnpNotification);

    if (NULL != pDeviceContext && 
        NULL != pDeviceContext->RuntimePowerInfo &&
        NULL != pDeviceContext->RuntimePowerInfo->pAdapterPnp)
    {
        status = pDeviceContext->RuntimePowerInfo->pAdapterPnp->QueryInterface(
                                                IID_IAdapterPnpManagement,
                                                &pnpAdapter);
        if (NT_SUCCESS(status))
        {
            // We got an IAdapterPnpManagement interface.
            switch (PnpNotification)
            {
            case PcPnpStop:
                PADAPTERPNPMANAGEMENT(pnpAdapter)->PnpStop();
                break;
            case PcPnpQueryStop:
                //
                // This is a notification, i.e., we are not asking the 
                // adapter for permission to stop.
                //
                PADAPTERPNPMANAGEMENT(pnpAdapter)->PnpQueryStop();
                break;
            case PcPnpCancelStop:
                PADAPTERPNPMANAGEMENT(pnpAdapter)->PnpCancelStop();
                break;
            default:
                ASSERT(FALSE);
                break;
            }
            
            PUNKNOWN(pnpAdapter)->Release();
            pnpAdapter = NULL;
        }
    }
}

#pragma code_seg("PAGE")
/*****************************************************************************
* PnpNotifySubdevices()
*****************************************************************************
* Called to notify the device's adapter about the pnp events.
* Caller must own the global lock.
*
*/
NTSTATUS
PnpNotifySubdevices
(
    _In_  PDEVICE_CONTEXT           pDeviceContext,
    _In_  PC_PNPNOTIFICATION_TYPE   PnpNotification 
)
{
    PAGED_CODE();

    ASSERT(pDeviceContext);

    NTSTATUS status = STATUS_SUCCESS;

    DoTraceLevelMessage(TRACE_LEVEL_VERBOSE,
                        PCTRACE_PNP,
                        "Device context %p",
                        pDeviceContext);

    PKSOBJECT_CREATE_ITEM createItem = pDeviceContext->CreateItems;

    // iterate through the subdevices
    for (ULONG index = 0; 
        index < pDeviceContext->MaxObjects && NT_SUCCESS(status); 
        index++, createItem++)
    {
        if (createItem && (createItem->Create))
        {
            PSUBDEVICE subDevice = PSUBDEVICE(createItem->Context);

            if (subDevice)
            {
                NTSTATUS    status2;
                PVOID       pRebalanceSubdevice = NULL;

                status2 = subDevice->QueryInterface(IID_IRebalanceSubdevice,
                                                    &pRebalanceSubdevice);
                if (NT_SUCCESS(status2))
                {
                    switch (PnpNotification)
                    {
                    case PcPnpStop:
                        //
                        // Only this event will result in a notification to
                        // the miniport driver.
                        //
                        PREBALANCESUBDEVICE(pRebalanceSubdevice)->PnpStop();
                        break;
                    case PcPnpQueryStop:
                        //
                        // Check if the subdevice and its streams can be stopped.
                        //
                        status = PREBALANCESUBDEVICE(pRebalanceSubdevice)->PnpQueryStop();
                        break;
                    case PcPnpCancelStop:
                        //
                        // Give an opportunity to our internal logic to cleanup 
                        // any pending-stop state.
                        //
                        PREBALANCESUBDEVICE(pRebalanceSubdevice)->PnpCancelStop();
                        break;
                    default:
                        ASSERT(FALSE);
                        break;
                    }
                    
                    PUNKNOWN(pRebalanceSubdevice)->Release();
                    pRebalanceSubdevice = NULL;
                }
                else if (PcPnpQueryStop == PnpNotification)
                {
                    //
                    // Fail query-stop for subdevices that are not waveRT or 
                    // topology type.
                    //
                    status = STATUS_DEVICE_BUSY;
                }
            }
        }
    }

    DoTraceLevelMessage(TRACE_LEVEL_INFORMATION,
                        PCTRACE_PNP,
                        "Device context %p, subdevices rebalance state: %!STATUS!",
                        pDeviceContext,
                        status);

    return status;
}

#pragma code_seg("PAGE")
/*****************************************************************************
* DisableAllDeviceInterfaces()
*****************************************************************************
* Turns off all the interfaces exposed by the driver.
*/
VOID
DisableAllDeviceInterfaces
(
    _In_    PDEVICE_OBJECT  pDeviceObject
)
{
    ASSERT(pDeviceObject);

    DoTraceLevelMessage(TRACE_LEVEL_VERBOSE,
                        PCTRACE_PNP,
                        "Device %p",
                        pDeviceObject);
    
    PAGED_CODE();

    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);
    PLIST_ENTRY     le;

    //
    // Disable all the device interfaces.
    //
    KeWaitForSingleObject(&pDeviceContext->ListProtect, Executive, KernelMode, FALSE, NULL);
    for (le = pDeviceContext->DeviceInterfaceList.Flink;
        le != &pDeviceContext->DeviceInterfaceList;
        le = le->Flink)
    {
        NTSTATUS status = STATUS_SUCCESS;
        PDEVICEINTERFACE pDeviceInterface = (PDEVICEINTERFACE)le;

        ASSERT(pDeviceInterface);
        ASSERT(pDeviceInterface->SymbolicLinkName.Buffer);

        if (pDeviceContext->AllowRegisterDeviceInterface)
        {
            status = IoSetDeviceInterfaceState(&pDeviceInterface->SymbolicLinkName, FALSE);
        }

        if (NT_SUCCESS(status))
        {
            DoTraceLevelMessage(TRACE_LEVEL_INFORMATION,
                PCTRACE_PNP,
                "Device %p, disabled device interface %wZ",
                pDeviceObject,
                &pDeviceInterface->SymbolicLinkName);
        }
        else
        {
            DoTraceLevelMessage(TRACE_LEVEL_ERROR,
                PCTRACE_PNP,
                "Device %p, failed to disable device interface %wZ, %!STATUS!",
                pDeviceObject,
                &pDeviceInterface->SymbolicLinkName,
                status);
        }
    }
    KeReleaseMutex(&pDeviceContext->ListProtect, FALSE);
}

/*****************************************************************************
 * DispatchPnp()
 *****************************************************************************
 * Supplying your PnP needs for over 20 min
 */
NTSTATUS
DispatchPnp
(
    IN      PDEVICE_OBJECT  pDeviceObject,
    IN      PIRP            pIrp
)
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pIrp);

    NTSTATUS ntStatus = STATUS_SUCCESS;

    BOOLEAN relPositionLock = FALSE;

    PIO_STACK_LOCATION pIrpStack =
        IoGetCurrentIrpStackLocation(pIrp);

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    static PCHAR aszMnNames[] =
    {
        "IRP_MN_START_DEVICE",
        "IRP_MN_QUERY_REMOVE_DEVICE",
        "IRP_MN_REMOVE_DEVICE",
        "IRP_MN_CANCEL_REMOVE_DEVICE",
        "IRP_MN_STOP_DEVICE",
        "IRP_MN_QUERY_STOP_DEVICE",
        "IRP_MN_CANCEL_STOP_DEVICE",

        "IRP_MN_QUERY_DEVICE_RELATIONS",
        "IRP_MN_QUERY_INTERFACE",
        "IRP_MN_QUERY_CAPABILITIES",
        "IRP_MN_QUERY_RESOURCES",
        "IRP_MN_QUERY_RESOURCE_REQUIREMENTS",
        "IRP_MN_QUERY_DEVICE_TEXT",
        "IRP_MN_FILTER_RESOURCE_REQUIREMENTS",
        "IRP_MN_UNKNOWN_0x0e",

        "IRP_MN_READ_CONFIG",
        "IRP_MN_WRITE_CONFIG",
        "IRP_MN_EJECT",
        "IRP_MN_SET_LOCK",
        "IRP_MN_QUERY_ID",
        "IRP_MN_QUERY_PNP_DEVICE_STATE",
        "IRP_MN_QUERY_BUS_INFORMATION",
        "IRP_MN_PAGING_NOTIFICATION",
        "IRP_MN_SURPRISE_REMOVAL"
    };
    if (pIrpStack->MinorFunction >= SIZEOF_ARRAY(aszMnNames))
    {
        DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                             PCTRACE_PNP,
                             "Device %p, Pnp minor function 0x%x",
                             pDeviceObject,
                             pIrpStack->MinorFunction );
    }
    else
    {
        DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                             PCTRACE_PNP,
                             "Device %p, %s",
                             pDeviceObject,
                             aszMnNames[pIrpStack->MinorFunction] );
    }

    ntStatus = PcValidateDeviceContext(pDeviceContext, pIrp);
    if (!NT_SUCCESS(ntStatus))
    {











        // Assume FDO, no PoStartNextPowerIrp as this isn't IRP_MJ_POWER
        ntStatus = pIrp->IoStatus.Status;
        IoCompleteRequest( pIrp, IO_NO_INCREMENT );
        return ntStatus;
    }

    ntStatus = AcquireRemoveLock( pDeviceContext, pIrp );
    if(!NT_SUCCESS(ntStatus))
    {
        pIrp->IoStatus.Status = ntStatus;
        IoCompleteRequest( pIrp, IO_NO_INCREMENT );
        return ntStatus;
    }

    switch (pIrpStack->MinorFunction)
    {
    case IRP_MN_START_DEVICE:

        // if we are already started, something wrong happened
        if( pDeviceContext->DeviceStopState == DeviceStarted )
        {
            //
            // In theory, this is the path that would be exercized by non-stop
            // rebalance. As it's the Fdo's choice to do so via
            // IoInvalidateDeviceState(...), and as we don't do this, we should
            // never ever be here unless something really strange happened...
            //
            // ASSERT(0);

            // ntStatus = STATUS_INVALID_DEVICE_REQUEST;
            DoTraceLevelMessage( TRACE_LEVEL_WARNING,
                                 PCTRACE_PNP,
                                 "Device %p, IRP_MN_START_DEVICE received when already started",
                                 pDeviceObject );
            //CompleteIrp( pDeviceContext, pIrp, ntStatus );

            ntStatus = ForwardIrpSynchronous(pDeviceContext,pIrp); // for some reason we get nested starts
            CompleteIrp( pDeviceContext, pIrp, ntStatus );
        } else {

            //
            // Forward request and start.
            //
            ntStatus = ForwardIrpSynchronous(pDeviceContext,pIrp);

            if (NT_SUCCESS(ntStatus))
            {
                    // Do a real start. Begin by pending the irp

                    IoMarkIrpPending(pIrp);                        
                    pDeviceContext->IrpStart = pIrp;

                    // queue the start work item
                    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                         PCTRACE_PNP,
                                         "Device %p, Queueing WorkQueueItemStart",
                                         pDeviceObject );

                    ntStatus = CallbackEnqueue(
                        &pDeviceContext->pWorkQueueItemStart,
                        PnpStartDevice,
                        pDeviceObject,
                        NULL,
                        PASSIVE_LEVEL,
                        EQCF_DIFFERENT_THREAD_REQUIRED
                        );

                    if (!NT_SUCCESS(ntStatus)) {
                        DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                                             PCTRACE_PNP,
                                             "Device %p, Failed to queue callback, %!STATUS!",
                                             pDeviceObject,
                                             ntStatus );
                        CompleteIrp( pDeviceContext, pIrp, ntStatus );
                    } 
                    ntStatus = STATUS_PENDING;
            }
            else
            {
                DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                                     PCTRACE_PNP,
                                     "Device %p, parent failed to start, %!STATUS!",
                                     pDeviceObject,
                                     ntStatus );
                CompleteIrp(pDeviceContext,pIrp,ntStatus);
            }
        }

        break;

    case IRP_MN_QUERY_STOP_DEVICE:
        //
        // Acquire the device to avoid race condition with Create
        //
        AcquireDevice( pDeviceContext );

        LONG handleCount;
        bool rebalance;
        
        ntStatus = STATUS_SUCCESS;

        //
        // If we are tearing everything down, we must check for open handles,
        // otherwise we do a quick activity check.
        //
        // For ref only, old logic:
        //handleCount = (pDeviceContext->PauseForRebalance) ?
        //    pDeviceContext->ActivePinCount :
        //    pDeviceContext->ExistingObjectCount;
        //
            
        handleCount = pDeviceContext->ExistingObjectCount;
        
        rebalance = false;
        if (handleCount == 0) 
        {
            //
            // no handles => we can support rebalance.
            // (legacy behavior)
            // 
            rebalance = true;
        }
        else
        {
            //
            // handles are present => perform the following checks:
            //
            //  #1 miniport/adapter must support rebalance.
            //
            //  #2 check if existing streams can be stopped. For example 
            //     we'll not support rebalance (doesn't matter what the 
            //     adapter says) if there is a resource (memory mapped I/O) 
            //     exposed in user mode.
            //
            PC_REBALANCE_TYPE rebalanceType;
            
            rebalanceType = GetAdapterRebalanceType(pDeviceContext);
            if (rebalanceType == PcRebalanceRemoveSubdevices)
            {
                if(NT_SUCCESS(PnpNotifySubdevices(pDeviceContext, PcPnpQueryStop)))    
                {
                    rebalance = true;
                }
            }
        }

        if(!rebalance)
        {
            //
            // we must fail this QUERY_STOP_DEVICE request
            //
            ntStatus = STATUS_DEVICE_BUSY;
            DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                                 PCTRACE_PNP,
                                 "Device %p, unable to rebalance,"
                                 " handle count %d, %!STATUS!",
                                 pDeviceObject,
                                 handleCount,
                                 ntStatus );
        }
        else 
        {
            //
            // Pass down the query.
            //
            pIrp->IoStatus.Status = STATUS_SUCCESS;
            ntStatus = ForwardIrpSynchronous(pDeviceContext,pIrp);
            if (NT_SUCCESS(ntStatus)) 
            {

                //
                // pend new creates, this'll keep the active counts from changing.
                //
                pDeviceContext->PendCreates = TRUE;
                pDeviceContext->DeviceStopState = DeviceStopPending;
                
                DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                     PCTRACE_PNP,
                                     "Device %p, query stop succeeded",
                                     pDeviceObject);
                //
                // Notify adapter about QueryStop.
                //
                PnpNotifyAdapter(pDeviceContext, PcPnpQueryStop);
            }
            else 
            {
                DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                                     PCTRACE_PNP,
                                     "Device %p, parent failed query stop, %!STATUS!",
                                     pDeviceObject,
                                     ntStatus );
            }
        }

        if (!NT_SUCCESS(ntStatus)) 
        {
            PnpNotifySubdevices(pDeviceContext, PcPnpCancelStop);
        }
        
        CompleteIrp( pDeviceContext, pIrp, ntStatus );
        ReleaseDevice( pDeviceContext );
        break ;

    case IRP_MN_CANCEL_STOP_DEVICE:
        
        ntStatus = STATUS_SUCCESS;

        if (pDeviceContext->DeviceStopState == DeviceStopPending)
        {
            AcquireDevice( pDeviceContext );
            pDeviceContext->DeviceStopState = DeviceStarted;
            
            //
            // Notify miniport objects about CancelStop.
            //
            PnpNotifyAdapter(pDeviceContext, PcPnpCancelStop);

            //
            // Notify subdevices about CancelStop.
            //
            PnpNotifySubdevices(pDeviceContext, PcPnpCancelStop);
            
            ReleaseDevice( pDeviceContext );
        }

        //
        // allow creates if in D0
        //
        if( NT_SUCCESS(CheckCurrentPowerState(pDeviceObject)) )
        {
            pDeviceContext->PendCreates = FALSE;

            //
            // Pull any pended irps off the pended irp list and
            // pass them back to PcDispatchIrp
            //
            CompletePendedIrps( pDeviceObject,
                                pDeviceContext,
                                EMPTY_QUEUE_AND_PROCESS );
        }

        // forward the irp
        pIrp->IoStatus.Status = STATUS_SUCCESS;
        ntStatus = ForwardIrpAsynchronous(pDeviceContext,pIrp);
        break ;

    case IRP_MN_STOP_DEVICE:

        if (pDeviceContext->DeviceStopState == DeviceStopPending)
        {
            //
            // Acquire locks and drain I/O requests.
            //
            AcquireDevice(pDeviceContext);
            
            if (pDeviceContext->UsePositionLock)
            {
                AcquirePositionLock(pDeviceContext);
                relPositionLock = TRUE;
            }

            //
            // Notify subdevices about the Stop.
            //
            PnpNotifySubdevices(pDeviceContext, PcPnpStop);
            
            //
            // Fail any future I/O directed to this device.
            //
            pDeviceContext->DeviceStopState = DeviceStopped;
            
            //
            // Release locks.
            //
            if (relPositionLock)
            {
                ReleasePositionLock(pDeviceContext);
                relPositionLock = FALSE;
            }
            
            ReleaseDevice(pDeviceContext);

            //
            // Turn off all audio interfaces after the logic above.
            // PnP will notify upper layers about the interfaces going away.
            // Disable the interfaces after the above step to 
            // prevent race-conditions when a new subdevice is added
            // after the disable-interfaces logic has already run.
            //
            DisableAllDeviceInterfaces(pDeviceObject);

            //
            // Now fail any pended irps.
            //
            CompletePendedIrps(pDeviceObject,
                pDeviceContext,
                EMPTY_QUEUE_AND_FAIL);

            //
            // Notify The adapter about the stop. Do not hold the global lock
            // while making this call, the driver may want to wait for any
            // work-items/dpc/async-threads it owns.
            //
            PnpNotifyAdapter(pDeviceContext, PcPnpStop);
            
            //
            // General stop/cleanup logic.
            //
            ntStatus = PnpStopDevice(pDeviceObject, STOPSTYLE_PAUSE_FOR_REBALANCE);

        }
        else
        {
            //
            // From MSDN:
            // "On Windows 2000 and later, a stop IRP is used solely to free 
            // a device's hardware resources so they can be reconfigured. 
            // Once the resources are reconfigured, the device is restarted. 
            // A stop IRP is not a precursor to a remove IRP. See Plug and Play
            // for more information about the order in which PnP IRPs are sent
            // to devices.
            //
            // On Windows 98/Me, a stop IRP is also used after a failed start 
            // and when a device is being disabled. WDM drivers that run on 
            // these operating systems should stop the device, fail any 
            // incoming I/O, and disable and deregister any user-mode 
            // interfaces." 
            //
            // This means it should not be possible in Windows 10 to receive
            // a stop without a successful stop-query. Adding a checked assert
            // to validate this behavior.
            //
            ASSERT(FALSE);
            
            //
            // Either we've decided not to pause during rebalance, or this is
            // a "naked" stop on Win9x, which occurs when the OS wishes to
            // disable us.
            //

            //
            // Stopping us will change our state and tear everything down
            //
            if (pDeviceContext->DeviceStopState != DeviceStopped)
            {
                ntStatus = PnpStopDevice(pDeviceObject, STOPSTYLE_DISABLE);
            }
            else
            {
                DoTraceLevelMessage( TRACE_LEVEL_WARNING,
                                     PCTRACE_PNP,
                                     "Device %p, stop received in unstarted state",
                                     pDeviceObject );
            }

            //
            // Now fail any pended irps.
            //
            CompletePendedIrps( pDeviceObject,
                                pDeviceContext,
                                EMPTY_QUEUE_AND_FAIL );
        }

        if (NT_SUCCESS(ntStatus))
        {
            // forward the irp
            pIrp->IoStatus.Status = STATUS_SUCCESS;
            ntStatus = ForwardIrpAsynchronous(pDeviceContext,pIrp);
        }
        else
        {
            CompleteIrp(pDeviceContext,pIrp,ntStatus);
        }
        break;

    case IRP_MN_QUERY_REMOVE_DEVICE:

        //
        // Acquire the device because we don't want to race with creates.
        //
        AcquireDevice(pDeviceContext);

        if ( pDeviceContext->ExistingObjectCount != 0 ) {

            //
            // Somebody has open handles on us, so fail the QUERY_REMOVE_DEVICE
            // request.
            //
            ntStatus = STATUS_DEVICE_BUSY;

        } else {

            //
            // Lookin good, pass down the query.
            //
            pIrp->IoStatus.Status = STATUS_SUCCESS;
            ntStatus = ForwardIrpSynchronous(pDeviceContext,pIrp);
            if (NT_SUCCESS(ntStatus))
            {
                //
                // Pend future creates.
                //
                pDeviceContext->PendCreates = TRUE;
                DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                     PCTRACE_PNP,
                                     "Device %p, query remove succeeded",
                                     pDeviceObject );

                pDeviceContext->DeviceRemoveState = DeviceRemovePending;
            }
            else
            {
                DoTraceLevelMessage( TRACE_LEVEL_WARNING,
                                     PCTRACE_PNP,
                                     "Device %p, parent failed query remove, %!STATUS!",
                                     pDeviceObject,
                                     ntStatus );
            }
        }

        ReleaseDevice(pDeviceContext);

        CompleteIrp(pDeviceContext,pIrp,ntStatus);

        break;

    case IRP_MN_CANCEL_REMOVE_DEVICE:
        //ASSERT( DeviceRemovePending == pDeviceContext->DeviceRemoveState );

        pDeviceContext->DeviceRemoveState = DeviceAdded;

        //
        // allow creates if in D0
        //
        if( NT_SUCCESS(CheckCurrentPowerState(pDeviceObject)) )
        {
            pDeviceContext->PendCreates = FALSE;

            //
            // Pull any pended irps off the pended irp list and
            // pass them back to PcDispatchIrp
            //
            CompletePendedIrps( pDeviceObject,
                                pDeviceContext,
                                EMPTY_QUEUE_AND_PROCESS );
        }

        // forward the irp
        pIrp->IoStatus.Status = STATUS_SUCCESS;
        ntStatus = ForwardIrpAsynchronous(pDeviceContext,pIrp);
        break;

    case IRP_MN_SURPRISE_REMOVAL:
        //
        // Acquire locks and drain I/O requests.
        //
        AcquireDevice(pDeviceContext);

        if (pDeviceContext->UsePositionLock)
        {
            AcquirePositionLock(pDeviceContext);
            relPositionLock = TRUE;
        }
        
        pDeviceContext->DeviceRemoveState = DeviceSurpriseRemoved;

        //
        // Release locks.
        //
        if (relPositionLock)
        {
            ReleasePositionLock(pDeviceContext);
            relPositionLock = FALSE;
        }
        
        ReleaseDevice(pDeviceContext);

        //
        // Fail any pended irps.
        //
        CompletePendedIrps( pDeviceObject,
                            pDeviceContext,
                            EMPTY_QUEUE_AND_FAIL );

        if (pDeviceContext->DeviceStopState != DeviceStopped)
        {
            PnpStopDevice(pDeviceObject, STOPSTYLE_DISABLE);
        }

        pIrp->IoStatus.Status = STATUS_SUCCESS;

        ntStatus = ForwardIrpAsynchronous( pDeviceContext, pIrp );

        break;

    case IRP_MN_REMOVE_DEVICE:

        //
        // Perform stop if required.
        //
        if (pDeviceContext->DeviceStopState != DeviceStopped)
        {
            DoTraceLevelMessage( TRACE_LEVEL_WARNING,
                                 PCTRACE_PNP,
                                 "Device %p, remove received in started state",
                                 pDeviceObject );
            PnpStopDevice(pDeviceObject, STOPSTYLE_DISABLE);
        }

        //
        // Fail any pended irps.
        //
        CompletePendedIrps( pDeviceObject,
                            pDeviceContext,
                            EMPTY_QUEUE_AND_FAIL );

        //
        // Free device header, must be done before forwarding irp
        //
        if( pDeviceContext->pDeviceHeader )
        {
            KsFreeDeviceHeader(pDeviceContext->pDeviceHeader);
        }

        //
        // Forward the request.
        //
        pIrp->IoStatus.Status = STATUS_SUCCESS;
        ntStatus = ForwardIrpAsynchronous(pDeviceContext, pIrp);

        //
        // Wait for the remove lock
        //
        if (pDeviceContext->RemoveLock)
        {
            AcquireRemoveLock( pDeviceContext, pIrp );  // need to reacquire since we released in ForwardIrpAsynchronous
            IoReleaseRemoveLockAndWait( pDeviceContext->RemoveLock, pIrp );
        }

        //
        // Remove the device.
        //
        PnpRemoveDevice(pDeviceObject);

        break;

    case IRP_MN_QUERY_CAPABILITIES:
        //
        //  Fill out power management / ACPI stuff
        //  for this device.
        //
        ntStatus = GetDeviceACPIInfo( pIrp, pDeviceObject );
        break;

    case IRP_MN_QUERY_DEVICE_RELATIONS:
    case IRP_MN_READ_CONFIG:
    case IRP_MN_WRITE_CONFIG:
    case IRP_MN_QUERY_INTERFACE:
    case IRP_MN_QUERY_RESOURCE_REQUIREMENTS:





    case IRP_MN_QUERY_RESOURCES:
    case IRP_MN_QUERY_DEVICE_TEXT:
    case IRP_MN_FILTER_RESOURCE_REQUIREMENTS:
    case IRP_MN_EJECT:
    case IRP_MN_SET_LOCK:
    case IRP_MN_QUERY_ID:
    case IRP_MN_QUERY_PNP_DEVICE_STATE:
    case IRP_MN_QUERY_BUS_INFORMATION:
//    case IRP_MN_PAGING_NOTIFICATION:
    default:
        ntStatus = ForwardIrpAsynchronous(pDeviceContext,pIrp);
        break;
    }

    return ntStatus;
}


/*****************************************************************************
 * SubdeviceIndex()
 *****************************************************************************
 * Returns the index of a subdevice in the create items list or ULONG(-1) if
 * not found.
 */
ULONG
SubdeviceIndex
(
    IN      PDEVICE_OBJECT  DeviceObject,
    IN      PSUBDEVICE      Subdevice
)
{
    PAGED_CODE();

    ASSERT(DeviceObject);
    ASSERT(Subdevice);

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(DeviceObject->DeviceExtension);

    ASSERT(pDeviceContext);

    PKSOBJECT_CREATE_ITEM createItem =
        pDeviceContext->CreateItems;

    
    ULONG index;
    for
    (
        index = 0;
        index < pDeviceContext->MaxObjects;
        index++, createItem++
    )
    {
        if (PSUBDEVICE(createItem->Context) == Subdevice)
        {
            break;
        }
    }

    if (index == pDeviceContext->MaxObjects)
    {
        index = ULONG(-1);
    }

    return index;
}

/*****************************************************************************
 * PcRegisterSubdevice()
 *****************************************************************************
 * Registers a subdevice.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcRegisterSubdevice
(
    __in      PDEVICE_OBJECT  DeviceObject,
    __in      PWSTR           Name,
    __in      PUNKNOWN        Unknown
)
{
    PAGED_CODE();

    ASSERT(DeviceObject);
    ASSERT(Name);
    ASSERT(Unknown);

    _DbgPrintF(DEBUGLVL_VERBOSE,("PcRegisterSubdevice %S",Name));

    //
    // Validate Parameters.
    //
    if (NULL == DeviceObject ||
        NULL == Name ||
        NULL == Unknown)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcRegisterSubDevice : Invalid Parameter."));
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Validate name length.
    //
    size_t nameLength;

    nameLength = wcslen(Name) * sizeof(WCHAR);
    if (nameLength >= MAXUSHORT)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcRegisterSubDevice : Name is too long, %Iu bytes.", nameLength));
        return STATUS_INVALID_PARAMETER;
    }
    
    PSUBDEVICE  pSubdevice;
    NTSTATUS    ntStatus =
        Unknown->QueryInterface
        (
        IID_ISubdevice,
        (PVOID *) &pSubdevice
        );

    if (NT_SUCCESS(ntStatus))
    {
        PDEVICE_CONTEXT pDeviceContext =
            PDEVICE_CONTEXT(DeviceObject->DeviceExtension);
            
        // Synchronize with unregisterSubdevice
        AcquireDevice(pDeviceContext);

        //
        // Fail new registrations if the device is in the stop state.
        //
        if (DeviceStopped == pDeviceContext->DeviceStopState)
        {
            ntStatus = STATUS_DEVICE_NOT_READY;
            DoTraceLevelMessage(TRACE_LEVEL_INFORMATION,
                                PCTRACE_PNP,
                                "Device context %p, PcRegisterSubDevice :"
                                " in STOP state, %!STATUS!",
                                pDeviceContext,
                                ntStatus);
        }
            
        if (NT_SUCCESS(ntStatus))
        {
            ntStatus =
                AddIrpTargetFactoryToDevice
                (
                DeviceObject,
                pSubdevice,
                Name,
                NULL   



                );
        }

        const SUBDEVICE_DESCRIPTOR *pSubdeviceDescriptor = NULL;
        if (NT_SUCCESS(ntStatus))
        {
            ntStatus = pSubdevice->GetDescriptor(&pSubdeviceDescriptor);
        }
        else
        {
            _DbgPrintF(DEBUGLVL_TERSE,("PcRegisterSubdevice AddIrpTargetFactoryToDevice failed (0x%08x)",ntStatus));
        }

        if (NT_SUCCESS(ntStatus) && pSubdeviceDescriptor->Topology->CategoriesCount)
        {

            ULONG index = SubdeviceIndex(DeviceObject,pSubdevice);
            if(index != ULONG(-1))
            {
                ASSERT(pSubdeviceDescriptor->Topology->Categories);
                ASSERT(pDeviceContext);

                UNICODE_STRING referenceString;
                RtlInitUnicodeString(&referenceString,Name);

                const GUID *pGuidCategories =
                    pSubdeviceDescriptor->Topology->Categories;
                for
                         (   ULONG ul = pSubdeviceDescriptor->Topology->CategoriesCount
                             ;   ul--
                         ;   pGuidCategories++
                         )
                {
                    UNICODE_STRING linkName;

                    if (pDeviceContext->AllowRegisterDeviceInterface)
                    {
                        ntStatus
                            = IoRegisterDeviceInterface
                              (
                              pDeviceContext->PhysicalDeviceObject,
                                                              pGuidCategories,
                                                              &referenceString,
                              &linkName
                              );

                        if (NT_SUCCESS(ntStatus))
                        {
                            ntStatus =
                                IoSetDeviceInterfaceState
                                (
                                &linkName,
                                TRUE
                                );

                            if (NT_SUCCESS(ntStatus))
                            {
                                _DbgPrintF(DEBUGLVL_VERBOSE,("PcRegisterSubdevice device interface %S set to state TRUE",linkName.Buffer));
                            }
                            else
                            {
                                _DbgPrintF(DEBUGLVL_TERSE,("PcRegisterSubdevice IoSetDeviceInterfaceState failed (0x%08x)",ntStatus));
                            }
                        }
                        else
                        {
                            _DbgPrintF(DEBUGLVL_TERSE,("PcRegisterSubdevice IoRegisterDeviceInterface failed (0x%08x)",ntStatus));
                        }
                    }
                    else
                    {
                        linkName.Length = (USHORT)(wcslen(Name) * sizeof(WCHAR));
                        linkName.MaximumLength = linkName.Length + sizeof(UNICODE_NULL);
                        linkName.Buffer = (PWSTR)ExAllocatePoolWithTag(PagedPool, linkName.MaximumLength,'NLcP');  //  'PcLN'
                        if (!linkName.Buffer)
                        {
                            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                        }
                        else
                        {
                            RtlStringCbCopyW (linkName.Buffer, linkName.MaximumLength, Name);
                        }
                    }

                    if (NT_SUCCESS(ntStatus))
                    {
                        //
                        // Save the first symbolic link name in the table.
                        //
                        if (! pDeviceContext->SymbolicLinkNames[index].Buffer)
                        {
                            pDeviceContext->SymbolicLinkNames[index] = linkName;
                        }

                        //
                        // Save the interface in a list for cleanup.
                        //
                        PDEVICEINTERFACE pDeviceInterface = new(PagedPool,'iDcP') DEVICEINTERFACE;
                        if (pDeviceInterface)
                        {
                            pDeviceInterface->Interface         = *pGuidCategories;
                            pDeviceInterface->SymbolicLinkName  = linkName;
                            pDeviceInterface->Subdevice         = pSubdevice;

                            KeWaitForSingleObject(&pDeviceContext->ListProtect,Executive,KernelMode,FALSE,NULL);
                            InsertTailList
                                (
                                &pDeviceContext->DeviceInterfaceList,
                                &pDeviceInterface->ListEntry
                                );
                            KeReleaseMutex(&pDeviceContext->ListProtect,FALSE);
                        }
                        else
                        {
                            _DbgPrintF(DEBUGLVL_TERSE,("PcRegisterSubdevice failed to allocate device interface structure for later cleanup"));
                            if (pDeviceContext->SymbolicLinkNames[index].Buffer == linkName.Buffer)
                                pDeviceContext->SymbolicLinkNames[index].Buffer = NULL;
                            RtlFreeUnicodeString(&linkName);
                            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                            break;
                        }
                    }
                }
            }
            else
            {
               ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            }
        }

        // Synchronize with unregisterSubdevice
        ReleaseDevice(pDeviceContext);

        pSubdevice->Release();
       
    }
    else
    {
        _DbgPrintF(DEBUGLVL_TERSE,("QI for IID_ISubdevice failed on UNKNOWN 0x%08p", Unknown));
    }

    PcWmiLogEvent( TRACE_LEVEL_INFORMATION,
                   PC_ETYPE_SUBDEV_REGISTER,
                   PortCls_SubDevice_GUID,
                   (ULONGLONG)DeviceObject,
                   (ULONGLONG)pSubdevice,
                   ntStatus,
                   0,
                   (ULONGLONG)NULL );

    return ntStatus;
}
/*****************************************************************************
 * PcUnregisterSubdevice()
 *****************************************************************************
 * Unregisters a subdevice.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcUnregisterSubdevice
(
    IN      PDEVICE_OBJECT  DeviceObject,
    IN      PUNKNOWN        Unknown
)
{
    PAGED_CODE();

    ASSERT(DeviceObject);
    ASSERT(Unknown);

    _DbgPrintF(DEBUGLVL_VERBOSE,("PcUnregisterSubdevice"));

    BOOLEAN relPositionLock = FALSE;

    //
    // Validate Parameters.
    //
    if (NULL == DeviceObject ||
        NULL == Unknown)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcUnregisterSubDevice : Invalid Parameter."));
        return STATUS_INVALID_PARAMETER;
    }

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(DeviceObject->DeviceExtension);

    ASSERT( pDeviceContext );

    // Lock out creates
    AcquireDevice(pDeviceContext);

    // Drain I/O.
    if (pDeviceContext->UsePositionLock)
    {
        AcquirePositionLock(pDeviceContext);
        relPositionLock = TRUE;
    }
    
    PSUBDEVICE  pSubdevice = NULL;
    NTSTATUS    ntStatus =
        Unknown->QueryInterface
        (
        IID_ISubdevice,
        (PVOID *) &pSubdevice
        );

    if (NT_SUCCESS(ntStatus))
    {
        ntStatus = STATUS_NOT_FOUND;

        //
        //  Remove DeviceInterface from DeviceInterfaceList
        //
        KeWaitForSingleObject(&pDeviceContext->ListProtect,Executive,KernelMode,FALSE,NULL);
        for( PLIST_ENTRY ListEntry = pDeviceContext->DeviceInterfaceList.Flink;
             ListEntry != &(pDeviceContext->DeviceInterfaceList);
             ListEntry = ListEntry->Flink )
        {
            PDEVICEINTERFACE pDeviceInterface = (PDEVICEINTERFACE) CONTAINING_RECORD( ListEntry,
                                                                                      DEVICEINTERFACE,
                                                                                      ListEntry );
            ASSERT(pDeviceInterface);
            ASSERT(pDeviceInterface->SymbolicLinkName.Buffer);

            //
            //  Is this the correct DeviceInterface to remove from list?
            //
            if (pDeviceInterface->Subdevice == pSubdevice)
            {
                if (pDeviceContext->AllowRegisterDeviceInterface)
                {
                    ntStatus = IoSetDeviceInterfaceState(&pDeviceInterface->SymbolicLinkName,FALSE);
                }
#if DBG
                if (NT_SUCCESS(ntStatus))
                {
                    _DbgPrintF(DEBUGLVL_VERBOSE,("PcUnregisterSubdevice disabled device interface %S",pDeviceInterface->SymbolicLinkName.Buffer));
                }
                else
                {
                    _DbgPrintF(DEBUGLVL_TERSE,("PcUnregisterSubdevice failed to disable device interface %S (0x%08x)",pDeviceInterface->SymbolicLinkName.Buffer,ntStatus));
                }
#endif
                //
                // Step back to take out the entry
                //
                PLIST_ENTRY TempListEntry = ListEntry->Blink;
                RemoveEntryList(ListEntry);
                ListEntry = TempListEntry;

                RtlFreeUnicodeString(&pDeviceInterface->SymbolicLinkName);
                delete pDeviceInterface;
            }
        }
        KeReleaseMutex(&pDeviceContext->ListProtect,FALSE);

        PKSOBJECT_CREATE_ITEM pKsObjectCreateItem =
            pDeviceContext->CreateItems;

        for
        (
            ULONG index = 0;
            index < pDeviceContext->MaxObjects;
            index++, pKsObjectCreateItem++
        )
        {
            if (pKsObjectCreateItem->Create)
            {
                ASSERT(pKsObjectCreateItem->Context);






                if (pSubdevice == PSUBDEVICE(pKsObjectCreateItem->Context))
                {
                    //
                    // Clear the entry in the symbolic link names table.
                    //
                    RtlZeroMemory(&pDeviceContext->SymbolicLinkNames[index],sizeof(UNICODE_STRING));

                    //
                    // Zero the create function so we won't get creates.
                    //
                    pKsObjectCreateItem->Create = NULL;

                    //
                    // Release the subdevice referenced by this create item.
                    //
                    pSubdevice->ReleaseChildren();
                    pSubdevice->Release();
                    break;
                }
            }
        }

        //
        //  Fail any pended irps on the cancelable queue for this device
        //
        FailPendedIrpsForSubdevice(DeviceObject,
                                   pDeviceContext,
                                   pSubdevice);

        //
        // Release the referenced subdevice
        //
        pSubdevice->Release();
    }
    else
    {
        _DbgPrintF(DEBUGLVL_TERSE,("QI for IID_ISubdevice failed on UNKNOWN 0x%08p",pSubdevice));
    }

    //
    // Release locks.
    //
    if (relPositionLock)
    {
        ReleasePositionLock(pDeviceContext);
        relPositionLock = FALSE;
    }
    
    // Allow Creates again
    ReleaseDevice(pDeviceContext);

    PcWmiLogEvent( TRACE_LEVEL_INFORMATION,
                   PC_ETYPE_SUBDEV_UNREGISTER,
                   PortCls_SubDevice_GUID,
                   (ULONGLONG)DeviceObject,
                   (ULONGLONG)pSubdevice,
                   ntStatus,
                   0,
                   (ULONGLONG)NULL );

    return ntStatus;
}

/*****************************************************************************
 * RegisterPhysicalConnection_()
 *****************************************************************************
 * Registers a physical connection between subdevices or external devices.
 * Specify either pUnknownTo or pUnicodeStringTo. Same for *From parameters.
 */
static
NTSTATUS
RegisterPhysicalConnection_
(
    IN      PDEVICE_OBJECT  pDeviceObject,
    IN      PUNKNOWN        pUnknownFrom        OPTIONAL,
    IN      PUNICODE_STRING pUnicodeStringFrom  OPTIONAL,
    IN      ULONG           ulFromPin,
    IN      PUNKNOWN        pUnknownTo          OPTIONAL,
    IN      PUNICODE_STRING pUnicodeStringTo    OPTIONAL,
    IN      ULONG           ulToPin
)
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pUnknownFrom || pUnicodeStringFrom);
    ASSERT(pUnknownTo || pUnicodeStringTo);
    ASSERT(! (pUnknownFrom && pUnicodeStringFrom));
    ASSERT(! (pUnknownTo && pUnicodeStringTo));

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    ASSERT(pDeviceContext);

    // Synchronize with Unregister...
    AcquireDevice(pDeviceContext);

    PSUBDEVICE  pSubdeviceFrom  = NULL;
    PSUBDEVICE  pSubdeviceTo    = NULL;

    NTSTATUS ntStatus = STATUS_SUCCESS;

    if (pUnknownFrom)
    {
        ntStatus =
            pUnknownFrom->QueryInterface
            (
                IID_ISubdevice,
                (PVOID *) &pSubdeviceFrom
            );
    }
    else
    {
        ntStatus =
            DupUnicodeString
            (
                &pUnicodeStringFrom,
                pUnicodeStringFrom
            );
    }

    if (NT_SUCCESS(ntStatus))
    {
        if (pUnknownTo)
        {
            ntStatus =
                pUnknownTo->QueryInterface
                (
                    IID_ISubdevice,
                    (PVOID *) &pSubdeviceTo
                );
        }
        else
        {
            ntStatus =
                DupUnicodeString
                (
                    &pUnicodeStringTo,
                    pUnicodeStringTo
                );
        }
    }
    else
    {
        // Don't delete those unicode strings.
        pUnicodeStringFrom = NULL;
        pUnicodeStringTo = NULL;
    }

    if (NT_SUCCESS(ntStatus))
    {
        PPHYSICALCONNECTION pPhysicalConnection =
            new(PagedPool,'cPcP') PHYSICALCONNECTION;

        if (pPhysicalConnection)
        {
            pPhysicalConnection->FromSubdevice   = pSubdeviceFrom;
            pPhysicalConnection->FromString      = pUnicodeStringFrom;
            pPhysicalConnection->FromPin         = ulFromPin;
            pPhysicalConnection->ToSubdevice     = pSubdeviceTo;
            pPhysicalConnection->ToString        = pUnicodeStringTo;
            pPhysicalConnection->ToPin           = ulToPin;

            if (pPhysicalConnection->FromSubdevice)
            {
                pPhysicalConnection->FromSubdevice->AddRef();
            }
            if (pPhysicalConnection->ToSubdevice)
            {
                pPhysicalConnection->ToSubdevice->AddRef();
            }

            //
            // So they don't get deleted.
            //
            pUnicodeStringFrom = NULL;
            pUnicodeStringTo = NULL;

            KeWaitForSingleObject(&pDeviceContext->ListProtect,Executive,KernelMode,FALSE,NULL);
            InsertTailList
            (
                &pDeviceContext->PhysicalConnectionList,
                &pPhysicalConnection->ListEntry
            );
            KeReleaseMutex(&pDeviceContext->ListProtect,FALSE);
        }
        else
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }
    }
    else
    {
        // Don't delete this unicode string.
        pUnicodeStringTo = NULL;
    }

    if (pSubdeviceFrom)
    {
        pSubdeviceFrom->Release();
    }

    if (pSubdeviceTo)
    {
        pSubdeviceTo->Release();
    }

    if (pUnicodeStringFrom)
    {
        DelUnicodeString(pUnicodeStringFrom);
    }

    if (pUnicodeStringTo)
    {
        DelUnicodeString(pUnicodeStringTo);
    }

    // Synchronize with Unregister...
    ReleaseDevice(pDeviceContext);

    return ntStatus;
}

/*****************************************************************************
 * PcRegisterPhysicalConnection()
 *****************************************************************************
 * Registers a physical connection between subdevices.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcRegisterPhysicalConnection
(
    __in    PDEVICE_OBJECT  pDeviceObject,
    __in    PUNKNOWN        pUnknownFrom,
    __in    ULONG           ulFromPin,
    __in    PUNKNOWN        pUnknownTo,
    __in    ULONG           ulToPin
)
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pUnknownFrom);
    ASSERT(pUnknownTo);

    //
    // Validate Parameters.
    //
    if (NULL == pDeviceObject ||
        NULL == pUnknownFrom ||
        NULL == pUnknownTo)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcRegisterPhysicalConnection : Invalid Parameter."));
        return STATUS_INVALID_PARAMETER;
    }

    return
        RegisterPhysicalConnection_
        (
            pDeviceObject,
            pUnknownFrom,
            NULL,
            ulFromPin,
            pUnknownTo,
            NULL,
            ulToPin
        );
}

/*****************************************************************************
 * PcRegisterPhysicalConnectionToExternal()
 *****************************************************************************
 * Registers a physical connection from a subdevice to an external device.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcRegisterPhysicalConnectionToExternal
(
    __in    PDEVICE_OBJECT  pDeviceObject,
    __in    PUNKNOWN        pUnknownFrom,
    __in    ULONG           ulFromPin,
    __in    PUNICODE_STRING pUnicodeStringTo,
    __in    ULONG           ulToPin
)
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pUnknownFrom);
    ASSERT(pUnicodeStringTo);

    //
    // Validate Parameters.
    //
    if (NULL == pDeviceObject ||
        NULL == pUnknownFrom ||
        NULL == pUnicodeStringTo)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcRegisterPhysicalConnectionToExternal : Invalid Parameter."));
        return STATUS_INVALID_PARAMETER;
    }

    return
        RegisterPhysicalConnection_
        (
            pDeviceObject,
            pUnknownFrom,
            NULL,
            ulFromPin,
            NULL,
            pUnicodeStringTo,
            ulToPin
        );
}

/*****************************************************************************
 * PcRegisterPhysicalConnectionFromExternal()
 *****************************************************************************
 * Registers a physical connection to a subdevice from an external device.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcRegisterPhysicalConnectionFromExternal
(
    __in    PDEVICE_OBJECT  pDeviceObject,
    __in    PUNICODE_STRING pUnicodeStringFrom,
    __in    ULONG           ulFromPin,
    __in    PUNKNOWN        pUnknownTo,
    __in    ULONG           ulToPin
)
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pUnicodeStringFrom);
    ASSERT(pUnknownTo);

    //
    // Validate Parameters.
    //
    if (NULL == pDeviceObject ||
        NULL == pUnicodeStringFrom ||
        NULL == pUnknownTo)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcRegisterPhysicalConnectionFromExternal : Invalid Parameter."));
        return STATUS_INVALID_PARAMETER;
    }

    return
        RegisterPhysicalConnection_
        (
            pDeviceObject,
            NULL,
            pUnicodeStringFrom,
            ulFromPin,
            pUnknownTo,
            NULL,
            ulToPin
        );
}

/*****************************************************************************
 * UnregisterPhysicalConnection_()
 *****************************************************************************
 * Unregisters a physical connection between subdevices or external devices.
 */
static
NTSTATUS
UnregisterPhysicalConnection_
(
    IN      PDEVICE_OBJECT  pDeviceObject,
    IN      PUNKNOWN        pUnknownFrom        OPTIONAL,
    IN      PUNICODE_STRING pUnicodeStringFrom  OPTIONAL,
    IN      ULONG           ulFromPin,
    IN      PUNKNOWN        pUnknownTo          OPTIONAL,
    IN      PUNICODE_STRING pUnicodeStringTo    OPTIONAL,
    IN      ULONG           ulToPin
)
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pUnknownFrom || pUnicodeStringFrom);
    ASSERT(pUnknownTo || pUnicodeStringTo);
    ASSERT(! (pUnknownFrom && pUnicodeStringFrom));
    ASSERT(! (pUnknownTo && pUnicodeStringTo));

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    ASSERT(pDeviceContext);

    // Lock out Creates
    AcquireDevice(pDeviceContext);

    PSUBDEVICE  pSubdeviceFrom  = NULL;
    PSUBDEVICE  pSubdeviceTo    = NULL;

    NTSTATUS ntStatus = STATUS_SUCCESS;

    if (pUnknownFrom)
    {
        ntStatus =
            pUnknownFrom->QueryInterface
            (
                IID_ISubdevice,
                (PVOID *) &pSubdeviceFrom
            );
    }
    else
    {
        ntStatus = STATUS_NOT_FOUND;
    }

    if (NT_SUCCESS(ntStatus))
    {
        if (pUnknownTo)
        {
            ntStatus =
                pUnknownTo->QueryInterface
                (
                    IID_ISubdevice,
                    (PVOID *) &pSubdeviceTo
                );
        }
    }

    if (NT_SUCCESS(ntStatus))
    {
        ntStatus = STATUS_NOT_FOUND;

        KeWaitForSingleObject(&pDeviceContext->ListProtect,Executive,KernelMode,FALSE,NULL);
        for( PLIST_ENTRY ListEntry = pDeviceContext->PhysicalConnectionList.Flink;
             ListEntry != &(pDeviceContext->PhysicalConnectionList);
             ListEntry = ListEntry->Flink )
        {
            PPHYSICALCONNECTION pPhysicalConnection = (PPHYSICALCONNECTION) CONTAINING_RECORD( ListEntry,
                                                                                               PHYSICALCONNECTION,
                                                                                               ListEntry );

            if ( ( (pSubdeviceFrom == pPhysicalConnection->FromSubdevice) ||
                   ( (pUnicodeStringFrom && pPhysicalConnection->FromString) &&
                       (!RtlCompareUnicodeString(pUnicodeStringFrom,
                                                 pPhysicalConnection->FromString,
                                                 TRUE) ) ) )                        &&
                 ( (pSubdeviceTo == pPhysicalConnection->ToSubdevice) ||
                   ( (pUnicodeStringTo && pPhysicalConnection->ToString) &&
                       (!RtlCompareUnicodeString(pUnicodeStringTo,
                                                 pPhysicalConnection->ToString,
                                                 TRUE) ) ) ) )
            {
                if ( (ulFromPin    == pPhysicalConnection->FromPin) &&
                     (ulToPin      == pPhysicalConnection->ToPin) )
                {
                    if (pPhysicalConnection->FromSubdevice)
                    {
                        pPhysicalConnection->FromSubdevice->Release();
                    }
                    if (pPhysicalConnection->ToSubdevice)
                    {
                        pPhysicalConnection->ToSubdevice->Release();
                    }
                    if (pPhysicalConnection->FromString)
                    {
                        DelUnicodeString(pPhysicalConnection->FromString);
                    }
                    if (pPhysicalConnection->ToString)
                    {
                        DelUnicodeString(pPhysicalConnection->ToString);
                    }

                    RemoveEntryList(ListEntry);
                    delete pPhysicalConnection;

                    ntStatus = STATUS_SUCCESS;
                    break;
                }
            }
        }
        KeReleaseMutex(&pDeviceContext->ListProtect,FALSE);
    }

    if (pSubdeviceFrom)
    {
        pSubdeviceFrom->Release();
    }

    if (pSubdeviceTo)
    {
        pSubdeviceTo->Release();
    }

    // Allow creates again
    ReleaseDevice(pDeviceContext);

    return ntStatus;
}

PORTCLASSAPI
NTSTATUS
NTAPI
/*****************************************************************************
 * PcUnregisterPhysicalConnection()
 *****************************************************************************
 * Unregisters a physical connection between subdevices.
 */
PcUnregisterPhysicalConnection
(
    IN      PDEVICE_OBJECT  pDeviceObject,
    IN      PUNKNOWN        pUnknownFrom,
    IN      ULONG           ulFromPin,
    IN      PUNKNOWN        pUnknownTo,
    IN      ULONG           ulToPin
)
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pUnknownFrom);
    ASSERT(pUnknownTo);

    //
    // Validate Parameters.
    //
    if (NULL == pDeviceObject ||
        NULL == pUnknownFrom ||
        NULL == pUnknownTo)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcUnregisterPhysicalConnection : Invalid Parameter."));
        return STATUS_INVALID_PARAMETER;
    }

    return
        UnregisterPhysicalConnection_
        (
            pDeviceObject,
            pUnknownFrom,
            NULL,
            ulFromPin,
            pUnknownTo,
            NULL,
            ulToPin
        );
}

/*****************************************************************************
 * PcUnregisterPhysicalConnectionToExternal()
 *****************************************************************************
 * Unregisters a physical connection from a subdevice to an external device.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcUnregisterPhysicalConnectionToExternal
(
    IN      PDEVICE_OBJECT  pDeviceObject,
    IN      PUNKNOWN        pUnknownFrom,
    IN      ULONG           ulFromPin,
    IN      PUNICODE_STRING pUnicodeStringTo,
    IN      ULONG           ulToPin
)
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pUnknownFrom);
    ASSERT(pUnicodeStringTo);

    //
    // Validate Parameters.
    //
    if (NULL == pDeviceObject ||
        NULL == pUnknownFrom ||
        NULL == pUnicodeStringTo)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcUnregisterPhysicalConnectionToExternal : Invalid Parameter."));
        return STATUS_INVALID_PARAMETER;
    }

    return
        UnregisterPhysicalConnection_
        (
            pDeviceObject,
            pUnknownFrom,
            NULL,
            ulFromPin,
            NULL,
            pUnicodeStringTo,
            ulToPin
        );
}

/*****************************************************************************
 * PcUnregisterPhysicalConnectionFromExternal()
 *****************************************************************************
 * Unregisters a physical connection to a subdevice from an external device.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcUnregisterPhysicalConnectionFromExternal
(
    IN      PDEVICE_OBJECT  pDeviceObject,
    IN      PUNICODE_STRING pUnicodeStringFrom,
    IN      ULONG           ulFromPin,
    IN      PUNKNOWN        pUnknownTo,
    IN      ULONG           ulToPin
)
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pUnicodeStringFrom);
    ASSERT(pUnknownTo);

    //
    // Validate Parameters.
    //
    if (NULL == pDeviceObject ||
        NULL == pUnicodeStringFrom ||
        NULL == pUnknownTo)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcUnregisterPhysicalConnectionFromExternal : Invalid Parameter."));
        return STATUS_INVALID_PARAMETER;
    }

    return
        UnregisterPhysicalConnection_
        (
            pDeviceObject,
            NULL,
            pUnicodeStringFrom,
            ulFromPin,
            pUnknownTo,
            NULL,
            ulToPin
        );
}


/*****************************************************************************
 * PcGetPhysicalDeviceObject()
 *****************************************************************************
 * Return the physical device object passed in to the portcls in the PcAddAdapterDevice call
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcGetPhysicalDeviceObject
(
    IN  PDEVICE_OBJECT  pDeviceObject,
    OUT PDEVICE_OBJECT  *ppPhysicalObject
)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PAGED_CODE();

    ASSERT(pDeviceObject);

    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);
    ASSERT(pDeviceContext);

    *ppPhysicalObject = NULL;

    if (pDeviceContext)
    {
        *ppPhysicalObject = pDeviceContext->PhysicalDeviceObject;
    }
    else
    {
        ntStatus = STATUS_DEVICE_CONFIGURATION_ERROR;
    }

    return ntStatus;
}

#pragma code_seg()
/*****************************************************************************
 * AcquireRemoveLock()
 *****************************************************************************
 * Acquires the remove lock.
 */
NTSTATUS
AcquireRemoveLock
(
    IN  PDEVICE_CONTEXT pDeviceContext,
    IN  PVOID           pTag
)
{
    ASSERT(pDeviceContext);

    NTSTATUS ntStatus = STATUS_SUCCESS;

    if(NULL != pDeviceContext->RemoveLock)
    {
        ntStatus = IoAcquireRemoveLock( pDeviceContext->RemoveLock, pTag );
    }

    return ntStatus;
}

#pragma code_seg()
/*****************************************************************************
 * ReleaseRemoveLock()
 *****************************************************************************
 * Releases the remove lock.
 */
void
ReleaseRemoveLock
(
    IN  PDEVICE_CONTEXT pDeviceContext,
    IN  PVOID           pTag
)
{
    ASSERT(pDeviceContext);

    if(NULL != pDeviceContext->RemoveLock)
    {
        IoReleaseRemoveLock( pDeviceContext->RemoveLock, pTag );
    }
}

#pragma code_seg("PAGE")
PORTCLASSAPI
NTSTATUS
PcAddStreamResource(
    _In_  PDEVICE_OBJECT     PhysicalDeviceObject, 
    _In_  PVOID              ResourceSetId, 
    _In_  PPCSTREAMRESOURCE_DESCRIPTOR ResourceDescriptor,
    _Out_ PCSTREAMRESOURCE*  ResourceHandle
    )
{
    PAGED_CODE();

    return StreamResourceManager_AddResource(PhysicalDeviceObject,
                                             ResourceSetId,
                                             ResourceDescriptor,
                                             ResourceHandle);
}

#pragma code_seg("PAGE")
PORTCLASSAPI
NTSTATUS
PcRemoveStreamResource(
    _In_ PCSTREAMRESOURCE  ResourceHandle
    )
{
    PAGED_CODE();

    return StreamResourceManager_RemoveResource(ResourceHandle);
}

