/*****************************************************************************
 * irpstrm.cpp - IRP stream object implementation
 *****************************************************************************
 * Copyright (c) Microsoft.  All rights reserved.
 */

#ifndef PC_KDEXT
#include "private.h"

#if !defined(_PREFAST_) //  Had to use an ugly prefast hack...
#pragma warning(disable:4068)
#endif

DRIVER_CANCEL           IrpStreamCancelRoutine;
static DRIVER_CONTROL   CallbackFromIoAllocateAdapterChannel;
KDEFERRED_ROUTINE       ReportMapAvailable;

#if DBG

#define DbgAcquireMappingIrp(a,b)   AcquireMappingIrp(a,b)
#define DbgAcquireUnmappingIrp(a)   AcquireUnmappingIrp(a)

#else

#define DbgAcquireMappingIrp(a,b)   AcquireMappingIrp(b)
#define DbgAcquireUnmappingIrp(a)   AcquireUnmappingIrp()

#endif

#include "pcwpptr.h"
#include "irpstrm.tmh"

#else   // PC_KDEXT

#define DoTraceMessage(x)

#endif  // PC_KDEXT



#define WHOLE_MDL_THRESHOLD   (PAGE_SIZE * 16)
#define PRE_OFFSET_THRESHOLD  ((PAGE_SIZE / 4) * 3)
#define POST_OFFSET_THRESHOLD (PAGE_SIZE / 4)
#define MAX_PAGES_PER_MDL     16
#define POOL_TAG_IRPSTREAM_IRP_CONTEXT 'sIcP'


#define MAPPING_QUEUE_SIZE  128   // maximum entries in the mapping queue
#define MAX_MAPPINGS        15    // maximum mappings per IoMapTransfer call
                                  //   (this results in at most 16 map registers)

/*****************************************************************************
 * PACKET_HEADER
 *****************************************************************************
 * Extension of KSSTREAM_HEADER containing a pointer to the matching MDL and
 * progress indicators for locking, mapping and unmapping.
 *
 * Invariants:  BytesTotal >= MapPosition >= UnmapPosition
 *
 * It is true of MapPosition and UnmapPosition that at most one packet in an 
 * IrpStream may have a value for the field that is not zero or BytesTotal.  
 * If there is such a packet, all packets preceding it have 0 in that field 
 * and all packets following have BytesTotal in that field.  The two fields 
 * form two progress indicators showing the position in the IrpStream that is
 * currently being mapped or unmapped.
 *
 * When both the BytesX are equal, the packet is ready for completion.  When
 * this is true of all packets in an IRP, the IRP is ready for completion.
 *
 * InputPosition and OutputPosition are used to locate the packet in the
 * stream.  InputPosition refers to the byte position of the packet on the
 * input side.  This means that looped packets are counted only once in this
 * context.  OutputPosition refers to the byte position of the packet on the
 * output side.  This means that looped packets are counted as many times as
 * they are 'played'.
 */
typedef struct PACKET_HEADER_
{
    PKSSTREAM_HEADER        StreamHeader;
    PMDL                    MdlAddress;
    ULONG                   BytesTotal;
    ULONG                   MapPosition;
    ULONG                   UnmapPosition;
    ULONG                   MapCount;
    ULONGLONG               InputPosition;
    ULONGLONG               OutputPosition;
    BOOLEAN                 IncrementMapping;
    BOOLEAN                 IncrementUnmapping;
    struct PACKET_HEADER_ * Next;
}
PACKET_HEADER, *PPACKET_HEADER;

typedef struct
{
#if (DBG)
    ULONG                   IrpLabel;
    ULONG                   Reserved;
#endif
    PEPROCESS               SubmissionProcess;
    PVOID                   IrpStream;
    PPACKET_HEADER          LockingPacket;
    PPACKET_HEADER          MappingPacket;
    PPACKET_HEADER          UnmappingPacket;
    PACKET_HEADER           Packets[1]; // variable
}
IRP_CONTEXT, *PIRP_CONTEXT;

typedef struct
{
    PHYSICAL_ADDRESS    PhysicalAddress;
    PIRP                Irp;
    PPACKET_HEADER      PacketHeader;
    PVOID               VirtualAddress;
    ULONG               ByteCount;
    ULONG               Flags;
    PVOID               MapRegisterBase;
    PVOID               Tag;
    ULONG               MappingStatus;
    PVOID               SubpacketVa;
    ULONG               SubpacketBytes;
}
MAPPING_QUEUE_ENTRY, *PMAPPING_QUEUE_ENTRY;

#define MAPPING_STATUS_EMPTY        0x0000
#define MAPPING_STATUS_MAPPED       0x0001
#define MAPPING_STATUS_DELIVERED    0x0002
#define MAPPING_STATUS_REVOKED      0x0004

#define MAPPING_FLAG_END_OF_SUBPACKET   0x0002

#define PPACKET_HEADER_LOOP PPACKET_HEADER(1)

#define CAST_LVALUE(type,lvalue) (*((type*)&(lvalue)))

#define FLINK_IRP_STORAGE(Irp)              \
    CAST_LVALUE(PLIST_ENTRY,(Irp)->Tail.Overlay.ListEntry.Flink)
#define BLINK_IRP_STORAGE(Irp)              \
    CAST_LVALUE(PLIST_ENTRY,(Irp)->Tail.Overlay.ListEntry.Blink)
#define FIRST_STREAM_HEADER_IRP_STORAGE(Irp)       \
    CAST_LVALUE(PKSSTREAM_HEADER,(Irp)->AssociatedIrp.SystemBuffer)
#define IRP_CONTEXT_IRP_STORAGE(Irp)       \
    CAST_LVALUE(PIRP_CONTEXT,IoGetCurrentIrpStackLocation(Irp)->    \
                Parameters.Others.Argument4)

/*****************************************************************************
 * CSpinMutant
 *****************************************************************************
 * "Mutated" spin lock that can be reacquired by the thread currently holding
 * the lock.
 */

class CSpinMutant
{
    KSPIN_LOCK  Lock;   
    PKTHREAD    Owner;              //  Thread "owning" the lock

    ULONG       AcquireCount;       //  Number of times the lock has been acquired
    KIRQL       OriginalIrql;       //  At original acquisition

public:
    CSpinMutant() {
        KeInitializeSpinLock(&Lock); 
        Owner = NULL; 
        AcquireCount = 0; 
        OriginalIrql = PASSIVE_LEVEL;
    }
    ~CSpinMutant() {ASSERT((0 == AcquireCount) && (NULL == Owner) ); }
    void    Acquire();
    void    Release();
};

/*****************************************************************************
 * DMA_CONTEXT
 *****************************************************************************
 * Context for DMA operations- used in IoAllocateAdapterChannel() callback.
 * Changes to this structure should only be made while revoke lock is held.
 */
struct DMA_CONTEXT
{
    class CIrpStream*   Stream;
    PPACKET_HEADER      PacketHeader;
    // Used for MdlAddress, MapRegisterBase (out)
    // Queue references this also
    PIRP                Irp;
    // Used for partial mappings
    ULONG               BytesThisMapping;
    // Used for partial mappings
    bool                LastSubPacket;
    // Used so deferred IRPs don't get completed until callback
    bool                Cancelled;
    // Track when we are queued on adapter
    bool                Queued;
    // Must the DPC release the adapter (only if queued by DMA callback)?
    bool                MustReleaseAdapter;
    //  The state will be one of these...
    enum {Idle, CallbackPending, SynchronousCallback, DeferredCallback}
    // Used to track operation state
                        State;
    //  Used so destructor does not complete before any pending callbacks
    //  occur.
    KEVENT              HangingCallbackCompleted;
private:
    // DPC for notification that stream can again get mappings.
    KDPC                MappingsAvailable;

public:
    void    Init(CIrpStream* Owner);
    void    ReportNewMappings(bool DeferredCallback = false);
    void    WaitForAnyHangingCallbacks();
};

/*****************************************************************************
 * CIrpStream
 *****************************************************************************
 * Irp stream implementation.
 */
class CIrpStream : public IIrpStreamVirtual,
                   public IIrpStreamPhysical,
                   public CUnknown
{
#ifndef PC_KDEXT
private:
#else // PC_KDEXT
public:
#endif // !PC_KDEXT
    PIKSSHELLTRANSPORT m_TransportSink;
    PIKSSHELLTRANSPORT m_TransportSource;

    KSSTATE     m_ksState;
    
    BOOLEAN     m_Flushing;
    BOOLEAN     WriteOperation;
    BOOLEAN     WasExhausted;

    ULONG       ProbeFlags;
    PIRP        LockedIrp;

    DMA_CONTEXT Dma;

    CSpinMutant m_kSpinLock;
    CSpinMutant m_RevokeLock;
    KSPIN_LOCK	m_irpStreamPositionLock;

    IRPSTREAM_POSITION  m_irpStreamPosition;

    ULONGLONG   InputPosition;
    ULONGLONG   OutputPosition;

    PADAPTER_OBJECT BusMasterAdapterObject;
    PDEVICE_OBJECT  FunctionalDeviceObject;
    ULONG   MaxMapRegisters;

    PIRPSTREAMNOTIFY            Notify;
    PIRPSTREAMNOTIFYPHYSICAL    NotifyPhysical;

    LIST_ENTRY  LockedQueue;
    KSPIN_LOCK  LockedQueueLock;
    LIST_ENTRY  MappedQueue;
    KSPIN_LOCK  MappedQueueLock;
    
    struct
    {
        PMAPPING_QUEUE_ENTRY    Array;
        ULONG                   Head;
        ULONG                   Tail;
        ULONG                   Get;
    }   MappingQueue;

    //  Queue linkages used by SynchronizedDMAQueue;
    CIrpStream* NextWaiter;
    CIrpStream* PreviousWaiter; //  Used for fast removals ONLY


    ULONG       MappingsOutstanding;
#if (DBG)
	ULONG       MappingsQueued;

    ULONG       IrpLabel;
    ULONG       IrpLabelToComplete;

    ULONGLONG   timeStep;
    ULONG       irpCompleteCount;

    PCSTR       MappingIrpOwner;
    PCSTR       UnmappingIrpOwner;
#endif

    PIRP AcquireMappingIrp
    (
#if DBG
        IN      PCSTR       Owner,
#endif
        IN      BOOLEAN     NotifyExhausted
    );

    PIRP AcquireUnmappingIrp
    (
#if DBG
        IN      PCSTR   Owner
#endif
    );

    void ReleaseMappingIrp
    (
        IN      PIRP            Irp,
        IN      PPACKET_HEADER  PacketHeader    OPTIONAL
    );

    void ReleaseUnmappingIrp
    (
        IN      PIRP            Irp,
        IN      PPACKET_HEADER  PacketHeader    OPTIONAL
    );

    unsigned AvailableMappingQueueEntries();

    NTSTATUS EnqueueMapping
    (
        IN      PHYSICAL_ADDRESS    PhysicalAddress,
        IN      PIRP                Irp,
        IN      PPACKET_HEADER      PacketHeader,
        IN      PVOID               VirtualAddress,
        IN      ULONG               ByteCount,
        IN      ULONG               Flags,
        IN      PVOID               MapRegisterBase,
        IN      ULONG               MappingStatus,
        IN      PVOID               SubpacketVa,
        IN      ULONG               SubpacketBytes
    );

    PMAPPING_QUEUE_ENTRY GetQueuedMapping
    (   void
    );

    PMAPPING_QUEUE_ENTRY DequeueMapping
    (   void
    );

    void
    CancelMappings
    (
        IN      PIRP    pIrp
    );

    void DbgQueues
    (   void
    );

    friend SynchronizedDMAQueue;    //  So it can queue us on adapter
    friend DMA_CONTEXT;             //  So it can call NotifyPhysical, etc.

public:
    DECLARE_STD_UNKNOWN();
    DEFINE_STD_CONSTRUCTOR(CIrpStream);
    ~CIrpStream();

    IMP_IIrpStreamVirtual;
    IMP_IIrpStreamPhysical_;

    /*************************************************************************
     * Friends
     */
    friend
    static
    DRIVER_CONTROL CallbackFromIoAllocateAdapterChannel;

    friend
    DRIVER_CANCEL IrpStreamCancelRoutine;

    friend
    KDEFERRED_ROUTINE ReportMapAvailable;

#ifdef PC_KDEXT
    //  Debugger extension routines
    friend
    VOID
    PCKD_AcquireDeviceData
    (
        PDEVICE_CONTEXT     DeviceContext,
        PLIST_ENTRY         SubdeviceList,
        ULONG               Flags
    );
    friend
    VOID
    PCKD_AcquireIrpStreamData
    (
        PVOID           CurrentPinEntry,
        CIrpStream     *RemoteIrpStream,
        CIrpStream     *LocalIrpStream    
    );
#endif
};

#ifndef PC_KDEXT
/*****************************************************************************
 * Factory.
 */

#pragma code_seg("PAGE")

/*****************************************************************************
 * CreateIrpStream()
 *****************************************************************************
 * Creates an IrpStream object.
 */
NTSTATUS
CreateIrpStream
(
    OUT     PUNKNOWN *  Unknown,
    IN      REFCLSID,
    IN      PUNKNOWN    UnknownOuter    OPTIONAL,
    IN      POOL_TYPE   PoolType
)
{
    PAGED_CODE();

    ASSERT(Unknown);

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_IRPSTRM,
                         "[CreateIrpStream]" );

    STD_CREATE_BODY_( CIrpStream,
                      Unknown,
                      UnknownOuter,
                      PoolType,
                      PIRPSTREAMVIRTUAL );
}

