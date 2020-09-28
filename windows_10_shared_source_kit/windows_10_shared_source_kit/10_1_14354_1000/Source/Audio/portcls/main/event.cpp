/*****************************************************************************
 * event.cpp - event support
 *****************************************************************************
 * Copyright (c) Microsoft.  All rights reserved.
 */

#include "private.h"





/*****************************************************************************
 * Functions
 */

#pragma code_seg("PAGE")

/*****************************************************************************
 * CheckEvent
 *****************************************************************************
 * Private function that checks the event property if it contains a node ID
 * and returns the node ID.
 *
 * Irp            the KS IOCTL
 * NodesCount     count of nodes on the filter
 * IsNodeEvent    set to TRUE if event is directed at a node, otherwise FALSE
 * NodeId         set to the node ID of the property
 */
NTSTATUS CheckEvent (IN PIRP Irp, IN ULONG NodesCount, OUT PBOOLEAN IsNodeEvent,
    OUT PULONG NodeId)
{
    PIO_STACK_LOCATION IrpStack;
    ULONG InputBufferLength;
    ULONG OutputBufferLength;
    ULONG AlignedBufferLength;

    PAGED_CODE();

    ASSERT (Irp);
    ASSERT (NodeId);
    ASSERT (IsNodeEvent);

    //
    // ---- CUT/PASTE snipped from KspEnableEvent -----
    // This code touches the event property and extracts the node ID (if present).
    //
    *IsNodeEvent = FALSE;

    //
    // Determine the offsets to both the Event and EventData parameters based
    // on the lengths of the DeviceIoControl parameters. A single allocation is
    // used to buffer both parameters. The EventData (or results on a support
    // query) is stored first, and the Event is stored second, on
    // FILE_QUAD_ALIGNMENT.
    //
    IrpStack = IoGetCurrentIrpStackLocation(Irp);
    InputBufferLength = IrpStack->Parameters.DeviceIoControl.InputBufferLength;
    OutputBufferLength = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    AlignedBufferLength = (OutputBufferLength + FILE_QUAD_ALIGNMENT) & ~FILE_QUAD_ALIGNMENT;

    //
    // Determine if the parameters have already been buffered by a previous
    // call to this function.
    //
    if (!Irp->AssociatedIrp.SystemBuffer) {
        //
        // Initially just check for the minimal event parameter length. The
        // actual minimal length will be validated when the event item is found.
        // Also ensure that the output and input buffer lengths are not set so
        // large as to overflow when aligned or added.
        //
        if ((InputBufferLength < sizeof(KSEVENT)) || (AlignedBufferLength < OutputBufferLength) || (AlignedBufferLength + InputBufferLength < AlignedBufferLength)) {
            return STATUS_INVALID_BUFFER_SIZE;
        }

        // Allocate a local buffer for the event
        PKSEVENT Event = (PKSEVENT)ExAllocatePoolWithTag(NonPagedPoolNx, InputBufferLength, 'tEcP');
        if(NULL == Event) {
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        __try {
            //
            // Validate the pointers if the client is not trusted.
            //
            if (Irp->RequestorMode != KernelMode) {
                ProbeForRead(IrpStack->Parameters.DeviceIoControl.Type3InputBuffer, InputBufferLength, sizeof(BYTE));
            }

            //
            // From now on we look at the KSEVENT structure and extract the node ID
            // if KSEVENT_TYPE_TOPOLOGY is set. This is done within the try-except
            // because this is a potentially untrusted buffer.
            //

            // 
            // Start by making a local copy
            // 
            RtlCopyMemory(PVOID(Event), PVOID(IrpStack->Parameters.DeviceIoControl.Type3InputBuffer), InputBufferLength );

            if (Event->Flags & KSEVENT_TYPE_TOPOLOGY) {
                //
                // Input buffer must include the node ID.
                //
                PKSE_NODE nodeEvent = (PKSE_NODE) Event;
                if (InputBufferLength < sizeof(*nodeEvent)) {
                    ExFreePoolWithTag(Event, 'tEcP');
                    return STATUS_INVALID_BUFFER_SIZE;
                }
                if (nodeEvent->NodeId >= NodesCount) {
                    ExFreePoolWithTag(Event, 'tEcP');
                    return STATUS_INVALID_DEVICE_REQUEST;
                }
                *NodeId = nodeEvent->NodeId;
                *IsNodeEvent = TRUE;
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            ExFreePoolWithTag(Event, 'tEcP');
            return GetExceptionCode();
        }

        ExFreePoolWithTag(Event, 'tEcP');
    }
    else
    {
        //
        // Retrieve a pointer to the Event, which is on FILE_LONG_ALIGNMENT within
        // the SystemBuffer, after any EventData.
        //
        PKSEVENT Event = (PKSEVENT)((PUCHAR)Irp->AssociatedIrp.SystemBuffer + AlignedBufferLength);

        //
        // Extract the node ID if present.
        //
        if (Event->Flags & KSEVENT_TYPE_TOPOLOGY) {
            //
            // Input buffer must include the node ID.
            //
            PKSE_NODE nodeEvent = (PKSE_NODE) Event;
            if (InputBufferLength < sizeof(*nodeEvent)) {
                return STATUS_INVALID_BUFFER_SIZE;
            }
            if (nodeEvent->NodeId >= NodesCount) {
                return STATUS_INVALID_DEVICE_REQUEST;
            }
            *NodeId = nodeEvent->NodeId;
            *IsNodeEvent = TRUE;
        }
    }

    return STATUS_SUCCESS;
}

/*****************************************************************************
 * PcHandleEnableEventWithTable()
 *****************************************************************************
 * Uses an event table to handle a KS enable event IOCTL.
 * This function uses the event table stored in pContext if the event is
 * directed at the filter/pin, otherwise uses the NodeEventTables if the IOCTL
 * was directed at a node.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcHandleEnableEventWithTable
(
    IN      PIRP                    pIrp,
    IN      PEVENT_CONTEXT          pContext,
    IN      ULONG                   NodeCount,
    IN      PEVENT_TABLE            NodeEventTables
)
{
    PAGED_CODE();

    ASSERT (pIrp);
    ASSERT (pContext);

    BOOLEAN IsNodeEvent;
    ULONG   NodeId;

    _DbgPrintF(DEBUGLVL_BLAB,("PcHandleEnableEventWithTable"));

    pIrp->Tail.Overlay.DriverContext[3] = pContext;

    NTSTATUS ntStatus = CheckEvent (pIrp, NodeCount, &IsNodeEvent, &NodeId);
    if (NT_SUCCESS(ntStatus))
    {
        if (IsNodeEvent)
        {
            ASSERT (NodeEventTables);
            pContext->ulEventSetCount = NodeEventTables[NodeId].EventSetCount;
            pContext->pEventSets = NodeEventTables[NodeId].EventSets;
        }

        ntStatus = KsEnableEvent (pIrp,
                           pContext->ulEventSetCount,
                           pContext->pEventSets,
                           NULL,
                           KSEVENTS_NONE,
                           NULL );
    }

    return ntStatus;

}

/*****************************************************************************
 * PcHandleDisableEventWithTable()
 *****************************************************************************
 * Uses an event table to handle a KS disable event IOCTL.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcHandleDisableEventWithTable
(
    IN      PIRP                    pIrp,
    IN      PEVENT_CONTEXT          pContext
)
{
    PAGED_CODE();

    ASSERT(pIrp);
    ASSERT(pContext);

    _DbgPrintF(DEBUGLVL_VERBOSE,("PcHandleDisableEventWithTable"));

    pIrp->Tail.Overlay.DriverContext[3] = pContext;

    return KsDisableEvent( pIrp,
                           &(pContext->pEventList->List),
                           KSEVENTS_SPINLOCK,
                           &(pContext->pEventList->ListLock) );
}

/*****************************************************************************
 * EventItemAddHandler()
 *****************************************************************************
 * KS-sytle event handler that handles Adds using the
 * PCEVENT_ITEM mechanism. Note that filter and pin events in the port do not
 * utilize this AddHandler, only events exposed by the miniport.
 */
NTSTATUS
EventItemAddHandler
(
    IN PIRP                     pIrp,
    IN PKSEVENTDATA             pEventData,
    IN PKSEVENT_ENTRY           pEventEntry
)
{
    PAGED_CODE();

    ASSERT(pIrp);

    NTSTATUS ntStatus = STATUS_SUCCESS;

    _DbgPrintF(DEBUGLVL_VERBOSE,("EventItemAddHandler"));

    // get the IRP stack location
    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation( pIrp );

    // get the event context
    PEVENT_CONTEXT pContext = PEVENT_CONTEXT(pIrp->Tail.Overlay.DriverContext[3]);

    //
    // Setup event request structure
    //
    PPCEVENT_REQUEST pPcEventRequest = new(NonPagedPoolNx,'rEcP') PCEVENT_REQUEST;

    if( !pPcEventRequest )
    {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }
    else
    {
        ASSERT(pIrp->AssociatedIrp.SystemBuffer);

        // IOCTL_KS_ENABLE_EVENT is a METHOD_NEITHER CTL_CODE, so the input buffer is
        // at  Parameters.DeviceIoControl.Type3InputBuffer with size Parameters.DeviceIoControl.InputBufferLength 
        // ... and the output buffer is at  Irp->UserBuffer with size  and Parameters.DeviceIoControl.OutputBufferLength 
        PKSEVENT  pKse = (PKSEVENT)irpSp->Parameters.DeviceIoControl.Type3InputBuffer;
        ULONG     cbKse = irpSp->Parameters.DeviceIoControl.InputBufferLength;

        if (cbKse == sizeof(KSE_NODE))
        {
            // Based on the size, the KSEVENT structure is actually a KSE_NODE or KSE_PIN.
            // These two structures are identical except for the ULONG context field (NodeId or PinId)
            // and can be used interchangeably.  
            // PCEVENT_REQUEST has a NodeId field, but not PinId field.  We will use this field to hold both
            // PinIds and NodeIds and use the KSEVENT_TYPE_TOPOLOGY flag to indicate the interpretation.
            pPcEventRequest->Node = ((PKSE_NODE)pKse)->NodeId;
        }

        //
        // Copy target information from the context structure (node taken care of above)
        //
        pPcEventRequest->MajorTarget    = pContext->pPropertyContext->pUnknownMajorTarget;
        pPcEventRequest->MinorTarget    = pContext->pPropertyContext->pUnknownMinorTarget;
        pPcEventRequest->EventItem      = NULL;

        // get the filter descriptor
        PPCFILTER_DESCRIPTOR pPcFilterDescriptor = pContext->pPropertyContext->pPcFilterDescriptor;

        if ((pKse->Flags & KSEVENT_TYPE_TOPOLOGY) == 0)
        {
            if( !pPcEventRequest->MinorTarget )
            {
                //
                // FILTER EVENT
                //

                if( ( pPcFilterDescriptor ) &&
                    ( pPcFilterDescriptor->AutomationTable ) )
                {
                    // search the filter's automation table for the event

                    const PCAUTOMATION_TABLE *pPcAutomationTable =
                        pPcFilterDescriptor->AutomationTable;

                    const PCEVENT_ITEM *pPcEventItem = pPcAutomationTable->Events;

                    for(ULONG ul = pPcAutomationTable->EventCount; ul--; )
                    {
                        if( IsEqualGUIDAligned( *pPcEventItem->Set,
                                                *pEventEntry->EventSet->Set ) &&
                            pPcEventItem->Id == pEventEntry->EventItem->EventId )
                        {
                            pPcEventRequest->EventItem = pPcEventItem;
                            break;
                        }

                        pPcEventItem = PPCEVENT_ITEM( PBYTE(pPcEventItem) + pPcAutomationTable->EventItemSize);
                    }
                }
            }
            else
            {
                //
                // PIN EVENT
                //

                // validate the pin id
                if( ( pPcFilterDescriptor ) &&
                    ( pContext->ulPinId < pPcFilterDescriptor->PinCount ) &&
                    ( pPcFilterDescriptor->Pins[pContext->ulPinId].AutomationTable ) )
                {
                    // search the pin's automation table for the event

                    const PCAUTOMATION_TABLE *pPcAutomationTable =
                        pPcFilterDescriptor->Pins[pContext->ulPinId].AutomationTable;

                    const PCEVENT_ITEM *pPcEventItem = pPcAutomationTable->Events;

                    for(ULONG ul = pPcAutomationTable->EventCount; ul--; )
                    {
                        if( IsEqualGUIDAligned( *pPcEventItem->Set,
                                                *pEventEntry->EventSet->Set ) &&
                            pPcEventItem->Id == pEventEntry->EventItem->EventId )
                        {
                            pPcEventRequest->EventItem = pPcEventItem;
                            break;
                        }

                        pPcEventItem = PPCEVENT_ITEM( PBYTE(pPcEventItem) + pPcAutomationTable->EventItemSize);
                    }
                }
            }
        }
        else
        {
            //
            //  NODE EVENT
            //

            // validate the node id
            if( ( pPcFilterDescriptor ) &&
                ( pPcEventRequest->Node < pPcFilterDescriptor->NodeCount ) )
            {
                const PCNODE_DESCRIPTOR *pPcNodeDescriptor =
                                                    PPCNODE_DESCRIPTOR
                                                    (   PBYTE(pPcFilterDescriptor->Nodes) +
                                                        pPcFilterDescriptor->NodeSize *
                                                        pPcEventRequest->Node
                                                    );
                const PCAUTOMATION_TABLE *pPcAutomationTable = pPcNodeDescriptor->AutomationTable;

                if (pPcAutomationTable)
                {
                    // search the node's automation table for the event
                    const PCEVENT_ITEM *pPcEventItem = pPcAutomationTable->Events;

                    for(ULONG ul = pPcAutomationTable->EventCount; ul--; )
                    {
                        if( IsEqualGUIDAligned( *pPcEventItem->Set,
                                                *pEventEntry->EventSet->Set ) &&
                            pPcEventItem->Id == pEventEntry->EventItem->EventId )
                        {
                            pPcEventRequest->EventItem = pPcEventItem;
                            break;
                        }

                        pPcEventItem = PPCEVENT_ITEM( PBYTE(pPcEventItem) + pPcAutomationTable->EventItemSize);
                    }
                }
            }
        }

        if( NT_SUCCESS(ntStatus) )
        {
            //
            // call the handler if we have an event item with a handler
            //
            if( pPcEventRequest->EventItem &&
                pPcEventRequest->EventItem->Handler )
            {
                PPCEVENT_ENTRY(pEventEntry)->EventItem = pPcEventRequest->EventItem;

                // PCEVENT_REQUEST doesn't have a field for Pin Ids, using the Flags
                // to tell if "Node" is a Pin factory Id or a Node Id
                if ((pKse->Flags & KSEVENT_TYPE_TOPOLOGY) == 0)
                {
                    PPCEVENT_ENTRY(pEventEntry)->PinId = pPcEventRequest->Node;
                    PPCEVENT_ENTRY(pEventEntry)->NodeId = (ULONG)-1;
                }
                else
                {
                    PPCEVENT_ENTRY(pEventEntry)->PinId = pContext->ulPinId;
                    PPCEVENT_ENTRY(pEventEntry)->NodeId = pPcEventRequest->Node;
                }
                PPCEVENT_ENTRY(pEventEntry)->pUnknownMajorTarget = pPcEventRequest->MajorTarget;
                PPCEVENT_ENTRY(pEventEntry)->pUnknownMinorTarget = pPcEventRequest->MinorTarget;

                pPcEventRequest->Verb       = PCEVENT_VERB_ADD;
                pPcEventRequest->Irp        = pIrp;
                pPcEventRequest->EventEntry = pEventEntry;


                //
                // call the handler
                //
                ntStatus = pPcEventRequest->EventItem->Handler( pPcEventRequest );
            }
            else
            {
                ntStatus = STATUS_NOT_FOUND;
            }
        }

        //
        // delete the request structure unless we are pending
        //
        if( ntStatus != STATUS_PENDING )
        {
            delete pPcEventRequest;
        }
        else
        {
            //
            // only requests with IRPs can be pending
            //
            ASSERT(pIrp);
        }
    }

    return ntStatus;
}

/*****************************************************************************
 * EventItemSupportHandler()
 *****************************************************************************
 * KS-sytle event handler that handles Supports using the
 * PCEVENT_ITEM mechanism.
 */
NTSTATUS
EventItemSupportHandler
(
    IN PIRP                 pIrp,
    IN PKSIDENTIFIER        pRequest,
    IN OUT PVOID            pData   OPTIONAL
)
{
    PAGED_CODE();

    ASSERT(pIrp);
    ASSERT(pRequest);

    NTSTATUS ntStatus = STATUS_SUCCESS;

    _DbgPrintF(DEBUGLVL_BLAB,("EventItemSupportHandler"));

    // get the IRP stack location
    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation( pIrp );

    // get the property/event context
    PEVENT_CONTEXT pContext = PEVENT_CONTEXT(pIrp->Tail.Overlay.DriverContext[3]);

    //
    // Setup event request structure
    //
    PPCEVENT_REQUEST pPcEventRequest = new(NonPagedPoolNx,'rEcP') PCEVENT_REQUEST;

    if( !pPcEventRequest )
    {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }
    else
    {
        ASSERT(pIrp->AssociatedIrp.SystemBuffer);

        // IOCTL_KS_ENABLE_EVENT is a METHOD_NEITHER CTL_CODE, so the input buffer is
        // at  Parameters.DeviceIoControl.Type3InputBuffer with size Parameters.DeviceIoControl.InputBufferLength 
        // ... and the output buffer is at  Irp->UserBuffer with size  and Parameters.DeviceIoControl.OutputBufferLength 
        PKSEVENT  pKse = (PKSEVENT)irpSp->Parameters.DeviceIoControl.Type3InputBuffer;
        ULONG     cbKse = irpSp->Parameters.DeviceIoControl.InputBufferLength;

        if (cbKse == sizeof(KSE_NODE))
        {
            // Based on the size, the KSEVENT structure is actually a KSE_NODE or KSE_PIN.
            // These two structures are identical except for the ULONG context field (NodeId or PinId)
            // and can be used interchangeably.  
            // PCEVENT_REQUEST has a NodeId field, but not PinId field.  We will use this field to hold both
            // PinIds and NodeIds and use the KSEVENT_TYPE_TOPOLOGY flag to indicate the interpretation.
            pPcEventRequest->Node = ((PKSE_NODE)pKse)->NodeId;
        }

        //
        // Copy target information from the context structure
        //
        pPcEventRequest->MajorTarget    = pContext->pPropertyContext->pUnknownMajorTarget;
        pPcEventRequest->MinorTarget    = pContext->pPropertyContext->pUnknownMinorTarget;
        pPcEventRequest->EventItem      = NULL;

        // get the filter descriptor
        PPCFILTER_DESCRIPTOR pPcFilterDescriptor = pContext->pPropertyContext->pPcFilterDescriptor;

        if ((pKse->Flags & KSEVENT_TYPE_TOPOLOGY) == 0)
        {
            if( !pPcEventRequest->MinorTarget )
            {
                //
                // FILTER EVENT
                //

                if( ( pPcFilterDescriptor ) &&
                    ( pPcFilterDescriptor->AutomationTable ) )
                {
                    // search the filter's automation table for the event

                    const PCAUTOMATION_TABLE *pPcAutomationTable =
                        pPcFilterDescriptor->AutomationTable;

                    const PCEVENT_ITEM *pPcEventItem = pPcAutomationTable->Events;

                    for(ULONG ul = pPcAutomationTable->EventCount; ul--; )
                    {
                        if( IsEqualGUIDAligned( *pPcEventItem->Set,
                                                pRequest->Set ) &&
                            pPcEventItem->Id == pRequest->Id )
                        {
                            pPcEventRequest->EventItem = pPcEventItem;
                            break;
                        }

                        pPcEventItem = PPCEVENT_ITEM( PBYTE(pPcEventItem) + pPcAutomationTable->EventItemSize);
                    }
                }
            }
            else
            {
                //
                // PIN EVENT
                //

                // validate the pin id
                if( ( pPcFilterDescriptor ) &&
                    ( pContext->ulPinId < pPcFilterDescriptor->PinCount ) &&
                    ( pPcFilterDescriptor->Pins[pContext->ulPinId].AutomationTable ) )
                {
                    // search the pin's automation table for the event

                    const PCAUTOMATION_TABLE *pPcAutomationTable =
                        pPcFilterDescriptor->Pins[pContext->ulPinId].AutomationTable;

                    const PCEVENT_ITEM *pPcEventItem = pPcAutomationTable->Events;

                    for(ULONG ul = pPcAutomationTable->EventCount; ul--; )
                    {
                        if( IsEqualGUIDAligned( *pPcEventItem->Set,
                                                pRequest->Set ) &&
                            pPcEventItem->Id == pRequest->Id )
                        {
                            pPcEventRequest->EventItem = pPcEventItem;
                            break;
                        }

                        pPcEventItem = PPCEVENT_ITEM( PBYTE(pPcEventItem) + pPcAutomationTable->EventItemSize);
                    }
                }
            }
        }
        else
        {
            //
            //  NODE EVENT
            //

            // validate the node id
            if( ( pPcFilterDescriptor ) &&
                ( pPcEventRequest->Node < pPcFilterDescriptor->NodeCount ) )
            {
                const PCNODE_DESCRIPTOR *pPcNodeDescriptor =
                                                    PPCNODE_DESCRIPTOR
                                                    (   PBYTE(pPcFilterDescriptor->Nodes) +
                                                        pPcFilterDescriptor->NodeSize *
                                                        pPcEventRequest->Node
                                                    );
                const PCAUTOMATION_TABLE *pPcAutomationTable = pPcNodeDescriptor->AutomationTable;

                if (pPcAutomationTable)
                {
                    // search the node's automation table for the event
                    const PCEVENT_ITEM *pPcEventItem = pPcAutomationTable->Events;

                    for(ULONG ul = pPcAutomationTable->EventCount; ul--; )
                    {
                        if( IsEqualGUIDAligned( *pPcEventItem->Set,
                                                pRequest->Set ) &&
                            pPcEventItem->Id == pRequest->Id )
                        {
                            pPcEventRequest->EventItem = pPcEventItem;
                            break;
                        }

                        pPcEventItem = PPCEVENT_ITEM( PBYTE(pPcEventItem) + pPcAutomationTable->EventItemSize);
                    }
                }
            }
        }

        if(NT_SUCCESS(ntStatus))
        {
            //
            // call the handler if we have an event item with a handler
            //
            if( pPcEventRequest->EventItem &&
                pPcEventRequest->EventItem->Handler )
            {
                pPcEventRequest->Verb       = PCEVENT_VERB_SUPPORT;
                pPcEventRequest->Irp        = pIrp;
                pPcEventRequest->EventEntry = NULL;

                //
                // call the handler
                //
                ntStatus = pPcEventRequest->EventItem->Handler( pPcEventRequest );
            }
            else
            {
                ntStatus = STATUS_NOT_FOUND;
            }
        }

        //
        // delete the request structure unless we are pending
        //
        if( ntStatus != STATUS_PENDING )
        {
            delete pPcEventRequest;
        }
        else
        {
            //
            // only requests with IRPs can be pending
            //
            ASSERT(pIrp);
        }
    }

    return ntStatus;
}

#pragma code_seg()
/*****************************************************************************
 * EventItemRemoveHandler()
 *****************************************************************************
 *
 */
void
EventItemRemoveHandler
(
    IN  PFILE_OBJECT    pFileObject,
    IN  PKSEVENT_ENTRY  pEventEntry
)
{
    ASSERT(pFileObject);
    ASSERT(pEventEntry);

    _DbgPrintF(DEBUGLVL_VERBOSE,("EventItemRemoveHandler"));

    PPCEVENT_ENTRY pPcEventEntry = PPCEVENT_ENTRY(pEventEntry);

    //
    // Setup event request structure
    //
    PPCEVENT_REQUEST pPcEventRequest = new(NonPagedPoolNx,'rEcP') PCEVENT_REQUEST;

    if( pPcEventRequest )
    {
        //
        // Fill out the event request for the miniport
        //
        pPcEventRequest->MajorTarget    = pPcEventEntry->pUnknownMajorTarget;
        pPcEventRequest->MinorTarget    = pPcEventEntry->pUnknownMinorTarget;
        pPcEventRequest->Node           = pPcEventEntry->NodeId;
        pPcEventRequest->EventItem      = pPcEventEntry->EventItem;
        pPcEventRequest->Verb           = PCEVENT_VERB_REMOVE;
        pPcEventRequest->Irp            = NULL;
        pPcEventRequest->EventEntry     = pEventEntry;

        if( ( pPcEventEntry->EventItem ) &&
            ( pPcEventEntry->EventItem->Handler ) )
        {
            pPcEventEntry->EventItem->Handler( pPcEventRequest );
        }

        delete pPcEventRequest;
    }

    RemoveEntryList( &(pEventEntry->ListEntry) );
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * PcCompletePendingEventRequest()
 *****************************************************************************
 * Completes a pending event request.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcCompletePendingEventRequest
(
    IN      PPCEVENT_REQUEST    EventRequest,
    IN      NTSTATUS            NtStatus
)
{
    PAGED_CODE();

    ASSERT(EventRequest);
    ASSERT(NtStatus != STATUS_PENDING);

    if (!NT_ERROR(NtStatus))
    {
        EventRequest->Irp->IoStatus.Information = 0;
    }

    EventRequest->Irp->IoStatus.Status = NtStatus;
    IoCompleteRequest(EventRequest->Irp,IO_NO_INCREMENT);

    delete EventRequest;

    return STATUS_SUCCESS;
}

/*****************************************************************************
 * PcFreeEventTable()
 *****************************************************************************
 * Frees allocated memory in a EVENT_TABLE structure.
 */
PORTCLASSAPI
void
NTAPI
PcFreeEventTable
(
    IN      PEVENT_TABLE         EventTable
)
{
    _DbgPrintF(DEBUGLVL_VERBOSE,("PcFreeEventTable"));

    PAGED_CODE();

    ASSERT(EventTable);

    ASSERT((!EventTable->EventSets) == (!EventTable->EventSetCount));
    //  EventSets and EventSetCount must be non-NULL/non-zero, or NULL/zero

    ASSERT(EventTable->StaticSets == (!EventTable->StaticItems));
    //  StaticSets and StaticItems must be TRUE/NULL, or FALSE/non-null

    PBOOLEAN     staticItem = EventTable->StaticItems;
    if (staticItem)
    {
        PKSEVENT_SET eventSet   = EventTable->EventSets;
        if (eventSet)
        {
            for( ULONG count = EventTable->EventSetCount;
                 count--;
                 eventSet++, staticItem++)
            {
                if ((! *staticItem) && eventSet->EventItem)
                {
                    ExFreePool(PVOID(eventSet->EventItem));
                }
            }
        }
        ExFreePool(EventTable->StaticItems);
        EventTable->StaticItems = NULL;
    }

    if (EventTable->EventSets && !EventTable->StaticSets)
    {
        EventTable->EventSetCount = 0;
        ExFreePool(EventTable->EventSets);
        EventTable->EventSets = NULL;
    }
    EventTable->StaticSets = TRUE;
}

/*****************************************************************************
 * PcAddToEventTable()
 *****************************************************************************
 * Adds an EVENT_ITEM event table to a EVENT_TABLE structure.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcAddToEventTable
(
    IN OUT  PEVENT_TABLE            EventTable,
    IN      ULONG                   EventItemCount,
    IN      const PCEVENT_ITEM *    EventItems,
    IN      ULONG                   EventItemSize
)
{
    PAGED_CODE();

    ASSERT(EventTable);
    ASSERT(EventItems);
    ASSERT(EventItemSize >= sizeof(PCEVENT_ITEM));

    _DbgPrintF(DEBUGLVL_VERBOSE,("PcAddToEventTable"));

#define ADVANCE(item) (item = PPCEVENT_ITEM(PBYTE(item) + EventItemSize))

// Disable the prefast warning that is erroneous.
#pragma prefast(disable: __WARNING_BUFFER_OVERFLOW, "Erroneous buffer overflow declared in following code")

    ASSERT((!EventTable->EventSets) == (!EventTable->EventSetCount));
    //  values must be non-NULL/non-zero, or NULL/zero.

    //
    // Determine how many sets we will end up with.
    //
    ULONG setCount = EventTable->EventSetCount;
    const PCEVENT_ITEM *item = EventItems;
    for (ULONG count = EventItemCount; count--; ADVANCE(item))
    {
        BOOLEAN countThis = TRUE;

        //
        // See if it's already in the table.
        //
        PKSEVENT_SET eventSet = EventTable->EventSets;
        for
        (   ULONG count2 = EventTable->EventSetCount;
            count2--;
            eventSet++
        )
        {
            if (IsEqualGUIDAligned(*item->Set,*eventSet->Set))
            {
                countThis = FALSE;
                break;
            }
        }

        if (countThis)
        {
            //
            // See if it's appeared in the list previously.
            //
            for
            (
                const PCEVENT_ITEM *prevItem = EventItems;
                prevItem != item;
                ADVANCE(prevItem)
            )
            {
                if (IsEqualGUIDAligned(*item->Set,*prevItem->Set))
                {
                    countThis = FALSE;
                    break;
                }
            }
        }

        if (countThis)
        {
            setCount++;
        }
    }

    NTSTATUS ntStatus = STATUS_SUCCESS;

    //
    // Make a new set table.
    //
    ASSERT(setCount);
    ASSERT(setCount >= EventTable->EventSetCount);
    //
    // Allocate memory required for the set table.
    //
    PKSEVENT_SET newTable;
    if  ((setCount < EventTable->EventSetCount) ||
         (ULONGLONG( sizeof(KSEVENT_SET) * setCount ) != (ULONGLONG(sizeof(KSEVENT_SET)) * ULONGLONG(setCount))))
    {
        newTable = NULL;    //  Eliminate potential overflows
    }
    else
    {
        newTable = 
        PKSEVENT_SET
        (
            ExAllocatePoolWithTag
            (
                NonPagedPoolNx,
                sizeof(KSEVENT_SET) * setCount,
                'tEcP'
            )
        );
    }

    //
    // Allocate memory for the static items flags.
    //
    PBOOLEAN newStaticItems = NULL;
    if (newTable)
    {
        newStaticItems =
            PBOOLEAN
            (
                ExAllocatePoolWithTag
                (
                    PagedPool,
                    sizeof(BOOLEAN) * setCount,
                    'bScP'
                )
            );

        if (! newStaticItems)
        {
            ExFreePool(newTable);
            newTable = NULL;
        }
    }

    if (newTable)
    {
        //
        // Initialize the new set table.
        //
        RtlZeroMemory
        (
            PVOID(newTable),
            sizeof(KSEVENT_SET) * setCount
        );

        if (EventTable->EventSetCount != 0)
        {
            RtlCopyMemory
            (
                PVOID(newTable),
                PVOID(EventTable->EventSets),
                sizeof(KSEVENT_SET) * EventTable->EventSetCount
            );
        }

        //
        // Initialize the new static items flags.
        //
        RtlFillMemory
        (
            PVOID(newStaticItems),
            sizeof(BOOLEAN) * setCount,
            0xff
        );

        if (EventTable->StaticItems && EventTable->EventSetCount)
        {
            //
            // Flags existed before...copy them.
            //
            RtlCopyMemory
            (
                PVOID(newStaticItems),
                PVOID(EventTable->StaticItems),
                sizeof(BOOLEAN) * EventTable->EventSetCount
            );
        }

        //
        // Assign set GUIDs to the new set entries.
        //
        PKSEVENT_SET addHere =
            newTable + EventTable->EventSetCount;

        const PCEVENT_ITEM *item2 = EventItems;
        for (ULONG count = EventItemCount; count--; ADVANCE(item2))
        {
            BOOLEAN addThis = TRUE;

            //
            // See if it's already in the table.
            //
            for( PKSEVENT_SET eventSet = newTable;
                 eventSet != addHere;
                 eventSet++)
            {
                if (IsEqualGUIDAligned(*item2->Set,*eventSet->Set))
                {
                    addThis = FALSE;
                    break;
                }
            }

            if (addThis)
            {
                addHere->Set = item2->Set;
                addHere++;
            }
        }

        ASSERT(addHere == newTable + setCount);

        //
        // Free old allocated tables.
        //
        if (EventTable->EventSets && (!EventTable->StaticSets))
        {
            ExFreePool(EventTable->EventSets);
        }
        if (EventTable->StaticItems)
        {
            ExFreePool(EventTable->StaticItems);
        }

        //
        // Install the new tables.
        //
        EventTable->EventSetCount   = setCount;
        EventTable->EventSets       = newTable;
        EventTable->StaticSets      = FALSE;
        EventTable->StaticItems     = newStaticItems;
    }
    else
    {
        //  if allocations fail, return error and
        //  keep sets and items as they were.
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Now we have an event set table that contains all the sets we need.
    //
    if (NT_SUCCESS(ntStatus))
    {
        //
        // For each set...
        //
        PKSEVENT_SET    eventSet    = EventTable->EventSets;
        PBOOLEAN        staticItem  = EventTable->StaticItems;
        for
        (   ULONG count = EventTable->EventSetCount;
            count--;
            eventSet++, staticItem++
        )
        {
            //
            // Check to see how many new items we have.
            //
            ULONG itemCount = eventSet->EventsCount;
            const PCEVENT_ITEM *item2 = EventItems;
            ULONG count2;
            for (count2 = EventItemCount; count2--; ADVANCE(item2))
            {
                if (IsEqualGUIDAligned(*item2->Set,*eventSet->Set))
                {
                    itemCount++;
                }
            }

            ASSERT(itemCount >= eventSet->EventsCount);
            if (itemCount != eventSet->EventsCount)
            {
                //
                // Allocate memory required for the items table.
                //
                PKSEVENT_ITEM newTable2;
                if  ((itemCount < eventSet->EventsCount) ||
                     (ULONGLONG( sizeof(KSEVENT_ITEM) * itemCount ) != (ULONGLONG(sizeof(KSEVENT_ITEM)) * ULONGLONG(itemCount))))
                {
                    newTable2 = NULL;   //  Force overflows to result in allocation failures
                }
                else
                {
                    newTable2 =
                    PKSEVENT_ITEM
                    (
                        ExAllocatePoolWithTag
                        (
                            NonPagedPoolNx,
                            sizeof(KSEVENT_ITEM) * itemCount,
                            'iEcP'
                        )
                    );
                }

                if (! newTable2)
                {
                    ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                    break;
                }

                //
                // Initialize the table.
                //
                RtlZeroMemory
                (
                    PVOID(newTable2),
                    sizeof(KSEVENT_ITEM) * itemCount
                );

                if (eventSet->EventsCount)
                {
                    RtlCopyMemory
                    (
                        PVOID(newTable2),
                        PVOID(eventSet->EventItem),
                        sizeof(KSEVENT_ITEM) * eventSet->EventsCount
                    );
                }

                //
                // Create the new items.
                //
                PKSEVENT_ITEM addHere =
                    newTable2 + eventSet->EventsCount;

                item2 = EventItems;
                for (count2 = EventItemCount; count2--; ADVANCE(item2))
                {
                    if (IsEqualGUIDAligned(*item2->Set,*eventSet->Set))
                    {
                        addHere->EventId            = item2->Id;
                        addHere->DataInput          = sizeof( KSEVENTDATA );
                        addHere->ExtraEntryData     = sizeof( PCEVENT_ENTRY ) - sizeof( KSEVENT_ENTRY );
                        addHere->AddHandler         = EventItemAddHandler;
                        addHere->RemoveHandler      = EventItemRemoveHandler;
                        addHere->SupportHandler     = EventItemSupportHandler;
                        addHere++;
                    }
                }

                ASSERT(addHere == newTable2 + itemCount);

                //
                // Free old allocated table.
                //
                if (eventSet->EventItem && ! *staticItem)
                {
                    ExFreePool(PVOID(eventSet->EventItem));
                }

                //
                // Install the new tables.
                //
                eventSet->EventsCount = itemCount;
                eventSet->EventItem    = newTable2;
                *staticItem = FALSE;
            }
        }
    }

// Enable prefast back.
#pragma prefast(enable: __WARNING_BUFFER_OVERFLOW, "Erroneous buffer overflow declared in previous code")

    return ntStatus;
}


#pragma code_seg()
/*****************************************************************************
 * PcGenerateEventList()
 *****************************************************************************
 * Walks an event list and generates desired events.
 */
PORTCLASSAPI
void
NTAPI
PcGenerateEventList
(
    IN      PINTERLOCKED_LIST   EventList,
    IN      GUID*               Set     OPTIONAL,
    IN      ULONG               EventId,
    IN      BOOL                PinEvent,
    IN      ULONG               PinId,
    IN      BOOL                NodeEvent,
    IN      ULONG               NodeId
)
{
    ASSERT(EventList);

    KIRQL           oldIrql;
    PLIST_ENTRY     ListEntry;
    PKSEVENT_ENTRY  EventEntry;

    if( EventList )
    {
        ASSERT( KeGetCurrentIrql() <= DISPATCH_LEVEL );

        // acquire the event list lock
        KeAcquireSpinLock( &(EventList->ListLock), &oldIrql );

        // only walk a non-empty list
        if( !IsListEmpty( &(EventList->List) ) )
        {
            for( ListEntry = EventList->List.Flink;
                 ListEntry != &(EventList->List);
                 ListEntry = ListEntry->Flink )
            {
                EventEntry = (PKSEVENT_ENTRY) CONTAINING_RECORD( ListEntry,
                                                                 KSEVENT_ENTRY,
                                                                 ListEntry );

                if( ( !Set
                      ||
                      IsEqualGUIDAligned( *Set, *(EventEntry->EventSet->Set) )
                    )
                    &&
                    ( EventId == EventEntry->EventItem->EventId
                    )
                    &&
                    ( !PinEvent
                      ||
                      ( PinId == PPCEVENT_ENTRY(EventEntry)->PinId )
                    )
                    &&
                    ( !NodeEvent
                      ||
                      ( NodeId == PPCEVENT_ENTRY(EventEntry)->NodeId )
                    )
                  )
                {
                    KsGenerateEvent( EventEntry );
                }
            }
        }

        // release the event list lock
        KeReleaseSpinLock( &(EventList->ListLock), oldIrql );
    }
}

/*****************************************************************************
 * PcGenerateEventDeferredRoutine()
 *****************************************************************************
 * This DPC routine is used when GenerateEventList is called at greater
 * that DISPATCH_LEVEL.
 *
 * This routine is called one time for each deferred event stored. After the 
 * information is consumed, it's marked free again for another deferred event.
 */
void
NTAPI
PcGenerateEventDeferredRoutine
(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,       // PEVENT_DPC_CONTEXT
    IN PVOID SystemArgument1,       // PINTERLOCKED_LIST
    IN PVOID SystemArgument2
)
{
    ASSERT(Dpc);
    ASSERT(DeferredContext);
    ASSERT(SystemArgument1);

    PEVENT_DPC_CONTEXT  Context = PEVENT_DPC_CONTEXT(DeferredContext);
    PINTERLOCKED_LIST   EventList = PINTERLOCKED_LIST(SystemArgument1);

    if( Context && EventList )
    {
        PcGenerateEventList( EventList,
                             Context->Set,
                             Context->EventId,
                             Context->PinEvent,
                             Context->PinId,
                             Context->NodeEvent,
                             Context->NodeId );

        // The scheduling routine uses an interlocked Compare Exchange routine to
        // fill it, thus we use one to clear the location.
        InterlockedExchange((LONG*)&Context->ContextInUse,FALSE);
    }
}