/*****************************************************************************
 * PcNewIrpStreamVirtual()
 *****************************************************************************
 * Creates and initializes an IrpStream object with a virtual access
 * interface.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcNewIrpStreamVirtual
(
    OUT     PIRPSTREAMVIRTUAL * OutIrpStreamVirtual,
    IN      PUNKNOWN            OuterUnknown    OPTIONAL,
    IN      BOOLEAN             WriteOperation,
    IN      PKSPIN_CONNECT      PinConnect,
    IN      PDEVICE_OBJECT      DeviceObject
)
{
    PAGED_CODE();

    ASSERT(OutIrpStreamVirtual);
    ASSERT(PinConnect);
    ASSERT(DeviceObject);

    PUNKNOWN    unknown;
    NTSTATUS    ntStatus = CreateIrpStream( &unknown,
                                            GUID_NULL,
                                            OuterUnknown,
                                            NonPagedPoolNx );

    if(NT_SUCCESS(ntStatus))
    {
        PIRPSTREAMVIRTUAL irpStream;
        ntStatus = unknown->QueryInterface( IID_IIrpStreamVirtual,
                                            (PVOID *) &irpStream );

        if(NT_SUCCESS(ntStatus))
        {
            ntStatus = irpStream->Init( WriteOperation,
                                        PinConnect,
                                        DeviceObject,
                                        NULL,
                                        0 );

            if(NT_SUCCESS(ntStatus))
            {
                *OutIrpStreamVirtual = irpStream;
                DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                     PCTRACE_IRPSTRM,
                                     "[PcNewIrpStreamVirtual] IrpStream=%p",
                                     *OutIrpStreamVirtual );
            }
            else
            {
                irpStream->Release();
            }
        }

        unknown->Release();
    }

    if(!NT_SUCCESS(ntStatus))
    {
        DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                             PCTRACE_IRPSTRM,
                             "[PcNewIrpStreamVirtual] FAILED 0x%x",
                             ntStatus );
    }

    return ntStatus;
}

/*****************************************************************************
 * PcNewIrpStreamPhysical()
 *****************************************************************************
 * Creates and initializes an IrpStream object with a physical access
 * interface.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcNewIrpStreamPhysical
(
    OUT     PIRPSTREAMPHYSICAL *    OutIrpStreamPhysical,
    IN      PUNKNOWN                OuterUnknown    OPTIONAL,
    IN      BOOLEAN                 WriteOperation,
    IN      PKSPIN_CONNECT          PinConnect,
    IN      PDEVICE_OBJECT          DeviceObject,
    IN      PADAPTER_OBJECT         AdapterObject,
    IN      ULONG                   MaxMappingRegisters
)
{
    PAGED_CODE();

    ASSERT(OutIrpStreamPhysical);
    ASSERT(DeviceObject);
    ASSERT(AdapterObject);
    ASSERT(MaxMappingRegisters);

    PUNKNOWN    unknown;
    NTSTATUS    ntStatus = CreateIrpStream( &unknown,
                                            GUID_NULL,
                                            OuterUnknown,
                                            NonPagedPoolNx );

    if(NT_SUCCESS(ntStatus))
    {
        PIRPSTREAMPHYSICAL irpStream;
        ntStatus = unknown->QueryInterface( IID_IIrpStreamPhysical,
                                            (PVOID *) &irpStream );

        if(NT_SUCCESS(ntStatus))
        {
            ntStatus = irpStream->Init( WriteOperation,
                                        PinConnect,
                                        DeviceObject,
                                        AdapterObject,
                                        MaxMappingRegisters );

            if(NT_SUCCESS(ntStatus))
            {
                *OutIrpStreamPhysical = irpStream;
                DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                     PCTRACE_IRPSTRM,
                                     "[PcNewIrpStreamPhysical] IrpStream=%p",
                                     *OutIrpStreamPhysical );
            }
            else
            {
                irpStream->Release();
            }
        }

        unknown->Release();
    }

    if(!NT_SUCCESS(ntStatus))
    {
        DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                             PCTRACE_IRPSTRM,
                             "[PcNewIrpStreamPhysical] FAILED 0x%x",
                             ntStatus );
    }

    return ntStatus;
}





/*****************************************************************************
 * Member functions.
 */

/*****************************************************************************
 * CIrpStream::~CIrpStream()
 *****************************************************************************
 * Destructor.
 */
CIrpStream::
~CIrpStream
(   void
)
{
    PAGED_CODE();

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_IRPSTRM,
                         "[IrpStrm%p::~CIrpStream]",
                         this );

    CancelAllIrps( TRUE );  // reset position counters

    if(Notify)
    {
        Notify->Release();
    }

    if(NotifyPhysical)
    {
        NotifyPhysical->Release();
    }

    if(MappingQueue.Array)
    {
        // Should the number of entries in this queue be zero?
        ExFreePool(MappingQueue.Array);
    }
    //  Wait if there are any hanging DMA callbacks
    Dma.WaitForAnyHangingCallbacks();
}

/*****************************************************************************
 * CIrpStream::NonDelegatingQueryInterface()
 *****************************************************************************
 * Obtains an interface.
 */
STDMETHODIMP_(NTSTATUS)
CIrpStream::
NonDelegatingQueryInterface
(
    __in    REFIID  Interface,
    __out   PVOID * Object
)
{
    PAGED_CODE();

    ASSERT(Object);

    if(IsEqualGUIDAligned(Interface,IID_IUnknown))
    {
        *Object = PVOID(PUNKNOWN(PIRPSTREAMVIRTUAL(this)));
    }
    else
        if(IsEqualGUIDAligned(Interface,IID_IIrpStreamSubmit))
    {
        *Object = PVOID(PIRPSTREAMSUBMIT(PIRPSTREAMVIRTUAL(this)));
    }
    else
        if(IsEqualGUIDAligned(Interface,IID_IIrpStream))
    {
        *Object = PVOID(PIRPSTREAM(PIRPSTREAMVIRTUAL(this)));
    }
    else
        if(IsEqualGUIDAligned(Interface,IID_IKsShellTransport))
    {
        *Object = PVOID(PIKSSHELLTRANSPORT(PIRPSTREAMVIRTUAL(this)));
    }
    else
        if(IsEqualGUIDAligned(Interface,IID_IIrpStreamVirtual))
    {
        // Only valid if not configured for physical access.
        if(BusMasterAdapterObject)
        {
            *Object = NULL;
        }
        else
        {
            *Object = QICAST(PIRPSTREAMVIRTUAL);
        }
    }
    else
        if(IsEqualGUIDAligned(Interface,IID_IIrpStreamPhysical))
    {
        // Only valid if configured for physical access or uninitialized.
        if(BusMasterAdapterObject || (ProbeFlags == 0))
        {
            *Object = QICAST(PIRPSTREAMPHYSICAL);
        }
        else
        {
            *Object = NULL;
        }
    }
    else
    {
        *Object = NULL;
    }

    if(*Object)
    {
        PUNKNOWN(*Object)->AddRef();
        return STATUS_SUCCESS;
    }

    return STATUS_INVALID_PARAMETER;
}

/*****************************************************************************
 * GetPartialMdlCountForMdl()
 *****************************************************************************
 * Determine number of partial MDLs that are required for a source MDL.
 */
ULONG
GetPartialMdlCountForMdl
(
    IN      PVOID   Va,
    IN      ULONG   Size
)
{
    PAGED_CODE();

    ULONG result = 1;

    if(Size > WHOLE_MDL_THRESHOLD)
    {
        ULONG pageCount = ADDRESS_AND_SIZE_TO_SPAN_PAGES(Va,Size);

        if(BYTE_OFFSET(Va) > PRE_OFFSET_THRESHOLD)
        {
            pageCount--;
        }

        if(BYTE_OFFSET(PVOID(PBYTE(Va) + Size - 1)) < POST_OFFSET_THRESHOLD)
        {
            pageCount--;
        }

        result = (pageCount + MAX_PAGES_PER_MDL - 1) / MAX_PAGES_PER_MDL;
    }

    return result;
}

STDMETHODIMP_(NTSTATUS)
CIrpStream::
TransferKsIrp
(
    IN PIRP Irp,
    OUT PIKSSHELLTRANSPORT* NextTransport
)

/*++

Routine Description:

    This routine handles the arrival of a streaming IRP via the shell 
    transport.

Arguments:

    Irp -
        Contains a pointer to the streaming IRP submitted to the queue.

    NextTransport -
        Contains a pointer to a location at which to deposit a pointer
        to the next transport interface to receive the IRP.  May be set
        to NULL indicating the IRP should not be forwarded further.

Return Value:

    STATUS_SUCCESS, STATUS_PENDING or some error status.

--*/

{
    PAGED_CODE();

    ASSERT(Irp);
    ASSERT(NextTransport);
    ASSERT(m_TransportSink);
    ASSERT(m_TransportSource);

    PcWmiLogEvent( TRACE_LEVEL_VERBOSE,
                   PC_ETYPE_IRP_RECEIVED,
                   PortCls_IrpProcessing_GUID,
                   (ULONGLONG)Irp,
                   0,
                   0,
                   0,
                   NULL );

    //
    // Shunt IRPs to the next object if we are not ready.
    //
    if(m_Flushing || (m_ksState == KSSTATE_STOP) || Irp->Cancel || 
       ! NT_SUCCESS(Irp->IoStatus.Status))
    {
        *NextTransport = m_TransportSink;
        return STATUS_SUCCESS;
    }

    //
    // Not smart enough to do this yet.
    //
    *NextTransport = NULL;

    //
    // Prepare the IRP using KS's handiest function.
    // We skip if the IRP came from a portcls pin we know has already done the probe    
    //
    NTSTATUS ntStatus;

    if(ProbeFlags)
    {
        ntStatus = KsProbeStreamIrp( Irp,
                                     ProbeFlags,
                                     0 );
    }
    else    
    {
        ntStatus = STATUS_SUCCESS;
    }

#pragma prefast(disable: __WARNING_BUFFER_OVERFLOW, "Erroneous buffer overflow declared in following code")

    PIRP_CONTEXT irpContext = NULL;
    if(NT_SUCCESS(ntStatus))
    {
        ntStatus = STATUS_PENDING;

        ULONG packetCount = 0;

        //
        // Count the number of 'packet headers' we will be needing.
        //
        PKSSTREAM_HEADER streamHeader = FIRST_STREAM_HEADER_IRP_STORAGE(Irp);

        if( (!streamHeader->DataUsed && WriteOperation) ||
            (!streamHeader->FrameExtent && !WriteOperation) )
        {
            //
            // At least one for the Irp context.
            //
            packetCount = 1;
        }
        else
        {
            for(PMDL mdl = Irp->MdlAddress; mdl; mdl = mdl->Next, streamHeader++)
            {
                packetCount++;
            }
        }

        irpContext = PIRP_CONTEXT( ExAllocatePoolWithTag( NonPagedPoolNx,
                                                          ( sizeof(IRP_CONTEXT) +
                                                            ( sizeof(PACKET_HEADER) *
                                                              (packetCount - 1) ) ),
                                                          POOL_TAG_IRPSTREAM_IRP_CONTEXT ) );

        if(irpContext)
        {
            IRP_CONTEXT_IRP_STORAGE(Irp) = irpContext;
        }
        else
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    if(NT_SUCCESS(ntStatus))
    {
        irpContext->SubmissionProcess = IoGetCurrentProcess();

        irpContext->LockingPacket =
        irpContext->MappingPacket =
        irpContext->UnmappingPacket = &irpContext->Packets[0];

        irpContext->IrpStream = PVOID(this);

#if (DBG)
        irpContext->IrpLabel = IrpLabel++;
#endif

        PKSSTREAM_HEADER    streamHeader    = FIRST_STREAM_HEADER_IRP_STORAGE(Irp);
        PPACKET_HEADER      packetHeader    = &irpContext->Packets[0];
        PPACKET_HEADER      firstLooped     = NULL;
        PPACKET_HEADER      prevLooped      = NULL;

        if( (!streamHeader->DataUsed && WriteOperation) ||
            (!streamHeader->FrameExtent && !WriteOperation) )
        {
            RtlZeroMemory( packetHeader, sizeof( PACKET_HEADER ) );
            packetHeader->MapCount          = 1;
            packetHeader->StreamHeader      = streamHeader;
            packetHeader->InputPosition     = InputPosition;
            packetHeader->OutputPosition    = OutputPosition;
        }
        else
        {
            for(PMDL mdl = Irp->MdlAddress;
               mdl;
               mdl = mdl->Next, streamHeader++)
            {
                ULONG bytesRemaining = ( WriteOperation ?
                                         streamHeader->DataUsed :
                                         streamHeader->FrameExtent );

                m_irpStreamPosition.ullCurrentExtent += bytesRemaining;

                if(   streamHeader->OptionsFlags
                      &   KSSTREAM_HEADER_OPTIONSF_LOOPEDDATA
                  )
                {
                    if(prevLooped)
                    {
                        // Point the previous looped packet to this one.
                        prevLooped->Next = packetHeader;
                    }
                    else
                    {
                        // No previous looped packet.
                        firstLooped = packetHeader;
                    }

                    prevLooped = packetHeader;
                }


                packetHeader->StreamHeader      = streamHeader;
                packetHeader->MdlAddress        = mdl;
                packetHeader->BytesTotal        = bytesRemaining;
                packetHeader->MapPosition       = 0;
                packetHeader->UnmapPosition     = 0;
                packetHeader->MapCount          =
                (packetHeader == &irpContext->Packets[0]) ? 1 : 0;
                packetHeader->IncrementMapping =
                packetHeader->IncrementUnmapping = (mdl->Next != NULL);
                packetHeader->Next              = firstLooped;

                packetHeader->InputPosition     = InputPosition;
                packetHeader->OutputPosition    = OutputPosition;

                InputPosition += packetHeader->BytesTotal;

                packetHeader++;
            }
        }


#pragma prefast(enable: __WARNING_BUFFER_OVERFLOW, "Erroneous buffer overflow detected by prefast in previous code")

#if (DBG)
        DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                             PCTRACE_IRPSTRM,
                             "[IrpStrm%p::TransferKsIrp] Irp Received %d %p",
                             this,
                             (UINT)IrpLabel,
                             Irp );
#else
        DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                             PCTRACE_IRPSTRM,
                             "[IrpStrm%p::TransferKsIrp] Irp Received %p",
                             this,
                             Irp );
#endif

        IoMarkIrpPending(Irp);
        
        // IRP is locked down in advance and ready to map.
        KsAddIrpToCancelableQueue( &LockedQueue,
                                   &LockedQueueLock,
                                   Irp,
                                   KsListEntryTail,
                                   IrpStreamCancelRoutine );

        // need to change WasExhausted BEFORE notifying the sink since
        // notifying the sink may result in starvation again.
        BOOLEAN TempWasExhausted = WasExhausted;
        WasExhausted = FALSE;

        if(Notify)
        {
            DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                 PCTRACE_IRPSTRM,
                                 "[IrpStrm%p::TransferKsIrp] Notify %s",
                                 this,
                                 TempWasExhausted?"E":"NE" );
            Notify->IrpSubmitted(Irp,TempWasExhausted);
        }
        else
            if(NotifyPhysical)
        {
            DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                 PCTRACE_IRPSTRM,
                                 "[IrpStrm%p::TransferKsIrp] NotifyPhysical %s",
                                 this,
                                 TempWasExhausted?"E":"NE" );
            NotifyPhysical->IrpSubmitted(Irp,TempWasExhausted);
        }

        ntStatus = STATUS_PENDING;
    }

    return ntStatus;
}

#pragma code_seg()

/*****************************************************************************
 * CIrpStream::GetPosition()
 *****************************************************************************
 * Gets the current position.
 */
STDMETHODIMP_(NTSTATUS)
CIrpStream::
GetPosition
(
    OUT     PIRPSTREAM_POSITION pIrpStreamPosition
)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    KIRQL oldIrql;

    KeAcquireSpinLock(&m_irpStreamPositionLock, &oldIrql );
    *pIrpStreamPosition = m_irpStreamPosition;

    //
    // Assume stream position and unmapping position are the same.
    //
    pIrpStreamPosition->ullStreamPosition = pIrpStreamPosition->ullUnmappingPosition;

    //
    // Assume no physical offset.
    //
    pIrpStreamPosition->ullPhysicalOffset = 0;

    //
    // Give the notification sink a chance to modify the position.
    //
    if(Notify)
    {
        ntStatus = Notify->GetPosition(pIrpStreamPosition);
    }
    else
        if(NotifyPhysical)
    {
        ntStatus = NotifyPhysical->GetPosition(pIrpStreamPosition);
    }

    KeReleaseSpinLock(&m_irpStreamPositionLock, oldIrql);

    return ntStatus;
}

#pragma code_seg("PAGE")


STDMETHODIMP_(void)
CIrpStream::
Connect
(
    IN PIKSSHELLTRANSPORT NewTransport OPTIONAL,
    OUT PIKSSHELLTRANSPORT *OldTransport OPTIONAL,
    IN KSPIN_DATAFLOW DataFlow
)

/*++

Routine Description:

    This routine establishes a shell transport connect.

Arguments:

Return Value:

--*/
{
    PAGED_CODE();

    KsShellStandardConnect( NewTransport,
                            OldTransport,
                            DataFlow,
                            PIKSSHELLTRANSPORT(PIRPSTREAMVIRTUAL(this)),
                            &m_TransportSource,
                            &m_TransportSink);
}


STDMETHODIMP_(NTSTATUS)
CIrpStream::
SetDeviceState
(
    IN KSSTATE NewState,
    IN KSSTATE OldState,
    OUT PIKSSHELLTRANSPORT* NextTransport
)
/*++

Routine Description:

    This routine handles notification that the device state has changed.

Arguments:

Return Value:

--*/
{
    PAGED_CODE();

    ASSERT(NextTransport);

    DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                         PCTRACE_DEVSTATE,
                         "[IrpStrm%p::SetDeviceState] from %d to %d",
                         this,
                         OldState,
                         NewState);

    if(m_ksState != NewState)
    {
        m_ksState = NewState;

        if(NewState > OldState)
        {
            *NextTransport = m_TransportSink;
        }
        else
        {
            *NextTransport = m_TransportSource;
        }

        if(NewState == KSSTATE_STOP)
        {
            if(! WriteOperation)
            {
                TerminatePacket();
            }

            CancelAllIrps(TRUE);
        }

        //
        // Adjust the active pin count
        //
        if( (NewState == KSSTATE_RUN) && (OldState != KSSTATE_RUN) )
        {
            UpdateActivePinCount( PDEVICE_CONTEXT(FunctionalDeviceObject->DeviceExtension),
                                  TRUE );

            // Adjust the stream base position
            if(NotifyPhysical)
            {
                NTSTATUS ntStatus = NotifyPhysical->GetPosition(&m_irpStreamPosition);
                if( NT_SUCCESS(ntStatus) )
                {
                    m_irpStreamPosition.ullStreamOffset = m_irpStreamPosition.ullStreamPosition -
                                                          m_irpStreamPosition.ullUnmappingPosition;
                }
            }
        }
        else
        {
            if( (NewState != KSSTATE_RUN) && (OldState == KSSTATE_RUN) )
            {
                UpdateActivePinCount( PDEVICE_CONTEXT(FunctionalDeviceObject->DeviceExtension),
                                      FALSE );
            }
        }

    }
    else
    {
        *NextTransport = NULL;
    }

    return STATUS_SUCCESS;
}


STDMETHODIMP_(void)
CIrpStream::
SetResetState
(
    IN  KSRESET ksReset,
    OUT PIKSSHELLTRANSPORT* NextTransport
)
/*++

Routine Description:

    This routine handles notification that the reset state has changed.

Arguments:

Return Value:

--*/

{
    PAGED_CODE();

    ASSERT(NextTransport);

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_IRPSTRM,
                         "[IrpStrm%p::SetResetState] %d",
                         this,
                         (UINT)ksReset );

    //
    // Take no action if we were already in this state.
    //
    if(m_Flushing != (ksReset == KSRESET_BEGIN))
    {
        //
        // Tell the caller to forward the state change to our sink.
        //
        *NextTransport = m_TransportSink;

        //
        // Set our local copy of the state.
        //
        m_Flushing = (ksReset == KSRESET_BEGIN);

        //
        // Flush the queues if we are beginning a reset.
        //
        if(m_Flushing)
        {
            if(! WriteOperation)
            {
                TerminatePacket();
            }
            CancelAllIrps(TRUE);
        }
    }
    else
    {
        *NextTransport = NULL;
    }
}

/*****************************************************************************
 * CIrpStream::Init()
 *****************************************************************************
 * Initializes the object.
 */
STDMETHODIMP_(NTSTATUS)
CIrpStream::
Init
(
    IN      BOOLEAN         WriteOperation_,
    IN      PKSPIN_CONNECT  PinConnect,
    IN      PDEVICE_OBJECT  DeviceObject,
    IN      PADAPTER_OBJECT AdapterObject   OPTIONAL,
    IN      ULONG           MaxMappingRegisters
)
{
    PAGED_CODE();

    ASSERT(DeviceObject);

    NTSTATUS ntStatus = STATUS_SUCCESS;

#if (DBG)
    timeStep = PcGetTimeInterval(0);
    irpCompleteCount = 0;
#endif

    m_ksState = KSSTATE_STOP;
    m_irpStreamPosition.bLoopedInterface =
        ( PinConnect->Interface.Id == KSINTERFACE_STANDARD_LOOPED_STREAMING );

    InputPosition           = 0;
    OutputPosition          = 0;
    WriteOperation          = WriteOperation_;
    FunctionalDeviceObject  = DeviceObject;
    BusMasterAdapterObject  = AdapterObject;
    MaxMapRegisters         = MaxMappingRegisters;
    ProbeFlags              = (( WriteOperation ?
                                 KSPROBE_STREAMWRITE :
                                 KSPROBE_STREAMREAD ) |
                               KSPROBE_PROBEANDLOCK | 
                               KSPROBE_SYSTEMADDRESS |
                               KSPROBE_ALLOCATEMDL );
    WasExhausted            = TRUE;

    MappingsOutstanding     = 0;
#if (DBG)
	MappingsQueued          = 0;
#endif

    KeInitializeSpinLock(&m_irpStreamPositionLock);
    Dma.Init(this);
    NextWaiter = PreviousWaiter = NULL;

    InitializeListHead(&LockedQueue);
    KeInitializeSpinLock(&LockedQueueLock);

    InitializeListHead(&MappedQueue);
    KeInitializeSpinLock(&MappedQueueLock);

    //
    // Source pins are a no-no for Longhorn and beyond
    //
    if(PinConnect->PinToHandle)
    {
        ntStatus = STATUS_INVALID_DEVICE_REQUEST;
    }

    // allocate the mapping array
    if(NT_SUCCESS(ntStatus) && BusMasterAdapterObject)
    {
        MappingQueue.Array = PMAPPING_QUEUE_ENTRY( ExAllocatePoolWithTag( NonPagedPoolNx,
                                                                          sizeof(MAPPING_QUEUE_ENTRY) * MAPPING_QUEUE_SIZE,
                                                                          'qMcP' ) );

        if(! MappingQueue.Array)
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    if(NT_SUCCESS(ntStatus))
    {
        DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                             PCTRACE_IRPSTRM,
                             "[IrpStrm%p::Init]",
                             this );
    } else
    {
        DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                             PCTRACE_IRPSTRM,
                             "[IrpStrm%p::Init] FAILED 0x%x",
                             this,
                             ntStatus );
    }

    return ntStatus;
}

#pragma code_seg()

/*****************************************************************************
 * CIrpStream::CancelAllIrps()
 *****************************************************************************
 * Cancel all IRPs.
 */
STDMETHODIMP_(void)
CIrpStream::CancelAllIrps
(
    IN BOOL ClearPositionCounters
)
{
    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_IRPSTRM,
                         "[IrpStrm%p::CancelAllIrps] %s",
                         this,
                         FALSE!=ClearPositionCounters?"T":"F" );
    
    KsCancelIo( &LockedQueue,
                &LockedQueueLock );
    
    KsCancelIo( &MappedQueue,
                &MappedQueueLock );

    // grab the revoke spinlock (must always grab this BEFORE the master spinlock)
    m_RevokeLock.Acquire();

    // grab the master spinlock
    m_kSpinLock.Acquire();

    //
    // clear the input and output position counts
    //
    BOOLEAN bLooped = m_irpStreamPosition.bLoopedInterface;
    ULONGLONG ullStreamOffset = m_irpStreamPosition.ullStreamOffset;
    RtlZeroMemory(&m_irpStreamPosition,sizeof(m_irpStreamPosition));
    m_irpStreamPosition.bLoopedInterface = bLooped;
    m_irpStreamPosition.ullStreamOffset = ullStreamOffset;

    if(ClearPositionCounters)
    {
        InputPosition = 0;
        OutputPosition = 0;
    }
    
    // release the spinlocks, master first
    m_kSpinLock.Release();
    m_RevokeLock.Release();
}

/*****************************************************************************
 * CIrpStream::TerminatePacket()
 *****************************************************************************
 * Bypasses all remaining mappings for the current packet.
 */
STDMETHODIMP_(void)
CIrpStream::
TerminatePacket
(   void
)
{
    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_IRPSTRM,
                         "[IrpStrm%p::TerminatePacket]",
                         this );

    if(BusMasterAdapterObject)
    {
        // TODO:  What do we do for WavePCI? BUG 662565
    }
    else
    {
        m_RevokeLock.Acquire();

        PIRP irp = DbgAcquireUnmappingIrp("TerminatePacket");
        if(irp)
        {
            PPACKET_HEADER packetHeader = IRP_CONTEXT_IRP_STORAGE(irp)->UnmappingPacket;

            //
            // The mapping window should be closed.
            //
            if( (packetHeader->MapCount == 1) &&
                (packetHeader->MapPosition == packetHeader->UnmapPosition) &&
                (packetHeader->MapPosition != 0) )
            {
                //
                // Adjust for unused extent.
                //
                if(m_irpStreamPosition.ullCurrentExtent != ULONGLONG(-1))
                {
                    m_irpStreamPosition.ullCurrentExtent +=
                    packetHeader->UnmapPosition;
                    m_irpStreamPosition.ullCurrentExtent -=
                    packetHeader->BytesTotal;
                }

                //
                // We are not at the start of the packet, and this packet
                // should be terminated.  The adjusted BytesTotal will get
                // copied back into DataUsed in the stream header.
                //
                packetHeader->BytesTotal = packetHeader->UnmapPosition;
            }
            else
            {
                //
                // We are at the start of the packet or the packet window is
                // not closed.
                //
                packetHeader = NULL;
            }

            ReleaseUnmappingIrp(irp,packetHeader);
        }

        m_RevokeLock.Release();
    }
}

/*****************************************************************************
 * CIrpStream::ChangeOptionsFlags()
 *****************************************************************************
 * Change the flags for the current mapping and unmapping IRPs.
 *
 * "Mapping" IRP is the packet currently submitted to the device
 * "Unmapping" IRP is the packet currently completed by the device
 */

STDMETHODIMP_(NTSTATUS)
CIrpStream::
ChangeOptionsFlags
(
    IN  ULONG   MappingOrMask,
    IN  ULONG   MappingAndMask,
    IN  ULONG   UnmappingOrMask,
    IN  ULONG   UnmappingAndMask
)
{
    PIRP            pIrp;
    PPACKET_HEADER  pPacketHeader;
    ULONG           oldOptionsFlags;
    NTSTATUS        ntStatus = STATUS_SUCCESS;

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_IRPSTRM,
                         "[IrpStrm%p::ChangeOptionsFlags]",
                         this );

    if((MappingOrMask) || (~MappingAndMask))
    {
        m_RevokeLock.Acquire();

        pIrp = DbgAcquireMappingIrp("ChangeOptionsFlags",FALSE);
        if(pIrp)
        {
            pPacketHeader = IRP_CONTEXT_IRP_STORAGE(pIrp)->MappingPacket;
            if((pPacketHeader) && (pPacketHeader->StreamHeader))
            {
                oldOptionsFlags = pPacketHeader->StreamHeader->OptionsFlags;
                oldOptionsFlags |= MappingOrMask;
                oldOptionsFlags &= MappingAndMask;
                pPacketHeader->StreamHeader->OptionsFlags = oldOptionsFlags;
            }
            else
                ntStatus = STATUS_UNSUCCESSFUL;
            ReleaseMappingIrp(pIrp,NULL);
        }
        else
        {
            ntStatus = STATUS_UNSUCCESSFUL;
        }

        m_RevokeLock.Release();
    }

    if((UnmappingOrMask) || (~UnmappingAndMask))
    {
        m_RevokeLock.Acquire();

        pIrp = DbgAcquireUnmappingIrp("ChangeOptionsFlags");
        if(pIrp)
        {
            pPacketHeader = IRP_CONTEXT_IRP_STORAGE(pIrp)->UnmappingPacket;
            if((pPacketHeader) && (pPacketHeader->StreamHeader))
            {
                oldOptionsFlags = pPacketHeader->StreamHeader->OptionsFlags;
                oldOptionsFlags |= UnmappingOrMask;
                oldOptionsFlags &= UnmappingAndMask;
                pPacketHeader->StreamHeader->OptionsFlags = oldOptionsFlags;
            }
            else
                ntStatus = STATUS_UNSUCCESSFUL;
            ReleaseUnmappingIrp(pIrp,NULL);
        }
        else
        {
            ntStatus = STATUS_UNSUCCESSFUL;
        }

        m_RevokeLock.Release();
    }

    return ntStatus;
}

/*****************************************************************************
 * CIrpStream::GetPacketInfo()
 *****************************************************************************
 * Get information about the current packet.
 *
 * "Mapping" information is the packet information currently
 *      submitted to the device
 * "Unmapping" information is the packet information currently 
 *      completed by the device
 *
 * OutputPosition is the unrolled position of the stream, e.g. the total
 *      number of bytes to the device.
 * InputPosition is the position within the data not include the unrolled
 *      loops.
 */

STDMETHODIMP_(NTSTATUS)
CIrpStream::
GetPacketInfo
(
    OUT     PIRPSTREAMPACKETINFO    Mapping     OPTIONAL,
    OUT     PIRPSTREAMPACKETINFO    Unmapping   OPTIONAL
)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    if(Mapping)
    {
        m_RevokeLock.Acquire();

        PIRP irp = DbgAcquireMappingIrp("GetPacketInfo",FALSE);

        if(irp)
        {
            PPACKET_HEADER packetHeader =
            IRP_CONTEXT_IRP_STORAGE(irp)->MappingPacket;

            Mapping->Header         = *packetHeader->StreamHeader;
            Mapping->InputPosition  = packetHeader->InputPosition;
            Mapping->OutputPosition = packetHeader->OutputPosition;
            Mapping->CurrentOffset  = packetHeader->MapPosition;

            ReleaseMappingIrp(irp,NULL);
        }
        else
        {
            RtlZeroMemory(&Mapping->Header,sizeof(KSSTREAM_HEADER));
            Mapping->InputPosition  = InputPosition;
            Mapping->OutputPosition = OutputPosition;
            Mapping->CurrentOffset  = 0;
        }

        m_RevokeLock.Release();
    }

    if(NT_SUCCESS(ntStatus) && Unmapping)
    {
        m_RevokeLock.Acquire();

        PIRP irp = DbgAcquireUnmappingIrp("GetPacketInfo");

        if(irp)
        {
            PPACKET_HEADER packetHeader =
            IRP_CONTEXT_IRP_STORAGE(irp)->MappingPacket;

            Unmapping->Header         = *packetHeader->StreamHeader;
            Unmapping->InputPosition  = packetHeader->InputPosition;
            Unmapping->OutputPosition = packetHeader->OutputPosition;
            Unmapping->CurrentOffset  = packetHeader->UnmapPosition;

            ReleaseUnmappingIrp(irp,NULL);
        }
        else
        {
            RtlZeroMemory(&Unmapping->Header,sizeof(KSSTREAM_HEADER));
            Unmapping->InputPosition  = InputPosition;
            Unmapping->OutputPosition = OutputPosition;
            Unmapping->CurrentOffset  = 0;
        }

        m_RevokeLock.Release();
    }

    return ntStatus;
}

/*****************************************************************************
 * CIrpStream::SetPacketOffsets()
 *****************************************************************************
 * Set packet mapping and unmapping offsets to a specified value.
 */
STDMETHODIMP_(NTSTATUS)
CIrpStream::
SetPacketOffsets
(
    IN      ULONG   MappingOffset,
    IN      ULONG   UnmappingOffset
)
{
    NTSTATUS ntStatus;

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_IRPSTRM,
                         "[IrpStrm%p::SetPacketOffsets] MO=0x%08x UMO=0x%08x",
                         this,
                         MappingOffset,
                         UnmappingOffset );

    // grab the revoke spinlock BEFORE getting the irp (which will grab the
    // master spinlock) so that we don't deadlock
    m_RevokeLock.Acquire();

    //
    // For physical mapping, all mappings must be cancelled.
    //
    CancelMappings(NULL);

    PIRP irp = DbgAcquireMappingIrp("SetPacketOffsets",FALSE);

    if(irp)
    {
        PPACKET_HEADER packetHeader = IRP_CONTEXT_IRP_STORAGE(irp)->MappingPacket;

        //
        // Check for position offsets beyond the end of the packet
        //
        if( (MappingOffset > packetHeader->BytesTotal) ||
            (UnmappingOffset > packetHeader->BytesTotal) )
        {
            ReleaseMappingIrp(irp,NULL);

            m_RevokeLock.Release();

            ntStatus = STATUS_UNSUCCESSFUL;
        } else
        {
            packetHeader->MapPosition = MappingOffset;
            packetHeader->UnmapPosition = UnmappingOffset;

            m_irpStreamPosition.ulMappingOffset     = MappingOffset;
            m_irpStreamPosition.ullMappingPosition  = MappingOffset;

            m_irpStreamPosition.ulUnmappingOffset   = UnmappingOffset;
            m_irpStreamPosition.ullUnmappingPosition= UnmappingOffset;

            //
            // Make sure we have good packet sizes.  Normally, the packet sizes
            // are recorded in m_irpStreamPosition when the packets are accessed
            // (e.g. through GetLockedRegion or Complete).  This is generally
            // cool because the offsets are zero until this happens anyway.  In
            // this case, we have non-zero offsets and the possibility that the
            // packet has not been accessed yet, hence no valid packet sizes.
            // Here's some code to fix that.
            //
            if(m_irpStreamPosition.ulMappingPacketSize == 0)
            {
                m_irpStreamPosition.ulMappingPacketSize =
                packetHeader->BytesTotal;
            }

            if(m_irpStreamPosition.ulUnmappingPacketSize == 0)
            {
                m_irpStreamPosition.ulUnmappingPacketSize =
                packetHeader->BytesTotal;
            }

            // Adjust the stream base position
            if(NotifyPhysical)
            {
                NTSTATUS ntStatus2 = NotifyPhysical->GetPosition(&m_irpStreamPosition);
                if( NT_SUCCESS(ntStatus2) )
                {
                    m_irpStreamPosition.ullStreamOffset = m_irpStreamPosition.ullStreamPosition -
                                                          m_irpStreamPosition.ullUnmappingPosition;
                }
            }

            ReleaseMappingIrp(irp,NULL);

            m_RevokeLock.Release();

            // kick the notify sink
            if(Notify)
            {
                Notify->IrpSubmitted(NULL,TRUE);
            }
            else
                if(NotifyPhysical)
            {
                NotifyPhysical->IrpSubmitted(NULL,TRUE);
            }

            ntStatus = STATUS_SUCCESS;
        }
    }
    else
    {
        m_RevokeLock.Release();

        ntStatus = STATUS_UNSUCCESSFUL;
    }

    return ntStatus;
}

#pragma code_seg("PAGE")

/*****************************************************************************
 * CIrpStream::RegisterNotifySink()
 *****************************************************************************
 * Registers a notification sink.
 */
STDMETHODIMP_(void)
CIrpStream::
RegisterNotifySink
(
    IN      PIRPSTREAMNOTIFY    NotificationSink    OPTIONAL
)
{
    PAGED_CODE();

    if(Notify)
    {
        Notify->Release();
    }

    Notify = NotificationSink;

    if(Notify)
    {
        Notify->AddRef();
    }
}

#pragma code_seg()

/*****************************************************************************
 * CIrpStream::GetLockedRegion()
 *****************************************************************************
 * Get a locked contiguous region of the IRP stream.  This region must be
 * released using ReleaseLockedRegion() within a few microseconds.
 */
STDMETHODIMP_(void)
CIrpStream::
GetLockedRegion
(
    OUT     PULONG      ByteCount,
    OUT     PVOID *     SystemAddress
)
{
    ASSERT(ByteCount);
    ASSERT(SystemAddress);

    BOOL            Done;
    PIRP            irp;
    PPACKET_HEADER  packetHeader = NULL;

    Done = FALSE;

    // Acquire the revoke lock before the main lock to prevent deadlocks
    m_RevokeLock.Acquire();

    //
    // Find an IRP that has requires some work...
    //
    do 
    {
        irp = DbgAcquireMappingIrp("GetLockedRegion",TRUE);
        Done = TRUE;
        if(irp)
        {
            packetHeader = IRP_CONTEXT_IRP_STORAGE(irp)->MappingPacket;
            //
            // If packetHeader->BytesTotal is 0, then this packet is completed.
            //
            if(! packetHeader->BytesTotal)
            {
                packetHeader->OutputPosition = OutputPosition;
                ReleaseMappingIrp(irp,packetHeader);
                irp = NULL;
                Done = FALSE;
            }
        }
    }
    while(!Done);

    if(irp)
    {
        ASSERT(! LockedIrp);

        LockedIrp = irp;

        //
        // Record new mapping packet information in the position structure.
        //
        if(packetHeader->MapPosition == 0)
        {
            packetHeader->OutputPosition = OutputPosition;
            m_irpStreamPosition.ulMappingOffset = 0;
            m_irpStreamPosition.ulMappingPacketSize =
            packetHeader->BytesTotal;
            m_irpStreamPosition.bMappingPacketLooped =
            (   (   packetHeader->StreamHeader->OptionsFlags
                    &   KSSTREAM_HEADER_OPTIONSF_LOOPEDDATA
                )
                !=  0
            );
        }

        *ByteCount = packetHeader->BytesTotal - packetHeader->MapPosition;
        if(*ByteCount)
        {
            //  Assertion- we know we will always get system addresses because 
            //  someone has done a successful probe with KsProbeIrp (including
            //  a system address), long before we got here.

            *SystemAddress = PVOID(
#ifdef UNDER_NT
                                   PBYTE(MmGetSystemAddressForMdlSafe(packetHeader->MdlAddress,HighPagePriority | MdlMappingNoExecute))
#else
            PBYTE(MmGetSystemAddressForMdl(packetHeader->MdlAddress))
#endif
                                   + packetHeader->MapPosition );
            ASSERT(*SystemAddress != PVOID((ULONG_PTR) packetHeader->MapPosition));
        }
        else
        {
            *SystemAddress = NULL;
            LockedIrp = NULL;
            ReleaseMappingIrp(irp,NULL);
        }   
    }
    else
    {
        *ByteCount = 0;
        *SystemAddress = NULL;
        LockedIrp = NULL;
    }

    DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                         PCTRACE_IRPSTRM,
                         "[IrpStrm%p::GetLockedRegion] VA=%p BC=0x%x",
                         this,
                         *SystemAddress,
                         *ByteCount );

    if(LockedIrp == NULL)
    {
        // release the revoke lock if we don't have a locked IRP.  If we do,
        // the revoke lock will be released in ReleaseLockedRegion
        m_RevokeLock.Release();
    }

}

/*****************************************************************************
 * CIrpStream::ReleaseLockedRegion()
 *****************************************************************************
 * Releases the region previously obtained with GetLockedRegion().
 */
STDMETHODIMP_(void)
CIrpStream::
ReleaseLockedRegion
(
    IN      ULONG       ByteCount
)
{
    DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                         PCTRACE_IRPSTRM,
                         "[IrpStrm%p::ReleaseLockedRegion] BC=0x%x",
                         this,
                         ByteCount );

    if(LockedIrp)
    {
        PIRP irp = LockedIrp;

        LockedIrp = NULL;

        PPACKET_HEADER packetHeader =
        IRP_CONTEXT_IRP_STORAGE(irp)->MappingPacket;

        ULONG bytes = packetHeader->BytesTotal - packetHeader->MapPosition;
        if(bytes > ByteCount)
        {
            bytes = ByteCount;
        }

        packetHeader->MapPosition += bytes;
        m_irpStreamPosition.ullMappingPosition += bytes;
        m_irpStreamPosition.ulMappingOffset += bytes;

        if(packetHeader->MapPosition == packetHeader->BytesTotal)
        {
            OutputPosition += packetHeader->BytesTotal;
        }
        else
        {
            // ReleaseMappingIrp() wants only completed headers.
            packetHeader = NULL;
        }

        ReleaseMappingIrp(irp,packetHeader);
        m_RevokeLock.Release();
    }
}

/*****************************************************************************
 * CIrpStream::Copy()
 *****************************************************************************
 * Copy to or from locked-down memory.
 */
STDMETHODIMP_(void)
CIrpStream::
Copy
(
    IN      BOOLEAN     WriteOperation,
    IN      ULONG       RequestedSize,
    OUT     PULONG      ActualSize,
    IN OUT  PVOID       Buffer
)
{
    ASSERT(ActualSize);
    ASSERT(Buffer);

    PBYTE buffer    = PBYTE(Buffer);
    ULONG remaining = RequestedSize;

#if DBG
    ULONG loopMax = 10000;
#endif

    while(remaining)
    {
    
#if DBG
        ASSERT(loopMax--);
#endif

        ULONG   byteCount;
        PVOID   systemAddress;

        GetLockedRegion( &byteCount,
                         &systemAddress );

        if(! byteCount)
        {
            break;
        }

        if(byteCount > remaining)
        {
            byteCount = remaining;
        }

        if(WriteOperation)
        {
            RtlCopyMemory(PVOID(buffer),systemAddress,byteCount);
        }
        else
        {
            RtlCopyMemory(systemAddress,PVOID(buffer),byteCount);
        }

        ReleaseLockedRegion(byteCount);

        buffer      += byteCount;
        remaining   -= byteCount;
    }

    *ActualSize = RequestedSize - remaining;
}

/*****************************************************************************
 * CIrpStream::GetIrpStreamPositionLock()
 *****************************************************************************
 *  So we protect access to m_IrpStreamPosition
 */
STDMETHODIMP_(PKSPIN_LOCK)
CIrpStream::GetIrpStreamPositionLock()
{
   return &m_irpStreamPositionLock;
}
 

/*****************************************************************************
 * CIrpStream::Complete()
 *****************************************************************************
 * Complete.
 */
STDMETHODIMP_(void)
CIrpStream::
Complete
(
    IN      ULONG       RequestedSize,
    OUT     PULONG      ActualSize
)
{
    ASSERT(ActualSize);

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_IRPSTRM,
                         "[IrpStrm%p::Complete]",
                         this );

    if(RequestedSize == 0)
    {
        *ActualSize = 0;
        return;
    }

    ULONG   remaining = RequestedSize;
    PIRP    irp;

    // grab the revoke lock before the master lock to prevent deadlocks
    m_RevokeLock.Acquire();

#if DBG
    ULONG loopMax = 10000;
#endif

    for(irp = DbgAcquireUnmappingIrp("Complete"); 
        irp; 
        irp = DbgAcquireUnmappingIrp("Complete"))
    { 
    
#if DBG
        ASSERT(loopMax--);
#endif 

        PPACKET_HEADER packetHeader = IRP_CONTEXT_IRP_STORAGE(irp)->UnmappingPacket;

        ULONG unmapped;

        //
        // Record new unmapping packet information in the position structure.
        //
        if(packetHeader->UnmapPosition == 0)
        {
            m_irpStreamPosition.ulUnmappingOffset = 0;
            m_irpStreamPosition.ulUnmappingPacketSize = packetHeader->BytesTotal;
            m_irpStreamPosition.bUnmappingPacketLooped = ((packetHeader->StreamHeader->OptionsFlags &
                                                           KSSTREAM_HEADER_OPTIONSF_LOOPEDDATA) !=  0 );
        }

        if(packetHeader->MapCount == 1)
        {
            unmapped = packetHeader->MapPosition - packetHeader->UnmapPosition;
        }
        else
        {
            unmapped = packetHeader->BytesTotal - packetHeader->UnmapPosition;
        }

        if(unmapped > remaining)
        {
            unmapped = remaining;
        }

        remaining -= unmapped;

        packetHeader->UnmapPosition += unmapped;
        m_irpStreamPosition.ullUnmappingPosition += unmapped;
        m_irpStreamPosition.ulUnmappingOffset += unmapped;

        if(packetHeader->UnmapPosition != packetHeader->BytesTotal)
        {
            // ReleaseUnmappingIrp() wants only completed headers.
            packetHeader = NULL;
        }

        ReleaseUnmappingIrp(irp,packetHeader);

        //
        // If all IRP processing is completed (e.g., the packet header
        // has data, but the processing loop has completed the requested
        // length) then break from this loop.
        //
        if(!remaining && unmapped)
        {
            break;
        }

        //
        // If we have unmapped everything that was mapped in the packet but
        // not all of the packet bytes, kick out of the loop
        //
        if( !unmapped && !packetHeader )
        {
            break;
        }
    }

    // release the revoke lock
    m_RevokeLock.Release();

    *ActualSize = RequestedSize - remaining;
}

#pragma code_seg("PAGE")

/*****************************************************************************
 * CIrpStream::RegisterPhysicalNotifySink()
 *****************************************************************************
 * Registers a notification sink.
 */
STDMETHODIMP_(void)
CIrpStream::
RegisterPhysicalNotifySink
(
    IN      PIRPSTREAMNOTIFYPHYSICAL    NotificationSink    OPTIONAL
)
{
    PAGED_CODE();

    if(NotifyPhysical)
    {
        NotifyPhysical->Release();
    }

    NotifyPhysical = NotificationSink;

    if(NotifyPhysical)
    {
        NotifyPhysical->AddRef();
    }
}

#pragma code_seg()

/*****************************************************************************
 * CIrpStream::GetMapping()
 *****************************************************************************
 * Gets a mapping.
 */
STDMETHODIMP_(void)
CIrpStream::
GetMapping
(
    IN      PVOID               Tag,
    OUT     PPHYSICAL_ADDRESS   PhysicalAddress,
    OUT     PVOID *             VirtualAddress,
    OUT     PULONG              ByteCount,
    OUT     PULONG              Flags
)
{
    ASSERT(PhysicalAddress);
    ASSERT(VirtualAddress);
    ASSERT(ByteCount);
    ASSERT(Flags);

    //Acquire the revoke spinlock
    m_RevokeLock.Acquire();

    PMAPPING_QUEUE_ENTRY entry = GetQueuedMapping();

    // skip over any revoked mappings
    while( (NULL != entry) && (entry->MappingStatus == MAPPING_STATUS_REVOKED) )
    {
        entry = GetQueuedMapping();
    }

    if(! entry)
    {
        PIRP irp = DbgAcquireMappingIrp("GetMapping",TRUE);

        if(irp)
        {
            PPACKET_HEADER packetHeader = IRP_CONTEXT_IRP_STORAGE(irp)->MappingPacket;

            // update mapping packet info
            m_irpStreamPosition.ulMappingPacketSize = packetHeader->BytesTotal;
            m_irpStreamPosition.bMappingPacketLooped = ( ( packetHeader->StreamHeader->OptionsFlags &
                                                           KSSTREAM_HEADER_OPTIONSF_LOOPEDDATA ) != 0 );
            m_irpStreamPosition.ulMappingOffset = packetHeader->MapPosition;

            //
            // Deal with one-shot buffer or empty irp.
            //
            if( packetHeader->MapPosition == packetHeader->BytesTotal )
            {
                ReleaseMappingIrp(irp,NULL);
            }
            else
            {
                SynchronizedDMAQueue&   Adapter = 
                    PDEVICE_CONTEXT(FunctionalDeviceObject->DeviceExtension)->pExtension->Queue;

                //  See if we can have the adapter, and then check our state.
                //  If we can't have the adapter, stop for now- we'll get a 
                //  callback when its our turn.
                //  If we are not Idle, a callback must be pending.  This 
                //  automatically means that there's some sort of resource 
                //  shortage, so return no mapppings.  We will pump the mapping
                //  queue when the callback occurs.

                Dma.Queued = !Adapter.IsAvailable(this);
                if (Dma.Queued || Dma.State != DMA_CONTEXT::Idle)
                {
                    ASSERT(Dma.Queued || Dma.State == DMA_CONTEXT::DeferredCallback);

                    if  (Dma.Queued)   //  In case we get cancelled before mapping begins
                        Dma.Irp = irp;

                    *ByteCount = 0;
                    WasExhausted = TRUE;
                    ReleaseMappingIrp(irp, NULL);
                    m_RevokeLock.Release();
                    return; 
                };

                //  We can't map more bytes than we have either mapping queue entries or map registers
                //  available for.

                ULONG BytesToMap = packetHeader->BytesTotal - packetHeader->MapPosition;

                ULONG   MaximumMappingsAvailable = min(AvailableMappingQueueEntries(), MAX_MAPPINGS);

                //  We have to account for partial initial buffers in determining the maximum amount we
                //  can map at this point in time.  Only the first buffer in a packet could be affected.

                ULONG   MaximumMappableBytes = MaximumMappingsAvailable ? 
                    (PAGE_SIZE * MaximumMappingsAvailable) - (packetHeader->MapPosition ?
                        0 :MmGetMdlByteOffset(packetHeader->MdlAddress)) : 
                    0;

                ULONG   BytesThisMapping = min(BytesToMap, MaximumMappableBytes);

                BytesThisMapping = min( BytesThisMapping, MaxMapRegisters*PAGE_SIZE - ULONG((ULONGLONG(PUCHAR(MmGetMdlVirtualAddress(packetHeader->MdlAddress))+packetHeader->MapPosition) % PAGE_SIZE)));

                packetHeader->OutputPosition = OutputPosition;

                ULONG mapRegisterCount = ( BytesThisMapping ?
                                           ADDRESS_AND_SIZE_TO_SPAN_PAGES( PUCHAR(MmGetMdlVirtualAddress( packetHeader->MdlAddress )) +
                                                                           packetHeader->MapPosition,
                                                                           BytesThisMapping ) :
                                           0 );

                ASSERT( MaxMapRegisters >= mapRegisterCount );

                if(mapRegisterCount != 0)   //  Only 0 if no mapping queue entries available or empty packet.
                {
                    Dma.PacketHeader     = packetHeader;
                    Dma.Irp              = irp;
                    Dma.State            = DMA_CONTEXT::CallbackPending;
                    Dma.BytesThisMapping = BytesThisMapping;
                    Dma.LastSubPacket    = (BytesThisMapping == BytesToMap);
                    Dma.Cancelled        = false;

                    // note - we're already at DISPATCH_LEVEL (we're holding spinlocks)

                    NTSTATUS ntStatus = IoAllocateAdapterChannel( BusMasterAdapterObject,
                                                                  FunctionalDeviceObject,
                                                                  mapRegisterCount,
                                                                  CallbackFromIoAllocateAdapterChannel,
                                                                  this);
                    if(NT_SUCCESS(ntStatus))
                    {
                        //  We succeeded, so determine synchronicity of the
                        //  callback.  If it's happened, get the newly queued
                        //  mapping, because that's what we're here for.
                        //  The new state is either "idle" or "deferred callback"
                        if (Dma.State == DMA_CONTEXT::CallbackPending)
                        {
                            Dma.State = DMA_CONTEXT::DeferredCallback;
                            //  Release the IRP so we can acquire it in the 
                            //  callback
                            ReleaseMappingIrp(irp, NULL);
                            KeClearEvent(&Dma.HangingCallbackCompleted);
                        }
                        else
                        {
                            ASSERT(Dma.State == DMA_CONTEXT::SynchronousCallback);
                            //  No callbacks pending- next user can request a channel
                            Adapter.RequestComplete(this);
                            entry = GetQueuedMapping();
                            Dma.State = DMA_CONTEXT::Idle;  // It is now safe for next op
                        }
                    } else
                    {
                        //  Shouldn't really happen, but if it does, idle 
                        //  everything
                        Dma.State = DMA_CONTEXT::Idle;
                        Adapter.RequestComplete(this);
                        ReleaseMappingIrp( irp, NULL );

                        DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                                             PCTRACE_IRPSTRM,
                                             "[IrpStrm%p::GetMapping] IoAllocateAdapterChannel FAILED 0x%x",
                                             this,
                                             ntStatus );
                    }
                } else 
                {
                    //  Seems unlikely, but we had nothing to map, hence, we're done!
                    Adapter.RequestComplete(this);
                    ReleaseMappingIrp( irp,NULL );
                }
            }
        }
    }

    if(entry)
    {
        // it had better be mapped...
        ASSERT( entry->MappingStatus == MAPPING_STATUS_MAPPED );
        entry->Tag            = Tag;

        entry->MappingStatus  = MAPPING_STATUS_DELIVERED;

        *PhysicalAddress      = entry->PhysicalAddress;
        *VirtualAddress       = entry->VirtualAddress;
        *ByteCount            = entry->ByteCount;
        *Flags                = (entry->Flags & (MAPPING_FLAG_END_OF_PACKET | MAPPING_FLAG_END_OF_SUBPACKET)) ?
                                MAPPING_FLAG_END_OF_PACKET : 0;

        m_irpStreamPosition.ullMappingPosition += entry->ByteCount;
        m_irpStreamPosition.ulMappingOffset += entry->ByteCount;

        MappingsOutstanding++;

        PcWmiLogEvent( TRACE_LEVEL_VERBOSE,
                       PC_ETYPE_IRP_MAPPING_ACQUIRED,
                       PortCls_IrpProcessing_GUID,
                       (ULONGLONG )this,
                       (ULONGLONG )entry->Irp,
                       entry->ByteCount,
                       MappingsOutstanding,
                       (ULONGLONG)Tag );

        DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                             PCTRACE_IRPSTRM,
                             "[IrpStrm%p::GetMapping] Tag=%p VA=%p BC=0x%x FL=0x%x",
                             this,
                             Tag,
                             *VirtualAddress,
                             *ByteCount,
                             *Flags );
    }
    else
    {
        WasExhausted = TRUE;
        *ByteCount = 0;

        DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                             PCTRACE_IRPSTRM,
                             "[IrpStrm%p::GetMapping] Exhausted",
                             this );
    }

    m_RevokeLock.Release();
}

/*****************************************************************************
 * CIrpStream::ReleaseMapping()
 *****************************************************************************
 * Releases a mapping obtained through GetMapping().
 */
STDMETHODIMP_(void)
CIrpStream::
ReleaseMapping
(
    IN      PVOID   Tag
)
{
    //Acquire the revoke spinlock
    m_RevokeLock.Acquire();

    PMAPPING_QUEUE_ENTRY entry = DequeueMapping();

    while( (NULL != entry) && (entry->MappingStatus != MAPPING_STATUS_DELIVERED) )
    {
        entry->MappingStatus = MAPPING_STATUS_EMPTY;
        entry->Tag = PVOID(-1);

        entry = DequeueMapping();
    }

    // check if we found and entry
    if( !entry )
    {
        m_RevokeLock.Release();

        DoTraceLevelMessage( TRACE_LEVEL_WARNING,
                             PCTRACE_IRPSTRM,
                             "[IrpStrm%p::ReleaseMapping] Tag=%p Failed to find mapping",
                             this,
                             Tag );
        return;
    }

    //
    // Due to race conditions between portcls and the WDM driver, the driver
    // might first release the second mapping and then the first mapping in
    // the row.
    // By design, it doesn't make sense for a audio driver to play "in the
    // middle" of a stream and release mappings there. The only exception
    // where mappings might not be released in order how the driver got them
    // is mentioned above.
    // Since we know that, we don't need to search for the right mapping!
    //

    DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                         PCTRACE_IRPSTRM,
                         "[IrpStrm%p::ReleaseMapping] Tag=%p%s",
                         this,
                         Tag,
                         Tag != entry->Tag ? " MISMATCH":"");

    
    // mark the entry as empty
    entry->MappingStatus = MAPPING_STATUS_EMPTY;

    MappingsOutstanding--;

    PcWmiLogEvent( TRACE_LEVEL_INFORMATION,
                   PC_ETYPE_IRP_MAPPING_RELEASED,
                   PortCls_IrpProcessing_GUID,
                   (ULONGLONG )this,
                   (ULONGLONG )entry->Irp,
                   entry->ByteCount,
                   MappingsOutstanding,
                   (ULONGLONG)entry->Tag );

    entry->Tag = PVOID(-1);

    // get the unmapping irp
    PIRP irp = DbgAcquireUnmappingIrp("ReleaseMapping");

    if( irp )
    {
        PPACKET_HEADER packetHeader = IRP_CONTEXT_IRP_STORAGE(irp)->UnmappingPacket;

        // update position info
        packetHeader->UnmapPosition += entry->ByteCount;
            m_irpStreamPosition.ulUnmappingPacketSize = packetHeader->BytesTotal;
        m_irpStreamPosition.ulUnmappingOffset = packetHeader->UnmapPosition;
        m_irpStreamPosition.ullUnmappingPosition += entry->ByteCount;
            m_irpStreamPosition.bUnmappingPacketLooped = ( ( packetHeader->StreamHeader->OptionsFlags &
                                                             KSSTREAM_HEADER_OPTIONSF_LOOPEDDATA ) != 0 );

        // check if this is the last mapping in the packet or subpacket
        if( ( entry->Flags & MAPPING_FLAG_END_OF_PACKET ) ||
            ( entry->Flags & MAPPING_FLAG_END_OF_SUBPACKET) )
        {
            // Flush the DMA adapter buffers.
            IoFlushAdapterBuffers( BusMasterAdapterObject,
                                   packetHeader->MdlAddress,
                                   entry->MapRegisterBase,
                                   entry->SubpacketVa,
                                   entry->SubpacketBytes,
                                   WriteOperation );
    
            IoFreeMapRegisters( BusMasterAdapterObject,
                                entry->MapRegisterBase,
                                ADDRESS_AND_SIZE_TO_SPAN_PAGES(entry->SubpacketVa,entry->SubpacketBytes) );
        }

        // release the unmapping irp and only pass the packet header if the packet is completed
        ReleaseUnmappingIrp(irp, (entry->Flags & MAPPING_FLAG_END_OF_PACKET) ? packetHeader : NULL);
    }

    m_RevokeLock.Release();
}

/*****************************************************************************
 * CallbackFromIoAllocateAdapterChannel()
 *****************************************************************************
 * Callback from IoAllocateAdapterChannel to create scatter/gather entries.
 */
static
IO_ALLOCATION_ACTION
CallbackFromIoAllocateAdapterChannel
(
    IN      PDEVICE_OBJECT  DeviceObject,
    IN      PIRP            Reserved,
    IN      PVOID           MapRegisterBase,
    IN      PVOID           VoidContext
)
{
    ASSERT(DeviceObject);
    ASSERT(VoidContext);

    //  Begin by getting access to the stream, and from that, the Revoke lock,
    //  and the DMA queue for the adapter.  

    CIrpStream&  Stream = *(CIrpStream*)VoidContext;
    //  Revoke lock is reacquirable on same processor, hence safe to grab here.
    //  This also says callback cannot complete during original call if it was deferred
    //  (because the original processor still owns the revoke lock)
    Stream.m_RevokeLock.Acquire();

    SynchronizedDMAQueue& Adapter = 
        PDEVICE_CONTEXT(Stream.FunctionalDeviceObject->DeviceExtension)->pExtension->Queue;

    ASSERT(Adapter.IsAvailable(&Stream));

    // If we are a deferred callback, then we must acquire the mapping Irp
    if (Stream.Dma.State != DMA_CONTEXT::CallbackPending)
    {
        ASSERT(Stream.Dma.State == DMA_CONTEXT::DeferredCallback);
        //  If an IRP awaiting callback is cancelled, we marked it but did not
        //  complete it so we could avoid having bad Irp pointers, etc.
        //  This marking always occurs under revoke lock, which we now hold.
        if (Stream.Dma.Cancelled)
        {
            //  Cancelled => Complete the IRP and reset the request.
            DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                                 PCTRACE_IRPSTRM,
                                 "[CallbackFromIoAllocateAdapterChannel] Irp Completed %p",
                                 Stream.Dma.Irp );

            PcWmiLogEvent( TRACE_LEVEL_INFORMATION,
                           Stream.Dma.Irp->IoStatus.Status == STATUS_CANCELLED ? PC_ETYPE_IRP_CANCELLED : PC_ETYPE_IRP_COMPLETED,
                           PortCls_IrpProcessing_GUID,
                           (ULONGLONG )Stream.Dma.Irp,
                           (ULONGLONG )1,
                           Stream.Dma.Irp->IoStatus.Status,
                           0, 
                           NULL );

            IoCompleteRequest(Stream.Dma.Irp, IO_NO_INCREMENT);
            Stream.Dma.Irp = NULL;
            Stream.Dma.State = DMA_CONTEXT::Idle;
            Adapter.RequestComplete(&Stream);
            Stream.Dma.Cancelled = false;
            Stream.m_RevokeLock.Release();
            KeSetEvent(&Stream.Dma.HangingCallbackCompleted, IO_NO_INCREMENT, 
                FALSE);
            return DeallocateObject;    //  Don't keep registers
        }
        //  Acquire the mapping irp now (didn't need to for that last bit...)
#if DBG
        PIRP irp = 
#endif
        Stream.DbgAcquireMappingIrp("GetMapping",TRUE);
#if DBG
        ASSERT(irp == Stream.Dma.Irp);
#endif
    }

    PIRP Irp = Stream.Dma.Irp;

    PUCHAR virtualAddress = PUCHAR(MmGetMdlVirtualAddress(Stream.Dma.PacketHeader->MdlAddress));

    //  Assertion- we know we will always get system addresses because someone 
    //  has done a successful probe with KsProbeIrp (including a system 
    //  address), long before we got here.

#ifdef UNDER_NT
    PUCHAR entryVA = PUCHAR(MmGetSystemAddressForMdlSafe(Stream.Dma.PacketHeader->MdlAddress,HighPagePriority | MdlMappingNoExecute));
#else
    PUCHAR entryVA = PUCHAR(MmGetSystemAddressForMdl(context->PacketHeader->MdlAddress));
#endif

    ASSERT(entryVA);

    ULONG bytesRemaining = Stream.Dma.BytesThisMapping;

    ULONG flags = Stream.Dma.LastSubPacket ? MAPPING_FLAG_END_OF_PACKET : MAPPING_FLAG_END_OF_SUBPACKET;

    //
    // Consider mapping offset in case we have set position.
    //
    virtualAddress  += Stream.Dma.PacketHeader->MapPosition;
    entryVA         += Stream.Dma.PacketHeader->MapPosition;

    PVOID subpacketVa = virtualAddress;

    while(bytesRemaining)
    {
        ULONG segmentLength = bytesRemaining;

        // Create one mapping.
        PHYSICAL_ADDRESS physicalAddress = IoMapTransfer(Stream.BusMasterAdapterObject,
                                                          Stream.Dma.PacketHeader->MdlAddress,
                                                          MapRegisterBase,
                                                          virtualAddress,
                                                          &segmentLength,
                                                          Stream.WriteOperation );

        bytesRemaining -= segmentLength;
        virtualAddress += segmentLength;

        // enqueue the mapping
        
        NTSTATUS ntStatus;
        
        // We do not attempt to break up large mappings based on hardware constraints
        // If that is ever to be done, the work would be done here.  No need has surfaced.
        
        ntStatus = Stream.EnqueueMapping(physicalAddress,
                                         Irp,
                                         Stream.Dma.PacketHeader,
                                         PVOID(entryVA),
                                         segmentLength,
                                         ((bytesRemaining == 0) ? flags : 0),
                                         MapRegisterBase,
                                         MAPPING_STATUS_MAPPED,
                                         ((bytesRemaining == 0) ? subpacketVa : NULL),
                                         ((bytesRemaining == 0) ? Stream.Dma.BytesThisMapping : 0 ) );
        if( NT_SUCCESS(ntStatus) )
        {
            entryVA += segmentLength;
            physicalAddress.LowPart += segmentLength;
        }
        else
        {
            //  We need to adjust the amount actually mapped here, and then bail again
            //  This amount is used to adjust the overall buffer tracking when these loops 
            //  exit.
            //  This really shouldn't happen, because we already verified we had enough 
            //  space in the mapping queue before we called for DMA mapping, and that's the
            //  only reason for failing EnqueueMapping.  But why take unneccessary chances?
            Stream.Dma.BytesThisMapping-= bytesRemaining + segmentLength;
            break;
        }
    }

    Stream.Dma.PacketHeader->MapPosition += Stream.Dma.BytesThisMapping;
    Stream.OutputPosition += Stream.Dma.BytesThisMapping;

    Stream.ReleaseMappingIrp(Stream.Dma.Irp,
        ((Stream.Dma.PacketHeader->MapPosition == Stream.Dma.PacketHeader->BytesTotal) ? Stream.Dma.PacketHeader : NULL));

    // If we entered with callback pending, we are synchronous callback, so 
    // advance the state.
    // Otherwise, we are deferred, and we must queue a DPC to restart any 
    // stalled pump.
    // In that case, we must also set the state to Idle, as we've completed the 
    // front end of the operation.
    if  (Stream.Dma.State == DMA_CONTEXT::CallbackPending)
    {
        Stream.Dma.State = DMA_CONTEXT::SynchronousCallback;
    }
    else
    {
        ASSERT(Stream.Dma.State == DMA_CONTEXT::DeferredCallback);
        Stream.Dma.ReportNewMappings(true);
        Stream.Dma.State = DMA_CONTEXT::Idle;
    }
    
    Stream.m_RevokeLock.Release();

    return DeallocateObjectKeepRegisters;
}

/*****************************************************************************
 * CIrpStream::AcquireMappingIrp()
 *****************************************************************************
 * Acquire the IRP in which mapping is currently occuring.
 */
PIRP
CIrpStream::
AcquireMappingIrp
(
#if DBG
    IN      PCSTR   Owner,
#endif
    IN      BOOLEAN NotifyExhausted
)
{
    m_kSpinLock.Acquire();
    
    PIRP irp = KsRemoveIrpFromCancelableQueue( &LockedQueue,
                                               &LockedQueueLock,
                                               KsListEntryHead,
                                               KsAcquireOnlySingleItem );

    if(! irp)
    {
        m_kSpinLock.Release();
    }

#if DBG
    if(irp)
    {
        MappingIrpOwner = Owner;
    }
    DbgQueues();
#endif

    return irp;
}

/*****************************************************************************
 * CIrpStream::AcquireUnmappingIrp()
 *****************************************************************************
 * Acquire the IRP in which unmapping is currently occuring.
 */
PIRP
CIrpStream::
AcquireUnmappingIrp
(
#if DBG
    IN      PCSTR   Owner
#endif
)
{
    m_kSpinLock.Acquire();

    // The IRP that we should be unmapping is at the head of the mapped queue
    // if it is completely mapped.  Otherwise it's at the head of the locked
    // queue, and the mapped queue is empty.

    // Acquire the head IRP in the locked queue just in case.
    PIRP lockedIrp = KsRemoveIrpFromCancelableQueue( &LockedQueue,
                                                     &LockedQueueLock,
                                                     KsListEntryHead,
                                                     KsAcquireOnlySingleItem );

    // Acquire the head IRP in the mapped queue.
    PIRP irp = KsRemoveIrpFromCancelableQueue( &MappedQueue,
                                               &MappedQueueLock,
                                               KsListEntryHead,
                                               KsAcquireOnlySingleItem );

    if(irp)
    {
        // Don't need the IRP from the locked queue.
        if(lockedIrp)
        {
            KsReleaseIrpOnCancelableQueue( lockedIrp,
                                           IrpStreamCancelRoutine );
        }
    }
    else
        if(IsListEmpty(&MappedQueue))
    {
        // Mapped queue is empty, try locked queue.
        if(lockedIrp)
        {
            irp = lockedIrp;
        }
    }
    else
    {
        // There's a busy IRP in the mapped queue.
        if(lockedIrp)
        {
            KsReleaseIrpOnCancelableQueue( lockedIrp,
                                           IrpStreamCancelRoutine );
        }
    }

    if(! irp)
    {
        m_kSpinLock.Release();
    }

#if DBG
    if(irp)
    {
        UnmappingIrpOwner = Owner;
    }
    DbgQueues();
#endif

    return irp;
}

/*****************************************************************************
 * CIrpStream::ReleaseMappingIrp()
 *****************************************************************************
 * Releases the mapping IRP previously acquired through AcqureMappingIrp(),
 * possibly handling the completion of a packet.
 */
void
CIrpStream::
ReleaseMappingIrp
(
    IN      PIRP            pIrp,
    IN      PPACKET_HEADER  pPacketHeader   OPTIONAL
)
{
    ASSERT(pIrp);

    if(pPacketHeader)
    {
        if(pPacketHeader->IncrementMapping)
        {
            pPacketHeader->IncrementMapping = FALSE;
            pPacketHeader++;
        }
        else
        {
            PPACKET_HEADER prevPacketHeader = pPacketHeader;

            pPacketHeader = pPacketHeader->Next;

            //
            // If looping back, stop if there is another IRP.
            //
            if( pPacketHeader &&
                (pPacketHeader <= prevPacketHeader) &&
                (FLINK_IRP_STORAGE(pIrp) != &LockedQueue) )
            {
                pPacketHeader = NULL;
            }
        }

        if(pPacketHeader)
        {
            // Use next packet header next time.
            IRP_CONTEXT_IRP_STORAGE(pIrp)->MappingPacket = pPacketHeader;

            pPacketHeader->MapCount++;
            pPacketHeader->MapPosition = 0;
            m_irpStreamPosition.ulMappingOffset = 0;
            m_irpStreamPosition.ulMappingPacketSize = pPacketHeader->BytesTotal;
            m_irpStreamPosition.bMappingPacketLooped = ( ( pPacketHeader->StreamHeader->OptionsFlags &
                                                           KSSTREAM_HEADER_OPTIONSF_LOOPEDDATA ) != 0 );

            KsReleaseIrpOnCancelableQueue( pIrp,
                                           IrpStreamCancelRoutine );
        }
        else if( m_irpStreamPosition.bLoopedInterface && (FLINK_IRP_STORAGE(pIrp) == &LockedQueue) )
        {
            //
            // Completed one-shot with looped interface and there are no more
            // packets.
            //
            KsReleaseIrpOnCancelableQueue( pIrp,
                                           IrpStreamCancelRoutine );
        }
        else
        {
            //
            // IRP is completely mapped.
            //

            //
            // See if we need to initiate unmapping.
            //
            BOOL bKickUnmapping = FALSE;

            if(IsListEmpty(&MappedQueue))
            {
                pPacketHeader = IRP_CONTEXT_IRP_STORAGE(pIrp)->UnmappingPacket;

                bKickUnmapping = ( pPacketHeader->UnmapPosition ==  pPacketHeader->BytesTotal );
            }

            //
            // Add the IRP to the mapped queued.
            //
            KsRemoveSpecificIrpFromCancelableQueue(pIrp);
            KsAddIrpToCancelableQueue( &MappedQueue,
                                       &MappedQueueLock,
                                       pIrp,
                                       KsListEntryTail,
                                       IrpStreamCancelRoutine );

            if(bKickUnmapping)
            {
                //
                // Unmap the completed header.
                //
#if DBG
                PIRP pIrpRemoved = 
#endif
                KsRemoveIrpFromCancelableQueue(&MappedQueue,
                                               &MappedQueueLock,
                                               KsListEntryHead,
                                               KsAcquireOnlySingleItem );
#if DBG
                ASSERT(pIrpRemoved == pIrp);
#endif
                ReleaseUnmappingIrp( pIrp, IRP_CONTEXT_IRP_STORAGE(pIrp)->UnmappingPacket );

                return; // ReleaseUnmappingIrp() releases the spinlock.
            }
        }
    }
    else
    {
        KsReleaseIrpOnCancelableQueue( pIrp,
                                       IrpStreamCancelRoutine );
    }

    m_kSpinLock.Release();
}

/*****************************************************************************
 * CIrpStream::ReleaseUnmappingIrp()
 *****************************************************************************
 * Releases the unmapping IRP acquired through AcquireUnmappingIrp(),
 * possibly handling the completion of a packet.
 */
void
CIrpStream::
ReleaseUnmappingIrp
(
    IN      PIRP            pIrp,
    IN      PPACKET_HEADER  pPacketHeader   OPTIONAL
)
{
    ASSERT(pIrp);

    //
    // Loop until there are no more packets completely unmapped.
    //
    while(1)
    {
        //
        // If we don't have a newly unmapped packet, just release.
        //
        if(! pPacketHeader)
        {
            KsReleaseIrpOnCancelableQueue( pIrp,
                                           IrpStreamCancelRoutine );
            break;
        }

        //
        // Loop 'til we find the next packet in the IRP if there is one.
        //
        while(1)
        {
            //
            // Copy back total byte count into data used for capture.
            // It's a no-op for render.
            //
            pPacketHeader->StreamHeader->DataUsed = pPacketHeader->BytesTotal;

            pPacketHeader->MapCount--;

            if(pPacketHeader->IncrementUnmapping)
            {
                pPacketHeader->IncrementUnmapping = FALSE;
                pPacketHeader++;
            }
            else
            {
                pPacketHeader = pPacketHeader->Next;
                if(! pPacketHeader)
                {
                    break;
                }
                else
                    if(pPacketHeader->MapCount == 0)
                {
                    pPacketHeader = NULL;
                    break;
                }
            }

            //
            // Loop only if this is a zero-length packet.
            //
            if(pPacketHeader->BytesTotal)
            {
                break;
            }
        }

        if(pPacketHeader)
        {
            //
            // Use next packet header next time.
            //
            IRP_CONTEXT_IRP_STORAGE(pIrp)->UnmappingPacket = pPacketHeader;

            pPacketHeader->UnmapPosition = 0;
            pPacketHeader = NULL;
        }
        else
        {
            //
            // Remove the IRP from the queue.
            //
            KsRemoveSpecificIrpFromCancelableQueue(pIrp);

            //
            // Done with IRP...free the context memory we allocated
            //
            if( IRP_CONTEXT_IRP_STORAGE(pIrp) )
            {
                ExFreePool( IRP_CONTEXT_IRP_STORAGE(pIrp) );
                IRP_CONTEXT_IRP_STORAGE(pIrp) = NULL;
            } else
            {
                ASSERT( !"Freeing IRP with no context");
            }

            //
            // Indicate in the IRP how much data we have captured.
            //
            if(! WriteOperation)
            {
                pIrp->IoStatus.Information = IoGetCurrentIrpStackLocation(pIrp)->
                                             Parameters.DeviceIoControl.OutputBufferLength;
            }

            //
            // Mark it happy.
            //
            pIrp->IoStatus.Status = STATUS_SUCCESS;

            DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                                 PCTRACE_IRPSTRM,
                                 "[IrpStrm%p::ReleaseUnmappingIrp] Irp finished %p",
                                 this,
                                 pIrp );

            //
            // Pass it to the next transport sink.
            //
            ASSERT(m_TransportSink);
            KsShellTransferKsIrp(m_TransportSink,pIrp);

            //
            // Acquire the head IRP in the mapped queue.
            //
            pIrp = KsRemoveIrpFromCancelableQueue( &MappedQueue,
                                                   &MappedQueueLock,
                                                   KsListEntryHead,
                                                   KsAcquireOnlySingleItem );

            //
            // No IRP.  Outta here.
            //
            if(! pIrp)
            {
                break;
            }

            //
            // See if we need to complete this packet.
            //
#pragma prefast(suppress: __WARNING_DEREF_NULL_PTR, "If it's on the queue and has no context, we WANT a bugcheck")
            pPacketHeader = IRP_CONTEXT_IRP_STORAGE(pIrp)->UnmappingPacket;

            if(pPacketHeader->UnmapPosition != pPacketHeader->BytesTotal)
            {
                pPacketHeader = NULL;
            }
        }
    }

    m_kSpinLock.Release();
}

/*****************************************************************************
 * CIrpStream::EnqueueMapping()
 *****************************************************************************
 * Add a mapping to the mapping queue.
 */
NTSTATUS
CIrpStream::
EnqueueMapping
(
    IN      PHYSICAL_ADDRESS    PhysicalAddress,
    IN      PIRP                Irp,
    IN      PPACKET_HEADER      PacketHeader,
    IN      PVOID               VirtualAddress,
    IN      ULONG               ByteCount,
    IN      ULONG               Flags,
    IN      PVOID               MapRegisterBase,
    IN      ULONG               MappingStatus,
    IN      PVOID               SubpacketVa,
    IN      ULONG               SubpacketBytes
)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    //  This logic treats the state where there is only one entry left, and you want to fill
    //  that final entry, as a "full queue".  So one item will always be unused.

    if( (MappingQueue.Tail + 1 == MappingQueue.Head) ||
        ( (MappingQueue.Tail + 1 == MAPPING_QUEUE_SIZE) &&
          (MappingQueue.Head == 0) ) )
    {
        // mapping queue looks full.  check to see if we can move the head to make
        // room.
        if( (MappingQueue.Array[MappingQueue.Head].MappingStatus != MAPPING_STATUS_MAPPED) &&
            (MappingQueue.Array[MappingQueue.Head].MappingStatus != MAPPING_STATUS_DELIVERED) )
        {
            PMAPPING_QUEUE_ENTRY entry = DequeueMapping();
            ASSERT(entry);
            if (entry)
            {
                entry->MappingStatus = MAPPING_STATUS_EMPTY;                        
            }
            else
            {
                ntStatus = STATUS_UNSUCCESSFUL;
            }
        }
        else
        {
            DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                                 PCTRACE_IRPSTRM,
                                 "[IrpStrm%p::EnqueueMapping] Mapping Queue FULL!",
                                 this );
            ntStatus = STATUS_UNSUCCESSFUL;
        }
    }

    if (NT_SUCCESS(ntStatus))
    {
        MappingQueue.Array[MappingQueue.Tail].PhysicalAddress  = PhysicalAddress;
        MappingQueue.Array[MappingQueue.Tail].Irp              = Irp;
        MappingQueue.Array[MappingQueue.Tail].PacketHeader     = PacketHeader;
        MappingQueue.Array[MappingQueue.Tail].VirtualAddress   = VirtualAddress;
        MappingQueue.Array[MappingQueue.Tail].ByteCount        = ByteCount;
        MappingQueue.Array[MappingQueue.Tail].Flags            = Flags;
        MappingQueue.Array[MappingQueue.Tail].MapRegisterBase  = MapRegisterBase;
        MappingQueue.Array[MappingQueue.Tail].MappingStatus    = MappingStatus;
        MappingQueue.Array[MappingQueue.Tail].SubpacketVa      = SubpacketVa;
        MappingQueue.Array[MappingQueue.Tail].SubpacketBytes   = SubpacketBytes;

#if (DBG)
        MappingsQueued++;
#endif

        DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                             PCTRACE_IRPSTRM,
                             "[IrpStrm%p::EnqueueMapping] Entry=%p Irp=%p VA=%p BC=0x%x FL=0x%x",
                             this,
                             &MappingQueue.Array[MappingQueue.Tail],
                             Irp,
                             VirtualAddress,
                             ByteCount,
                             Flags );

        if(++MappingQueue.Tail == MAPPING_QUEUE_SIZE)
        {
            MappingQueue.Tail = 0;
        }
    }
    return ntStatus;
}

/*****************************************************************************
 * CIrpStream::AvailableMappingQueueEntries()
 *****************************************************************************
 * Tells us how many mapping queue entries are available
 */
unsigned
CIrpStream::
AvailableMappingQueueEntries
(
)
{
    //  The logic used in managing this queue always has one entry not in use
    return (MAPPING_QUEUE_SIZE -1) - (MappingQueue.Tail >= MappingQueue.Head ?
        MappingQueue.Tail - MappingQueue.Head :
        MappingQueue.Tail + (MAPPING_QUEUE_SIZE - MappingQueue.Head));
}

/*****************************************************************************
 * CIrpStream::GetQueuedMapping()
 *****************************************************************************
 * Get a queued mapping from the mapping queue.
 */
PMAPPING_QUEUE_ENTRY
CIrpStream::
GetQueuedMapping
(   void
)
{
    PMAPPING_QUEUE_ENTRY result;

    if(MappingQueue.Get == MappingQueue.Tail)
    {
        result = NULL;
    }
    else
    {
        result = &MappingQueue.Array[MappingQueue.Get];

        if(++MappingQueue.Get == MAPPING_QUEUE_SIZE)
        {
            MappingQueue.Get = 0;
        }
    }

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_IRPSTRM,
                         "[IrpStrm%p::GetQueuedMapping] Entry=%p",
                         this,
                         result );

    return result;
}

/*****************************************************************************
 * CIrpStream::DequeueMapping()
 *****************************************************************************
 * Remove a mapping from the mapping queue.
 */
PMAPPING_QUEUE_ENTRY
CIrpStream::
DequeueMapping
(   void
)
{
    PMAPPING_QUEUE_ENTRY result;

    if(MappingQueue.Head == MappingQueue.Tail)
    {
        result = NULL;
    }
    else
    {
        result = &MappingQueue.Array[MappingQueue.Head];

#if (DBG)
        MappingsQueued--;
#endif

        if(++MappingQueue.Head == MAPPING_QUEUE_SIZE)
        {
            MappingQueue.Head = 0;
        }
    }

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_IRPSTRM,
                         "[IrpStrm%p::DequeueMapping] Entry=%p",
                         this,
                         result );

    return result;
}

/*****************************************************************************
 * IrpStreamCancelRoutine()
 *****************************************************************************
 * Do cancellation.Always called at DISPATCH_LEVEL.
 */
#pragma prefast(disable: 28144)
VOID
IrpStreamCancelRoutine
(
    IN      PDEVICE_OBJECT   DeviceObject,
    IN      PIRP             Irp
)
{
    ASSERT(DeviceObject);
    ASSERT(Irp);

    DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                         PCTRACE_IRPSTRM,
                         "[IrpStreamCancelRoutine] Irp=%p",
                         Irp );

    //
    // Mark the IRP cancelled and call the standard routine.  Doing the
    // marking first has the effect of not completing the IRP in the standard
    // routine.  The standard routine removes the IRP from the queue and
    // releases the cancel spin lock.
    //
    Irp->IoStatus.Status = STATUS_CANCELLED;
    KsCancelRoutine(DeviceObject,Irp);

    //  We want to return to the Irql at which the IRP was cencelled before we
    //  go, but the call to IoReleaseCancelSpinlock has to come while we hold
    //  other locks.  I don't want Irql dropping below dispatch level there, or
    //  the mechnism for reacquring spin locks WILL break.

    //  So save the cancel irql, set it to DISPATCH_LEVEL, and then call 
    //  KeLowerIrql when all is said and done.

    //  This won't break the CancellAllIrps thread, because the CancelIrql for
    //  these irps will already be DISPATCH_LEVEL (because the locks are held).

    KIRQL   IrpCancelIrql = Irp->CancelIrql;
    Irp->CancelIrql = DISPATCH_LEVEL;

    bool    CompleteTheIrp = true;
	CIrpStream *that = NULL;

    if (IRP_CONTEXT_IRP_STORAGE(Irp))
    {
        // get the IrpStream context
        that = (CIrpStream *)(PIRP_CONTEXT(IRP_CONTEXT_IRP_STORAGE(Irp))->IrpStream);
        
        // must always grab revoke lock BEFORE master lock
   	    that->m_RevokeLock.Acquire();
       	that->m_kSpinLock.Acquire();

        // Search the mapping queue for mappings to revoke.
        // Free associated map registers.

        that->CancelMappings(Irp);

        // We can complete the Irp unless it's pending a callback
        // Also, want to make all DMA state checks while the revoke lock is 
        // held.

        CompleteTheIrp = 
            that->Dma.State != DMA_CONTEXT::DeferredCallback ||
            that->Dma.Irp != Irp;

        if  (!CompleteTheIrp)
            that->Dma.Cancelled = true;
        //  If this request is queued waiting on DMA adapter, release it.
        else if (that->Dma.Irp == Irp && that->Dma.Queued)
        {
            ((PDEVICE_CONTEXT)(that->FunctionalDeviceObject->DeviceExtension))->
                pExtension->Queue.RequestComplete(that);
            that->Dma.Queued = false;
        }

  	    // release the spinlocks, master first
        that->m_kSpinLock.Release();
   	    that->m_RevokeLock.Release();

        // Free the context memory we allocated
        ExFreePool(IRP_CONTEXT_IRP_STORAGE(Irp));
        IRP_CONTEXT_IRP_STORAGE(Irp) = NULL;
    }
    else
    {
        ASSERT( !"Freeing IRP with no context");
    }

    //  NOW, restore the cancel irql from the irp
    KeLowerIrql(IrpCancelIrql);
    Irp->CancelIrql = KeGetCurrentIrql();

    if  (CompleteTheIrp)
    {
        PcWmiLogEvent( TRACE_LEVEL_VERBOSE,
                       Irp->IoStatus.Status == STATUS_CANCELLED ? PC_ETYPE_IRP_CANCELLED : PC_ETYPE_IRP_COMPLETED,
                       PortCls_IrpProcessing_GUID,
                       (ULONGLONG )Irp,
                       (ULONGLONG )2,
                       Irp->IoStatus.Status,
                       0,
                       NULL );

        IoCompleteRequest(Irp,IO_NO_INCREMENT);
    }
}
#pragma prefast(enable: 28144)

/*****************************************************************************
 * CIrpStream::CancelMappings()
 *****************************************************************************
 * Cancel mappings for an IRP or all IRPs.
 */
void
CIrpStream::
CancelMappings
(
    IN      PIRP    pIrp
)
{
    // NOTE: the revoke and master spinlocks must be held before calling this routine

    DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                         PCTRACE_IRPSTRM,
                         "[IrpStrm%p::CancelMappings] Irp=%p",
                         this,
                         pIrp );

    // check only if we have a non-empty mapping queue
    if( (MappingQueue.Array) &&
        (MappingQueue.Head != MappingQueue.Tail) )
    {
        ULONG   ulPosition      = MappingQueue.Head;
        ULONG   ulFirst         = ULONG(-1);
        ULONG   ulLast          = ULONG(-1);
        ULONG   ulMappingCount  = 0;

        // walk mapping queue from head to tail
        while( ulPosition != MappingQueue.Tail )
        {
            // get the mapping queue entry
            PMAPPING_QUEUE_ENTRY entry = &MappingQueue.Array[ulPosition];

            // check if this mapping belongs to the irp(s) being cancelled
            if( (NULL == pIrp) || (entry->Irp == pIrp) )
            {
                // check if the mapping has been delivered
                if( entry->MappingStatus == MAPPING_STATUS_DELIVERED )
                {
                    // keep track of this for the driver revoke call
                    if( ulFirst == ULONG(-1) )
                    {
                        ulFirst = ulPosition;
                    }

                    ulLast = ulPosition;
                    ulMappingCount++;
                }

                // is this the last mapping in a packet (and not previously revoked)?
                if( ( ( entry->Flags & MAPPING_FLAG_END_OF_PACKET ) ||
                      ( entry->Flags & MAPPING_FLAG_END_OF_SUBPACKET) ) &&
                    ( entry->MappingStatus != MAPPING_STATUS_REVOKED ) )
                {
                    // do we need to revoke anything in the driver?
                    if( ulMappingCount )
                    {
                        ULONG   ulRevoked = ulMappingCount; // init to how many we are asking for

                        // revoke mappings in the driver
                        if( NotifyPhysical )
                        {
                            NotifyPhysical->MappingsCancelled( MappingQueue.Array[ulFirst].Tag,
                                                               MappingQueue.Array[ulLast].Tag,
                                                               &ulRevoked );

                            MappingsOutstanding -= ulRevoked;
                        }

                        // check if all were revoked
                        if( ulRevoked != ulMappingCount )
                        {
                            DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                                                 PCTRACE_IRPSTRM,
                                                 "[IrpStrm%p::CancelMappings] %d of %d mappings revoked",
                                                 this,
                                                 ulRevoked,
                                                 ulMappingCount );

                            // What does it mean if the driver can't revoke something?
                            ASSERT("It's assumed that the driver will cancell all mappings is requested!");
                        }

                        // reset the revoke tracking
                        ulFirst = ULONG(-1);
                        ulLast = ULONG(-1);
                        ulMappingCount = 0;
                    }

                    // get the packet header
                    PPACKET_HEADER header = entry->PacketHeader;

                    // release the mappings in this subpacket
                    if( ( header ) &&
                        ( entry->SubpacketVa ) &&
                        ( entry->SubpacketBytes ) )
                    {
                        // flush and free the mappings and map registers

                        IoFlushAdapterBuffers( BusMasterAdapterObject,
                                               header->MdlAddress,
                                               entry->MapRegisterBase,
                                               entry->SubpacketVa,
                                               entry->SubpacketBytes,
                                               WriteOperation );

                        IoFreeMapRegisters( BusMasterAdapterObject,
                                            entry->MapRegisterBase,
                                            ADDRESS_AND_SIZE_TO_SPAN_PAGES( entry->SubpacketVa,
                                                                            entry->SubpacketBytes ) );

                        if( entry->Flags & MAPPING_FLAG_END_OF_PACKET )
                        {
                            // decrement the map count if this is the end of a packet
                            header->MapCount--;
                        }
                    }
                }

                // mark the mapping as revoked
                entry->MappingStatus = MAPPING_STATUS_REVOKED;
            }

            // move on to the next entry
            if( ++ulPosition == MAPPING_QUEUE_SIZE )
            {
                ulPosition = 0;
            }
        }
    }
}

/*

  CSpinMutant methods

  These are not in-line because inlines can wind up in pageable segments if the
  compiler decides not to inline (eg checked builds).
     
  */

// prktCancel is non-NULL if we're called within the CancelAllIrps routine. 
void    CSpinMutant::Acquire()
{
    PKTHREAD pktCurrentThread = KeGetCurrentThread();

    //  If the thread does not own the lock, it must truly acquire it, and 
    //  mark itself as the owner.

    if  (Owner != pktCurrentThread)
    {
        KeAcquireSpinLock(&Lock, &OriginalIrql);
        ASSERT(0 == AcquireCount);  //  After acquisition, that is!
        Owner = pktCurrentThread;
    }
    ASSERT(DISPATCH_LEVEL == KeGetCurrentIrql());
    AcquireCount++;
}

void    CSpinMutant::Release()
{
    ASSERT(KeGetCurrentThread() == Owner);
    ASSERT(DISPATCH_LEVEL == KeGetCurrentIrql());
    ASSERT(0 < AcquireCount);

    if  (0 == --AcquireCount)
    {
        //  0 acquire count means it's time to release the lock for real.
        Owner = NULL;
        KeReleaseSpinLock(&Lock, OriginalIrql);
    }
}

/*

  DMA_CONTEXT methods

  */

// CustomDpc used to restart mapping pump when a deferred DMA callback
// occurs, or when this stream become the owner of a busy adapter.

void ReportMapAvailable(PKDPC, PVOID Context, PVOID, PVOID)
{
    CIrpStream& Stream = *(CIrpStream*) Context;

    if  (Stream.Dma.MustReleaseAdapter)
    {
        SynchronizedDMAQueue& Adapter = 
            PDEVICE_CONTEXT(Stream.FunctionalDeviceObject->DeviceExtension)->
                pExtension->Queue;
        Adapter.RequestComplete(&Stream);
    }

    if  (Stream.NotifyPhysical)
        Stream.NotifyPhysical->IrpSubmitted(NULL, true);
}

//  Initialization / deferred construction

void    DMA_CONTEXT::Init(CIrpStream* Owner)
{
    Stream = Owner;
    KeInitializeDpc(&MappingsAvailable, ReportMapAvailable, Stream);
    KeInitializeEvent(&HangingCallbackCompleted, NotificationEvent, FALSE);
    State = Idle;
    Cancelled = Queued = false;
    Irp = NULL;
    PacketHeader = NULL;
    LastSubPacket = FALSE;
    BytesThisMapping = 0;
}

//  Report new mappings available (or adapter available for mapping) via DPC

void    DMA_CONTEXT::ReportNewMappings(bool DeferredCallback)
{
    MustReleaseAdapter = DeferredCallback;
    KeInsertQueueDpc(&MappingsAvailable, NULL, NULL);
}

/*
    DMA_CONTEXT::WaitForAnyHangingCallbacks()

  This is only called from the CIrpStream destructor, and at passive level.

  If a callback is pending, we MUST wait for it to finish before we return
  from the destructor, since the callback assumes a vaild Irpstream context.

  Once the callback occurs, this is quick, as the IRP will have been cancelled.

  */

void    DMA_CONTEXT::WaitForAnyHangingCallbacks()
{
    //  At passive level, the only possible states are Idle or DeferredCallback
    if  (State == DeferredCallback)
        KeWaitForSingleObject(&HangingCallbackCompleted, Executive, KernelMode,
            FALSE, NULL);
}

/*

  Serializer

  This is a lightweight object that uses a spin lock to serialize access to
  structures at Dispatch level.  Serialization is ensured from point of 
  declaration to out-of-scope.  

  You can't use this for heavy work (such as multiple instances of this class,
  because destructor order isn't guaranteed), but it's OK to use it here.

  */

class Serializer
{
    PKSPIN_LOCK Lock;
public:
    Serializer(PKSPIN_LOCK TheLock)
    {
        KeAcquireSpinLockAtDpcLevel(Lock = TheLock);
    }
    ~Serializer() { KeReleaseSpinLockFromDpcLevel(Lock); }
};

/*


  SynchronizedDMAQueue

  The queue is a roughly FIFO list of IRP streams waiting to use the adapter.
  The mechanism is:
  (a) ask if adapter is available.  If it is, you are the first in line, and
  you may now begin/continue any DMA operations you have in mind.  If it is 
  not, you go into the queue at the end if you're not already there.
  (b) Report when you're done (this means with the mapping phase, not the 
  entire DMA op).  You get removed from the list.  If there's a waiter, a DPC
  is queued to tell it to resume mapping.

  The queue is maintianed using pointers contained in the CIrpStream objects.
  That causes this class to be a friend of CIrpStream, but it also provides
  maximum scalability and excellent performance.

  If you ask for availablity and you are the owner, you get a true return and 
  nothing else happens.  If you report being done, and you're not in there, 
  you're ignored.  If you report being done, and you are not the current owner 
  (this can happen when an IRP waiting on the queue is cancelled) you get 
  removed from the queue without any mapping callback.

  */

void*   DCExtension::operator new(size_t t) //  We now allocate an extension
{
    return ExAllocatePoolWithTag(NonPagedPoolNx, t, 'eDcP');
}

SynchronizedDMAQueue::SynchronizedDMAQueue()
{
    KeInitializeSpinLock(&Synchronizer);
    AdapterOwner = LastWaiter = NULL;
}

SynchronizedDMAQueue::~SynchronizedDMAQueue()
{
    ASSERT(AdapterOwner == NULL);
    ASSERT(LastWaiter == NULL);
}

/*

  bool  SynchronizedDmaQueue::IsAvailable(CIrpStream* Requester)

  If the queue is empty, the requester becomes the owner.  If the requester
  is not in the queue, then it is added.  The return indicates whether or not
  we are the Adapter owner.

  */

bool    SynchronizedDMAQueue::IsAvailable(CIrpStream* Requester)
{
    Serializer  Lock(&Synchronizer);    //  Serialize all queue operations

    if  (Requester == NULL)
    {
        return FALSE;
    }

    if  (AdapterOwner == NULL)
    {
        ASSERT(Requester->PreviousWaiter == NULL);
        ASSERT(Requester->NextWaiter == NULL);
        ASSERT(LastWaiter == NULL);
        AdapterOwner = LastWaiter = Requester;
    }

    //  If requester is not the owner and is not in the queue add it
    if  (AdapterOwner != Requester && !Requester->PreviousWaiter)
    {
        ASSERT(Requester->NextWaiter == NULL);
        Requester->PreviousWaiter = LastWaiter;
        LastWaiter->NextWaiter = Requester;
        LastWaiter = Requester;
    }

    ASSERT(Requester->PreviousWaiter == NULL || 
        Requester->PreviousWaiter->NextWaiter == Requester);

    ASSERT(Requester->NextWaiter == NULL || 
        Requester->NextWaiter->PreviousWaiter == Requester);

    //  The return indicates if we are the owner or not.

    return  Requester == AdapterOwner;
}

/*

  void  SynchronizedDMAQueue::RequestComplete(CIrpStream* Requester)

  Removes the stream from the queue (if it is there).  If the stream is the
  current owner of the Adapter then the next waiting stream becomes the queue 
  owner, and its "mapping available" entry is called to start mapping again.

  Otherwise, we just remove the stream from the queue.

  */

void    SynchronizedDMAQueue::RequestComplete(CIrpStream* Requester)
{
    Serializer  Lock(&Synchronizer);

    if  (Requester == NULL)
    {
        return;
    }

    if  (AdapterOwner == NULL)
    {
        ASSERT(LastWaiter == NULL);
        return;
    }

    //  Route linkages around this item

    if  (Requester->NextWaiter)
    {
        ASSERT(Requester->NextWaiter->PreviousWaiter == Requester);
        Requester->NextWaiter->PreviousWaiter = Requester->PreviousWaiter;
    }

    if  (Requester->PreviousWaiter)
    {
        ASSERT(Requester->PreviousWaiter->NextWaiter == Requester);
        Requester->PreviousWaiter->NextWaiter = Requester->NextWaiter;
    }

    //  Maintain list head/tail and report new adapter owner if we now have one

    if  (Requester == LastWaiter)
        LastWaiter = Requester->PreviousWaiter;

    if  (Requester == AdapterOwner)
    {
        AdapterOwner = Requester->NextWaiter;
        //  If there IS a new owner, tell it it can now begin mapping
        if  (AdapterOwner != NULL)
            AdapterOwner->Dma.ReportNewMappings();
    }

    //  Mark request as no longer queued
    Requester->NextWaiter = Requester->PreviousWaiter = NULL;
}

#if (DBG)
/*****************************************************************************
 * CIrpStream::DbgQueues()
 *****************************************************************************
 * Show the queues.
 */
void
CIrpStream::
DbgQueues
(   void
)
{
    m_kSpinLock.Acquire();  //  Don't walk without protection!
    PLIST_ENTRY entry = LockedQueue.Flink;

    _DbgPrintF(DEBUGLVL_BLAB,("DbgQueues() LockedQueue"));
    while(entry != &LockedQueue)
    {
        PIRP irp = PIRP(CONTAINING_RECORD(entry,IRP,Tail.Overlay.ListEntry));

        _DbgPrintF(DEBUGLVL_BLAB,("    %d 0x%8x",IRP_CONTEXT_IRP_STORAGE(irp)->IrpLabel,irp));

        entry = entry->Flink;
    }

    entry = MappedQueue.Flink;

    _DbgPrintF(DEBUGLVL_BLAB,("DbgQueues() MappedQueue"));
    while(entry != &MappedQueue)
    {
        PIRP irp = PIRP(CONTAINING_RECORD(entry,IRP,Tail.Overlay.ListEntry));

        _DbgPrintF(DEBUGLVL_BLAB,("    %d 0x%8x",IRP_CONTEXT_IRP_STORAGE(irp)->IrpLabel,irp));

        entry = entry->Flink;
    }
    m_kSpinLock.Release();
}


#include "stdio.h"


STDMETHODIMP_(void)
CIrpStream::
DbgRollCall
(
    IN ULONG MaxNameSize,
    OUT __out_ecount(MaxNameSize) PCHAR Name,
    OUT PIKSSHELLTRANSPORT* NextTransport,
    OUT PIKSSHELLTRANSPORT* PrevTransport
)

/*++

Routine Description:

    This routine produces a component name and the transport pointers.

Arguments:

Return Value:

--*/

{
    _DbgPrintF(DEBUGLVL_BLAB,("CIrpStream::DbgRollCall"));

    PAGED_CODE();

    ASSERT(Name);
    ASSERT(NextTransport);
    ASSERT(PrevTransport);

    ULONG references = AddRef() - 1; Release();

    RtlStringCchPrintfA (Name, MaxNameSize, "IrpStream%p refs=%d\n", this, references);
    *NextTransport = m_TransportSink;
    *PrevTransport = m_TransportSource;
}


#endif  // DBG

#endif  // PC_KDEXT
