/*****************************************************************************
 * property.cpp - property support
 *****************************************************************************
 * Copyright (c) Microsoft.  All rights reserved.
 */

#include "private.h"













#pragma code_seg("PAGE")


//
// Streaming position properties.
//
typedef struct {
    const GUID*     Set;
    ULONG           Id;
} PC_PROPERTY_ID, *PPC_PROPERTY_ID;

PC_PROPERTY_ID PositionProperties[] = 
{
    {&KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_SETWRITEPACKET},
    {&KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_PRESENTATION_POSITION},
    {&KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_GETREADPACKET},
    {&KSPROPSETID_RtAudio, KSPROPERTY_RTAUDIO_PACKETCOUNT},
    {&KSPROPSETID_Audio,   KSPROPERTY_AUDIO_POSITION},
    {&KSPROPSETID_Audio,   KSPROPERTY_AUDIO_POSITIONEX},
};

#define PositionPropertyCount (SIZEOF_ARRAY(PositionProperties))

/*****************************************************************************
 * Functions
 */

#pragma code_seg("PAGE")

/*****************************************************************************
 * CheckProperty
 *****************************************************************************
 * Private function that checks the property if it contains a node ID and
 * returns the node ID.
 *
 * Irp            the KS IOCTL
 * NodesCount     count of nodes on the filter
 * IsNodeProperty set to TRUE if property is directed at a node, otherwise FALSE
 * NodeId         set to the node ID of the property
 */
NTSTATUS CheckProperty (IN PIRP Irp, IN ULONG NodesCount, OUT PBOOLEAN IsNodeProperty,
    OUT PULONG NodeId)
{
    PIO_STACK_LOCATION IrpStack;
    ULONG InputBufferLength;
    ULONG OutputBufferLength;
    ULONG AlignedBufferLength;
    ULONG Flags;

    PAGED_CODE();

    ASSERT (Irp);
    ASSERT (NodeId);
    ASSERT (IsNodeProperty);

    //
    // ---- CUT/PASTE snipped from KspPropertyHandler -----
    // This code touches the property and extracts the node ID (if present).
    //
    *IsNodeProperty = FALSE;
      
    //
    // Determine the offsets to both the Property and UserBuffer parameters based
    // on the lengths of the DeviceIoControl parameters. A single allocation is
    // used to buffer both parameters. The UserBuffer (or results on a support
    // query) is stored first, and the Property is stored second, on
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
        // Initially just check for the minimal property parameter length. The
        // actual minimal length will be validated when the property item is found.
        // Also ensure that the output and input buffer lengths are not set so
        // large as to overflow when aligned or added.
        //
        if ((InputBufferLength < sizeof(KSPROPERTY)) || (AlignedBufferLength < OutputBufferLength) || (AlignedBufferLength + InputBufferLength < AlignedBufferLength)) {
            return STATUS_INVALID_BUFFER_SIZE;
        }

        // Allocate a local buffer for the property
        PKSPROPERTY Property = (PKSPROPERTY)ExAllocatePoolWithTag(NonPagedPoolNx, InputBufferLength, 'rPcP');
        if(NULL == Property) {
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
            // From now on we look at the PKSPROPERTY structure and extract the node ID
            // if KSPROPERTY_TYPE_TOPOLOGY is set. This is done within the try-except
            // because this is a potentially untrusted buffer.
            //

            //
            // Start by making a local copy
            // 
            RtlCopyMemory(PVOID(Property), PVOID(IrpStack->Parameters.DeviceIoControl.Type3InputBuffer), InputBufferLength);

            Flags = Property->Flags;
            //
            // HACK!  This is done because wdmaud sets this bit when requesting node names (bug 320925).
            //
            if (IsEqualGUIDAligned(Property->Set, KSPROPSETID_Topology)) {
                Flags = Property->Flags & ~KSPROPERTY_TYPE_TOPOLOGY;
            }
            if (Flags & KSPROPERTY_TYPE_TOPOLOGY) {
                //
                // Input buffer must include the node ID.
                //
                PKSP_NODE nodeProperty = (PKSP_NODE) Property;

                if (InputBufferLength < sizeof(*nodeProperty)) {
                    ExFreePoolWithTag(Property, 'rPcP');
                    return STATUS_INVALID_BUFFER_SIZE;
                }

                //
                // Only read the node ID once to protect against it being modified to an invalid value
                // after validation.
                //
                ULONG ulNodeId = nodeProperty->NodeId;

                if (ulNodeId >= NodesCount) {
                    ExFreePoolWithTag(Property, 'rPcP');
                    return STATUS_INVALID_DEVICE_REQUEST;
                }
                *NodeId = ulNodeId;
                *IsNodeProperty = TRUE;
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            ExFreePoolWithTag(Property, 'rPcP');
            return GetExceptionCode();
        }

        ExFreePoolWithTag(Property, 'rPcP');
    }
    else
    {
        //
        // Retrieve a pointer to the property, which is on FILE_LONG_ALIGNMENT within
        // the SystemBuffer, after any PropertyData.
        //
        PKSPROPERTY Property = (PKSPROPERTY)((PUCHAR)Irp->AssociatedIrp.SystemBuffer + AlignedBufferLength);
        Flags = Property->Flags;





        if (IsEqualGUIDAligned(Property->Set, KSPROPSETID_Topology)) {
            Flags = Property->Flags & ~KSPROPERTY_TYPE_TOPOLOGY;
        }
        if (Flags & KSPROPERTY_TYPE_TOPOLOGY) {
            //
            // Input buffer must include the node ID.
            //
            PKSP_NODE nodeProperty = (PKSP_NODE) Property;
            if (InputBufferLength < sizeof(*nodeProperty)) {
                return STATUS_INVALID_BUFFER_SIZE;
            }
            if (nodeProperty->NodeId >= NodesCount) {
                return STATUS_INVALID_DEVICE_REQUEST;
            }
            *NodeId = nodeProperty->NodeId;
            *IsNodeProperty = TRUE;
        }
    }

    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * CheckPositionProperty
 *****************************************************************************
 * Private function that checks if the specified property is a stream
 * position property.
 *
 */
NTSTATUS 
CheckPositionProperty(
    _In_    PIRP        Irp, 
    _Out_   BOOLEAN *   IsPositionProperty
)
{
    NTSTATUS status = STATUS_SUCCESS;
    PKSPROPERTY Property = NULL;
    BOOLEAN FreeProperty = FALSE;
    BOOLEAN isPositionProperty = FALSE;
    PIO_STACK_LOCATION IrpStack;
    ULONG InputBufferLength;
    ULONG OutputBufferLength;
    ULONG AlignedBufferLength;

    PAGED_CODE();

    ASSERT (Irp);
    ASSERT (IsPositionProperty);

    //
    // ---- CUT/PASTE snipped from KspPropertyHandler -----
    // This code touches the property and extracts the node ID (if present).
    //
      
    //
    // Determine the offsets to both the Property and UserBuffer parameters based
    // on the lengths of the DeviceIoControl parameters. A single allocation is
    // used to buffer both parameters. The UserBuffer (or results on a support
    // query) is stored first, and the Property is stored second, on
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
    if (!Irp->AssociatedIrp.SystemBuffer) 
    {
        //
        // Initially just check for the minimal property parameter length. The
        // actual minimal length will be validated when the property item is found.
        // Also ensure that the output and input buffer lengths are not set so
        // large as to overflow when aligned or added.
        //
        if ((InputBufferLength < sizeof(KSPROPERTY)) || (AlignedBufferLength < OutputBufferLength) || (AlignedBufferLength + InputBufferLength < AlignedBufferLength)) 
        {
            status = STATUS_INVALID_BUFFER_SIZE;
            goto Done;
        }

        // Allocate a local buffer for the property
        Property = (PKSPROPERTY)ExAllocatePoolWithTag(NonPagedPoolNx, InputBufferLength, 'rPcP');
        if(NULL == Property) 
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto Done;
        }

        FreeProperty = TRUE;
        
        __try 
        {
            //
            // Validate the pointers if the client is not trusted.
            //
            if (Irp->RequestorMode != KernelMode) {
                ProbeForRead(IrpStack->Parameters.DeviceIoControl.Type3InputBuffer, InputBufferLength, sizeof(BYTE));
            }

            //
            // Make a local copy.
            // 
            RtlCopyMemory(PVOID(Property), PVOID(IrpStack->Parameters.DeviceIoControl.Type3InputBuffer), InputBufferLength);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            status = GetExceptionCode();
            goto Done;
        }
    }
    else
    {
        //
        // Retrieve a pointer to the property, which is on FILE_LONG_ALIGNMENT within
        // the SystemBuffer, after any PropertyData.
        //
        Property = (PKSPROPERTY)((PUCHAR)Irp->AssociatedIrp.SystemBuffer + AlignedBufferLength);
    }

    //
    // Check if this is a position property.
    //
    if (!(Property->Flags & KSPROPERTY_TYPE_TOPOLOGY) &&
        !(Property->Flags & KSPROPERTY_TYPE_BASICSUPPORT))
    {
        for (int i = 0; i<PositionPropertyCount; ++i)
        {
            if (IsEqualGUIDAligned(*PositionProperties[i].Set, Property->Set) &&
                (PositionProperties[i].Id == Property->Id)) 
            {
                isPositionProperty = TRUE;
                break;
            }
        }
    }
    
    status = STATUS_SUCCESS;
    
Done:
    
    *IsPositionProperty = isPositionProperty;
    
    if (FreeProperty)
    {
        ExFreePoolWithTag(Property, 'rPcP');
        FreeProperty = FALSE;
    }

    return status;
}

/*****************************************************************************
 * PcHandlePropertyWithTable()
 *****************************************************************************
 * Uses a property table to handle a property request IOCTL.
 * This function uses the 'PropertyTable' if the property is directed at the
 * filter or pin, or uses the 'NodePropertyTables' if the property is directed
 * at a node.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcHandlePropertyWithTable
(   IN      PIRP                    pIrp,
    IN      PROPERTY_TABLE          PropertyTable,
    IN      ULONG                   NodeCount,
    IN      PPROPERTY_TABLE         NodePropertyTables,
    IN      PPROPERTY_CONTEXT       pPropertyContext
)
{
    PAGED_CODE();

    ASSERT (pIrp);
    ASSERT (pPropertyContext);

    BOOLEAN IsNodeProperty;
    ULONG   NodeId;

    pIrp->Tail.Overlay.DriverContext[3] = pPropertyContext;

    NTSTATUS ntStatus = CheckProperty (pIrp, NodeCount, &IsNodeProperty, &NodeId);
    if (NT_SUCCESS(ntStatus))
    {
        if (IsNodeProperty)
        {
            ASSERT (NodePropertyTables);
            ntStatus = KsPropertyHandler
            (
                pIrp,
                NodePropertyTables[NodeId].PropertySetCount,
                NodePropertyTables[NodeId].PropertySets
            );
        }
        else
        {
            ntStatus = KsPropertyHandler
            (
                pIrp,
                PropertyTable.PropertySetCount,
                PropertyTable.PropertySets
            );
        }
    }

    return ntStatus;
}

/*****************************************************************************
 * PcDispatchProperty()
 *****************************************************************************
 * Dispatch a property via a PCPROPERTY_ITEM entry.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcDispatchProperty
(
    IN          PIRP                pIrp            OPTIONAL,
    IN          PPROPERTY_CONTEXT   pPropertyContext,
    IN const    KSPROPERTY_SET *    pKsPropertySet  OPTIONAL,
    IN          ULONG               ulIdentifierSize,
    IN          PKSIDENTIFIER       pKsIdentifier,
    IN OUT      PULONG              pulDataSize,
    IN OUT      PVOID               pvData          OPTIONAL
)
{
    PAGED_CODE();

    ASSERT(pPropertyContext);
    ASSERT(pKsIdentifier);
    ASSERT(pulDataSize);

    PPCPROPERTY_REQUEST pPcPropertyRequest = 
        new(NonPagedPoolNx,'rPcP') PCPROPERTY_REQUEST;

    NTSTATUS ntStatus;
    if (! pPcPropertyRequest)
    {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }
    else
    {
        //
        // Copy target information from the context structure.
        //
        pPcPropertyRequest->MajorTarget = 
            pPropertyContext->pUnknownMajorTarget;
        pPcPropertyRequest->MinorTarget = 
            pPropertyContext->pUnknownMinorTarget;
        pPcPropertyRequest->Node = 
            pPropertyContext->ulNodeId;

        //
        // Determine the value size.
        //
        pPcPropertyRequest->ValueSize = *pulDataSize;

        //
        // If the node number is in the instance data, extract it.
        // TODO:  Remove this when node objects rule.
        //
        if  (   (pKsIdentifier->Flags & KSPROPERTY_TYPE_TOPOLOGY)
            &&  (pPcPropertyRequest->Node == ULONG(-1))
            )
        {
            //
            // Get the node id and remaining instance.
            //
            if  (ulIdentifierSize < sizeof(KSP_NODE))
            {
                delete pPcPropertyRequest;

                return STATUS_INVALID_BUFFER_SIZE;
            }

            PKSP_NODE pKsPNode = PKSP_NODE(pKsIdentifier);

            pPcPropertyRequest->Node = pKsPNode->NodeId;

            pPcPropertyRequest->InstanceSize = 
                ulIdentifierSize - sizeof(KSP_NODE);

            pPcPropertyRequest->Instance = 
                (   (pPcPropertyRequest->InstanceSize == 0) 
                ?   NULL 
                :   PVOID(pKsPNode + 1)
                );
        }
        else
        {
            //
            // No node in the instance...get generic instance if any.
            //
            pPcPropertyRequest->InstanceSize =
                ulIdentifierSize - sizeof(KSIDENTIFIER);

            pPcPropertyRequest->Instance = 
                (   (pPcPropertyRequest->InstanceSize == 0) 
                ?   NULL 
                :   PVOID(pKsIdentifier + 1)
                );
        }

        if (pKsPropertySet)
        {
            ASSERT(pKsPropertySet->PropertyItem);

            //
            // Find the property item in the KS-style list.
            //
            const KSPROPERTY_ITEM *pKsPropertyItem = pKsPropertySet->PropertyItem;
            ULONG loop;
            for (loop = pKsPropertySet->PropertiesCount; loop; pKsPropertyItem++, loop--)
            {
                if (pKsPropertyItem->PropertyId == pKsIdentifier->Id)
                    break;
            }

            if ((0 == loop) || (pKsPropertyItem->PropertyId != pKsIdentifier->Id))
            {
                delete pPcPropertyRequest;
                return STATUS_INVALID_PARAMETER;
            }

            //
            // The property item is stashed in the Relations field if this is not
            // a node property.  If it is, we have to find the node property in
            // the original list.
            //
            pPcPropertyRequest->PropertyItem = 
                PPCPROPERTY_ITEM(pKsPropertyItem->Relations);
        }
        else
        {
            //
            // No KS set was supplied.  We need to look in the original list
            // associated with the node.
            //
            pPcPropertyRequest->PropertyItem = NULL;
        }

        if (! pPcPropertyRequest->PropertyItem)
        {
            PPCFILTER_DESCRIPTOR pPcFilterDescriptor =
                pPropertyContext->pPcFilterDescriptor;

            if  (   pPcFilterDescriptor
                &&  (   pPcPropertyRequest->Node 
                    <   pPcFilterDescriptor->NodeCount
                    )
                )
            {
                const PCNODE_DESCRIPTOR *pPcNodeDescriptor =
                                                    PPCNODE_DESCRIPTOR
                                                    (   PBYTE(pPcFilterDescriptor->Nodes) +
                                                        pPcFilterDescriptor->NodeSize *
                                                        pPcPropertyRequest->Node
                                                    );
                const PCAUTOMATION_TABLE *pPcAutomationTable = pPcNodeDescriptor->AutomationTable;

                if (pPcAutomationTable)
                {
                //
                // Valid node...search the original property item list.
                //
                const PCPROPERTY_ITEM *pPcPropertyItem = 
                    pPcAutomationTable->Properties;
                for (ULONG ul = pPcAutomationTable->PropertyCount; ul--; )
                {
                    if  (   IsEqualGUIDAligned
                            (   *pPcPropertyItem->Set
                            ,   pKsIdentifier->Set
                            )
                        &&  (pPcPropertyItem->Id == pKsIdentifier->Id)
                        )
                    {
                        pPcPropertyRequest->PropertyItem = pPcPropertyItem;
                        break;
                    }

                    pPcPropertyItem = 
                        PPCPROPERTY_ITEM
                        (   PBYTE(pPcPropertyItem) 
                        +   pPcAutomationTable->PropertyItemSize
                        );
                }
            }
            else
            {
                //
                    //  No automation table found
                    //
                    ntStatus = STATUS_NOT_FOUND;
                }
            }
            else
            {
                //
                // The node ID was invalid.
                //
                ntStatus = STATUS_NOT_FOUND;
            }
        }

        //
        // Call the handler if we have a property item with a handler.
        //
        if  (   pPcPropertyRequest->PropertyItem
            &&  pPcPropertyRequest->PropertyItem->Handler
            )
        {
            ASSERT(pIrp);
            
            pPcPropertyRequest->Verb    = pKsIdentifier->Flags;
            pPcPropertyRequest->Value   = pvData;
            pPcPropertyRequest->Irp     = pIrp;

            //
            // Call the handler.
            //
            ntStatus =
                pPcPropertyRequest->PropertyItem->Handler
                (
                    pPcPropertyRequest
                );

            // 
            // If the STATUS_BUFFER_OVERFLOW warning code is returned even
            // though a non-null non-zero sized instance buffer was supplied, convert
            // the warning to the STATUS_BUFFER_TOO_SMALL error in order to 
            // prevent the I/O manager from attempting to copy any data.
            //
            if ((ntStatus == STATUS_BUFFER_OVERFLOW) && 
                (pPcPropertyRequest->Value != NULL) &&
                (pPcPropertyRequest->ValueSize > 0))
            {
                ntStatus = STATUS_BUFFER_TOO_SMALL;
            }

            *pulDataSize = pPcPropertyRequest->ValueSize;
        }
        else
        {
            ntStatus = STATUS_NOT_FOUND;
        }

        //
        // Delete the request structure unless we are pending.
        //
        if (ntStatus != STATUS_PENDING)
        {
            delete pPcPropertyRequest;
        }
        else
        {
            //
            // Only requests with IRPs can be pending.
            //
            ASSERT(pIrp);
        }
    }

    return ntStatus;
}

/*****************************************************************************
 * PropertyItemPropertyHandler()
 *****************************************************************************
 * KS-sytle property handler that handles all properties using the 
 * PCPROPERTY_ITEM mechanism.
 */
NTSTATUS
PropertyItemPropertyHandler
(
    IN      PIRP            pIrp,
    IN      PKSIDENTIFIER   pKsIdentifier,
    IN OUT  PVOID           pvData      OPTIONAL
)
{
    PAGED_CODE();

    ASSERT(pIrp);
    ASSERT(pKsIdentifier);

    //
    // Extract various things from the IRP and dispatch the property.
    //
    PIO_STACK_LOCATION irpSp = 
        IoGetCurrentIrpStackLocation(pIrp);

    ULONG ulDataSize =
        irpSp->Parameters.DeviceIoControl.OutputBufferLength;

    NTSTATUS ntStatus =
        PcDispatchProperty
        (   pIrp
        ,   PPROPERTY_CONTEXT(pIrp->Tail.Overlay.DriverContext[3])
        ,   KSPROPERTY_SET_IRP_STORAGE(pIrp)
        ,   irpSp->Parameters.DeviceIoControl.InputBufferLength
        ,   pKsIdentifier
        ,   &ulDataSize
        ,   pvData
        );

    //
    // Inform the caller of the resulting status and size.
    // Pending IRPs must be IoMarkIrpPending()ed before the dispatch routine
    // returns.
    //
    if ((ntStatus != STATUS_PENDING) && !NT_ERROR(ntStatus))
    {
        pIrp->IoStatus.Information = ulDataSize;
    }

    return ntStatus;
}

/*****************************************************************************
 * PcCompletePendingPropertyRequest()
 *****************************************************************************
 * Completes a pending property request.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcCompletePendingPropertyRequest
(
    __in    PPCPROPERTY_REQUEST PropertyRequest,
    __in    NTSTATUS            NtStatus
)
{
    PAGED_CODE();

    ASSERT(PropertyRequest);
    ASSERT(NtStatus != STATUS_PENDING);

    //
    // Validate Parameters.
    //
    if (NULL == PropertyRequest)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcCompletePendingPropertyRequest : Invalid Parameter."));
        return STATUS_INVALID_PARAMETER;
    }

    if (!NT_ERROR(NtStatus))
    {
        PropertyRequest->Irp->IoStatus.Information = 
            PropertyRequest->ValueSize;
    }

    PropertyRequest->Irp->IoStatus.Status = NtStatus;
    IoCompleteRequest(PropertyRequest->Irp,IO_NO_INCREMENT);

    delete PropertyRequest;

    return STATUS_SUCCESS;
}

/*****************************************************************************
 * PcFreePropertyTable()
 *****************************************************************************
 * Frees allocated memory in a PROPERTY_TABLE structure.
 */
PORTCLASSAPI
void
NTAPI
PcFreePropertyTable
(
    IN      PPROPERTY_TABLE         PropertyTable
)
{



    //  No synchronization / exclusion is used
    //  here, because this structure is created at PnP start device time, and
    //  torn down at PnP stop device time.  These can only come from the
    //  configuration manager, and it will never issue them simultaneously (it
    //  would be tough to manage any device state reliably if it did).  This
    //  was verified with the owner of that code.

    _DbgPrintF(DEBUGLVL_VERBOSE,("PcFreePropertyTable"));

    PAGED_CODE();
    
    ASSERT(PropertyTable);
    
    ASSERT((!PropertyTable->PropertySets) == (!PropertyTable->PropertySetCount));
    //  PropertySets and PropertySetCount must be non-NULL/non-zero, or NULL/zero

    ASSERT(PropertyTable->StaticSets == (!PropertyTable->StaticItems));
    //  StaticSets and StaticItems must be TRUE/NULL, or FALSE/non-null

    PBOOLEAN    staticItem  = PropertyTable->StaticItems;
    if (staticItem)
    {
        PKSPROPERTY_SET propertySet = PropertyTable->PropertySets;
        if (propertySet)
        {
            for( ULONG count = PropertyTable->PropertySetCount; 
                 count--; 
                 propertySet++, staticItem++)
            {
                if ((! *staticItem) && propertySet->PropertyItem)
                {
                    ExFreePool(PVOID(propertySet->PropertyItem));
                }
            }
        }
        ExFreePool(PropertyTable->StaticItems);
        PropertyTable->StaticItems = NULL;
    }

    if (PropertyTable->PropertySets && !PropertyTable->StaticSets)
    {
        PropertyTable->PropertySetCount = 0;
        ExFreePool(PropertyTable->PropertySets);
        PropertyTable->PropertySets = NULL;
    }
    PropertyTable->StaticSets = TRUE;
}

/*****************************************************************************
 * PcIsPropertyInPropertyTable()
 *****************************************************************************
 * Checks if a PROPERTY_ITEM is present in the specified property table.
 */

BOOLEAN
PcIsPropertyInPropertyTable
(
    _In_    PPROPERTY_TABLE         PropertyTable,
    _In_    const PCPROPERTY_ITEM * PropertyItem
)
{
    //  No synchronization / exclusion is used
    //  here, because this structure is created at PnP start device time, and
    //  torn down at PnP stop device time.  These can only come from the
    //  configuration manager, and it will never issue them simultaneously (it
    //  would be tough to manage any device state reliably if it did).  This
    //  was verified with the owner of that code.

    PAGED_CODE();

    ASSERT(PropertyTable);
    ASSERT(PropertyItem);

    BOOLEAN present = FALSE;

    _DbgPrintF(DEBUGLVL_VERBOSE,("PcIsPropertyInPropertyTable"));

    //
    // Search for the property set.
    //
    PKSPROPERTY_SET propertySet = PropertyTable->PropertySets;
    for (ULONG count = PropertyTable->PropertySetCount; count--; propertySet++)
    {
        if (IsEqualGUIDAligned(*PropertyItem->Set,*propertySet->Set))
        {
            //
            // Search for the prop in the property set.
            //
            const KSPROPERTY_ITEM* propertyItem = propertySet->PropertyItem;
            for (ULONG itemCount = propertySet->PropertiesCount; itemCount--; propertyItem++)
            {
                if (PropertyItem->Id == propertyItem->PropertyId)
                {
                    present = TRUE;
                    break;
                }
            }

            // no need to search other prop sets.
            break;
        }
    }

    return present;
}

/*****************************************************************************
 * PcAddToPropertyTable()
 *****************************************************************************
 * Adds a PROPERTY_ITEM property table to a PROPERTY_TABLE structure.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcAddToPropertyTable
(
    IN OUT  PPROPERTY_TABLE         PropertyTable,
    IN      ULONG                   PropertyItemCount,
    IN      const PCPROPERTY_ITEM * PropertyItems,
    IN      ULONG                   PropertyItemSize
)
{



    //  No synchronization / exclusion is used
    //  here, because this structure is created at PnP start device time, and
    //  torn down at PnP stop device time.  These can only come from the
    //  configuration manager, and it will never issue them simultaneously (it
    //  would be tough to manage any device state reliably if it did).  This
    //  was verified with the owner of that code.

    PAGED_CODE();

    ASSERT(PropertyTable);
    ASSERT(PropertyItems);
    ASSERT(PropertyItemSize >= sizeof(PCPROPERTY_ITEM));

    _DbgPrintF(DEBUGLVL_VERBOSE,("PcAddToEventTable"));

#define ADVANCE(item) (item = PPCPROPERTY_ITEM(PBYTE(item) + PropertyItemSize))

// Disable the prefast warning that is erroneous.
#pragma prefast(disable: __WARNING_BUFFER_OVERFLOW, "Erroneous buffer overflow declared in following code")

    ASSERT((!PropertyTable->PropertySets) == (!PropertyTable->PropertySetCount));
    //  values must be non-NULL/non-zero, or NULL/zero.
    
    //
    // Determine how many sets we will end up with.
    //
    ULONG setCount = PropertyTable->PropertySetCount;
    const PCPROPERTY_ITEM *item = PropertyItems;
    for (ULONG count = PropertyItemCount; count--; ADVANCE(item))
    {
        BOOLEAN countThis = TRUE;

        //
        // See if it's already in the table.
        //
        PKSPROPERTY_SET propertySet = PropertyTable->PropertySets;
        for 
        (   ULONG count2 = PropertyTable->PropertySetCount; 
            count2--; 
            propertySet++
        )
        {
            if (IsEqualGUIDAligned(*item->Set,*propertySet->Set))
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
                const PCPROPERTY_ITEM *prevItem = PropertyItems; 
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
    // Make a new set table
    //
    ASSERT(setCount);
    ASSERT(setCount >= PropertyTable->PropertySetCount);
    //
    // Allocate memory required for the set table.
    //
    PKSPROPERTY_SET newTable;

    if  ((setCount < PropertyTable->PropertySetCount) ||
         (ULONGLONG(sizeof(KSPROPERTY_SET) * setCount) != (ULONGLONG(sizeof(KSPROPERTY_SET)) * ULONGLONG(setCount))))
    {
        newTable = NULL;    //  Overflow errors (makes Prefast happy)
    }
    else
    {
        newTable = 
            PKSPROPERTY_SET
            (
                ExAllocatePoolWithTag
                (
                    PagedPool,
                    sizeof(KSPROPERTY_SET) * setCount,
                    'tScP'
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
            sizeof(KSPROPERTY_SET) * setCount
        );

        if (PropertyTable->PropertySetCount != 0)
        {
            RtlCopyMemory
            (
                PVOID(newTable),
                PVOID(PropertyTable->PropertySets),
                sizeof(KSPROPERTY_SET) * PropertyTable->PropertySetCount
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

        if (PropertyTable->StaticItems && PropertyTable->PropertySetCount)
        {
            //
            // Flags existed before...copy them.
            //
            RtlCopyMemory
            (
                PVOID(newStaticItems),
                PVOID(PropertyTable->StaticItems),
                sizeof(BOOLEAN) * PropertyTable->PropertySetCount
            );
        }

        //
        // Assign set GUIDs to the new set entries.
        //
        PKSPROPERTY_SET addHere = 
            newTable + PropertyTable->PropertySetCount;

        const PCPROPERTY_ITEM *item2 = PropertyItems;
        for (ULONG count = PropertyItemCount; count--; ADVANCE(item2))
        {
            BOOLEAN addThis = TRUE;

            //
            // See if it's already in the table.
            //
            for( PKSPROPERTY_SET propertySet = newTable;
                 propertySet != addHere;
                 propertySet++)
            {
                if (IsEqualGUIDAligned(*item2->Set,*propertySet->Set))
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
        if (PropertyTable->PropertySets && (!PropertyTable->StaticSets))
        {
            ExFreePool(PropertyTable->PropertySets);
        }
        if (PropertyTable->StaticItems)
        {
            ExFreePool(PropertyTable->StaticItems);
        }

        //
        // Install the new tables.
        //
        PropertyTable->PropertySetCount = setCount;
        PropertyTable->PropertySets     = newTable;
        PropertyTable->StaticSets       = FALSE;
        PropertyTable->StaticItems      = newStaticItems;
    }
    else
    {
        //  if allocations fail, return error and 
        //  keep sets and items as they were.
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Now we have a property set table that contains all the sets we need.
    //
    if (NT_SUCCESS(ntStatus))
    {
        //
        // For each set...
        //
        PKSPROPERTY_SET propertySet = PropertyTable->PropertySets;
        PBOOLEAN        staticItem  = PropertyTable->StaticItems;
        for 
        (   ULONG count = PropertyTable->PropertySetCount; 
            count--; 
            propertySet++, staticItem++
        )
        {
            //
            // Check to see how many new items we have.
            //
            ULONG itemCount = propertySet->PropertiesCount;
            const PCPROPERTY_ITEM *item2 = PropertyItems;
            for (ULONG count2 = PropertyItemCount; count2--; ADVANCE(item2))
            {
                if (IsEqualGUIDAligned(*item2->Set,*propertySet->Set))
                {
                    itemCount++;
                }
            }

            ASSERT(itemCount >= propertySet->PropertiesCount);
            if (itemCount != propertySet->PropertiesCount)
            {
                //
                // Allocate memory required for the items table.
                //
                PKSPROPERTY_ITEM newTable2;

                if  ((itemCount < propertySet->PropertiesCount) ||
                     (ULONGLONG(sizeof(KSPROPERTY_ITEM) * itemCount) != (ULONGLONG(sizeof(KSPROPERTY_ITEM)) * ULONGLONG(itemCount))))
                {
                    //  Keep Prefast happy- this is an overflow!
                    newTable2 = NULL;
                }
                else
                {
                    newTable2 = 
                        PKSPROPERTY_ITEM
                        (
                            ExAllocatePoolWithTag
                            (
                                PagedPool,
                                sizeof(KSPROPERTY_ITEM) * itemCount,
                                'iScP'
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
                    sizeof(KSPROPERTY_ITEM) * itemCount
                );

                if (propertySet->PropertiesCount)
                {
                    RtlCopyMemory
                    (
                        PVOID(newTable2),
                        PVOID(propertySet->PropertyItem),
                        sizeof(KSPROPERTY_ITEM) * propertySet->PropertiesCount
                    );
                }

                //
                // Create the new items.
                //
                PKSPROPERTY_ITEM addHere = 
                    newTable2 + propertySet->PropertiesCount;

                item2 = PropertyItems;
                for (ULONG count2 = PropertyItemCount; count2--; ADVANCE(item2))
                {
                    if (IsEqualGUIDAligned(*item2->Set,*propertySet->Set))
                    {
                        addHere->PropertyId         = item2->Id;
                        addHere->GetPropertyHandler = 
                            (   (item2->Flags & PCPROPERTY_ITEM_FLAG_GET) 
                            ?   PropertyItemPropertyHandler 
                            :   NULL
                            );
                        addHere->MinProperty        = sizeof(KSPROPERTY);
                        addHere->MinData            = 0;
                        addHere->SetPropertyHandler = 
                            (   (item2->Flags & PCPROPERTY_ITEM_FLAG_SET) 
                            ?   PropertyItemPropertyHandler 
                            :   NULL
                            );
                        addHere->Values             = NULL;
                        addHere->RelationsCount     = 0;
                        addHere->Relations          = PKSPROPERTY(item2);  
                        addHere->SupportHandler     = 
                            (   (item2->Flags & PCPROPERTY_ITEM_FLAG_BASICSUPPORT) 
                            ?   PropertyItemPropertyHandler 
                            :   NULL
                            );
                        addHere->SerializedSize     = 
                            (   (item2->Flags & PCPROPERTY_ITEM_FLAG_SERIALIZE) 
                            ?   ULONG(-1) 
                            :   0
                            );
                        AddValidation(*propertySet->Set, addHere++);
                    }
                }

                ASSERT(addHere == newTable2 + itemCount);

                //
                // Free old allocated table.
                //
                if (propertySet->PropertyItem && ! *staticItem)
                {
                    ExFreePool(PVOID(propertySet->PropertyItem));
                }

                //
                // Install the new tables.
                //
                propertySet->PropertiesCount = itemCount;
                propertySet->PropertyItem    = newTable2;
                *staticItem = FALSE;
            }
        }
    }

// Enable prefast warning back
#pragma prefast(enable: __WARNING_BUFFER_OVERFLOW, "Erroneous buffer overflow declared in previous code")

    return ntStatus;
}

/*****************************************************************************
 * GenerateFormatFromRange()
 *****************************************************************************
 * Determine the particular format, based on the intersection of these 
 * two specific data ranges.  First ask the miniport, then fall back to
 * our own algorithm if the miniport doesn't handle it.
 */
NTSTATUS 
GenerateFormatFromRange (
    IN PIRP Irp,
    IN ULONG PinId,
    IN PKSDATARANGE MyDataRange,
    IN PKSDATARANGE ClientDataRange,
    IN ULONG OutputBufferLength,
    OUT PVOID ResultantFormat,
    OUT PULONG ResultantFormatLength)
{
    BOOLEAN                         bSpecifier;
    NTSTATUS                        Status;
    ULONG                           RequiredSize;

    PAGED_CODE();
    
    PPROPERTY_CONTEXT pPropertyContext = PPROPERTY_CONTEXT(Irp->Tail.Overlay.DriverContext[3]);
    ASSERT(pPropertyContext);

    PSUBDEVICE pSubdevice = pPropertyContext->pSubdevice;
    ASSERT(pSubdevice);

    //
    // Give the miniport a chance to fill in the format structure
    //    
    Status = pSubdevice->DataRangeIntersection (PinId,
                                                ClientDataRange,
                                                MyDataRange,
                                                OutputBufferLength,
                                                ResultantFormat,
                                                ResultantFormatLength);
    
    //
    // return if the miniport filled out the structure.
    //
    if (Status != STATUS_NOT_IMPLEMENTED)
    {
        return Status;
    }

    //
    // In case the miniport didn't implement the DataRangeIntersection,
    // we provide a default handler here.
    //
    
    //
    // Check if there is a wildcard somewhere.
    //
    if (IsEqualGUIDAligned (ClientDataRange->MajorFormat, KSDATAFORMAT_TYPE_WILDCARD) ||
        IsEqualGUIDAligned (ClientDataRange->SubFormat, KSDATAFORMAT_SUBTYPE_WILDCARD) ||
        IsEqualGUIDAligned (ClientDataRange->Specifier, KSDATAFORMAT_SPECIFIER_WILDCARD))
    {
        // If the miniport exposed a WILDCARD, then it must implement an 
        // intersection handler, because it must provide the specific 
        // ideal matching data range for that WILDCARD.
        //
        return STATUS_NO_MATCH;
    }

    if (!IsEqualGUIDAligned (ClientDataRange->Specifier, KSDATAFORMAT_SPECIFIER_NONE))
    {
        //
        // The miniport did not resolve this format. Let's handle the specifiers
        // that we know.
        //
        if (!IsEqualGUIDAligned (ClientDataRange->Specifier, KSDATAFORMAT_SPECIFIER_DSOUND) &&
            !IsEqualGUIDAligned (ClientDataRange->Specifier, KSDATAFORMAT_SPECIFIER_WAVEFORMATEX))
        {
            return STATUS_NO_MATCH;
        }
        
        bSpecifier = TRUE;
        
        //
        // The specifier here defines the format of ClientDataRange
        // and the format expected to be returned in ResultantFormat.
        //
        if (IsEqualGUIDAligned (ClientDataRange->Specifier, KSDATAFORMAT_SPECIFIER_DSOUND))
        {
            RequiredSize = sizeof (KSDATAFORMAT_DSOUND);
        } 
        else 
        {
            RequiredSize = sizeof (KSDATAFORMAT_WAVEFORMATEX);
        }            
    } 
    else 
    {
        bSpecifier = FALSE;
        RequiredSize = sizeof (KSDATAFORMAT);
    }
            
    //
    // Validate return buffer size, if the request is only for the
    // size of the resultant structure, return it now.
    //
    if (!OutputBufferLength) 
    {
        *ResultantFormatLength = RequiredSize;
        return STATUS_BUFFER_OVERFLOW;
    } 
    else if (OutputBufferLength < RequiredSize) 
    {
        return STATUS_BUFFER_TOO_SMALL;
    }
    
    // There was a specifier ...
    if (bSpecifier) 
    {     
        PKSDATARANGE_AUDIO  myAudioRange,clientAudioRange;
        PKSDATAFORMAT       resultantFormat;
        PWAVEFORMATEX       resultantWaveFormatEx;
        
        myAudioRange = (PKSDATARANGE_AUDIO) MyDataRange;
        clientAudioRange = (PKSDATARANGE_AUDIO) ClientDataRange;
        resultantFormat = (PKSDATAFORMAT)ResultantFormat;
        
        //
        // Fill out the dataformat and other general fields.
        //
        *resultantFormat = *ClientDataRange;
        resultantFormat->FormatSize = RequiredSize;
        *ResultantFormatLength = RequiredSize;
        
        //
        // Fill out the DSOUND specific structure
        //
        if (IsEqualGUIDAligned (ClientDataRange->Specifier, KSDATAFORMAT_SPECIFIER_DSOUND)) 
        {
            PKSDATAFORMAT_DSOUND    resultantDSoundFormat;
            
            resultantDSoundFormat = (PKSDATAFORMAT_DSOUND)ResultantFormat;
            
            _DbgPrintF (DEBUGLVL_VERBOSE, ("returning KSDATAFORMAT_DSOUND format intersection"));
            
            //
            // DSound format capabilities are not expressed 
            // this way in KS, so we express no capabilities. 
            //
            resultantDSoundFormat->BufferDesc.Flags = 0 ;
            resultantDSoundFormat->BufferDesc.Control = 0 ;
            resultantWaveFormatEx = &resultantDSoundFormat->BufferDesc.WaveFormatEx;
        }
        else 
        {
            _DbgPrintF (DEBUGLVL_VERBOSE, ("returning KSDATAFORMAT_WAVEFORMATEX format intersection"));
        
            resultantWaveFormatEx = (PWAVEFORMATEX)((PKSDATAFORMAT)ResultantFormat + 1);
        }
        
        //
        // Return a format that intersects the given audio range, 
        // using our maximum support as the "best" format.
        // 
        resultantWaveFormatEx->wFormatTag = WAVE_FORMAT_PCM;
        
        resultantWaveFormatEx->nChannels = (USHORT) min (
                        myAudioRange->MaximumChannels,clientAudioRange->MaximumChannels);
        
        resultantWaveFormatEx->nSamplesPerSec = min (
                        myAudioRange->MaximumSampleFrequency,clientAudioRange->MaximumSampleFrequency);
        
        resultantWaveFormatEx->wBitsPerSample = (USHORT) min (
                        myAudioRange->MaximumBitsPerSample,clientAudioRange->MaximumBitsPerSample);
        
        resultantWaveFormatEx->nBlockAlign = (resultantWaveFormatEx->wBitsPerSample * resultantWaveFormatEx->nChannels) / 8;
        resultantWaveFormatEx->nAvgBytesPerSec = (resultantWaveFormatEx->nSamplesPerSec * resultantWaveFormatEx->nBlockAlign);
        resultantWaveFormatEx->cbSize = 0;
        resultantFormat->Flags &= (~KSDATAFORMAT_ATTRIBUTES);
        resultantFormat->SampleSize = resultantWaveFormatEx->nBlockAlign;

        _DbgPrintF (DEBUGLVL_VERBOSE, ("Channels = %d",    resultantWaveFormatEx->nChannels));
        _DbgPrintF (DEBUGLVL_VERBOSE, ("Samples/sec = %d", resultantWaveFormatEx->nSamplesPerSec));
        _DbgPrintF (DEBUGLVL_VERBOSE, ("Bits/sample = %d", resultantWaveFormatEx->wBitsPerSample));
    } 
    else 
    {   // There was no specifier. Return only the KSDATAFORMAT structure.
        //
        // Copy the data format structure.
        //
        _DbgPrintF (DEBUGLVL_VERBOSE, ("returning default format intersection"));
        RtlCopyMemory (ResultantFormat, ClientDataRange, sizeof (KSDATAFORMAT));
        *ResultantFormatLength = sizeof (KSDATAFORMAT);
        ((PKSDATAFORMAT)ResultantFormat)->Flags &= (~KSDATAFORMAT_ATTRIBUTES);
    }
    
    return STATUS_SUCCESS;
}

/*****************************************************************************
 * ValidateTypeAndSpecifier()
 *****************************************************************************
 * Find the data range that best matches the client's data range, given our
 * entire list of data ranges.  This might include wildcard-based ranges.
 *
 */
NTSTATUS
ValidateTypeAndSpecifier(
    IN PIRP Irp,
    IN ULONG PinId,
    IN PKSDATARANGE ClientDataRange,
    IN ULONG MyDataRangesCount,
    IN const PKSDATARANGE * MyDataRanges,
    IN ULONG OutputBufferLength,
    OUT PVOID ResultantFormat,
    OUT PULONG ResultantFormatLength
    )
{
    NTSTATUS          ntStatus;
    PKSDATARANGE      aClientDataRange;
    KSDATARANGE_AUDIO dr;

    PAGED_CODE();

    //
    // Check the size of the structure.
    //
    if (ClientDataRange->FormatSize < sizeof (KSDATARANGE))
    {
        _DbgPrintF (DEBUGLVL_TERSE, ("Validating ClientDataRange: size < KSDATARANGE!"));
        return STATUS_INVALID_PARAMETER;
    }
    
    //
    // We default to no match.
    //
    ntStatus = STATUS_NO_MATCH;

    //
    // Go through all the dataranges in the validate list until we get a SUCCESS.
    //
    for (; MyDataRangesCount--; MyDataRanges++)
    {
        PKSDATARANGE myDataRange = *MyDataRanges;

        //
        // Verify that the correct major format, subformat and specifier (or wildcards)
        // are in the intersection.
        //
        
        if ((!IsEqualGUIDAligned(ClientDataRange->MajorFormat,myDataRange->MajorFormat) &&
             !IsEqualGUIDAligned(ClientDataRange->MajorFormat,KSDATAFORMAT_TYPE_WILDCARD)) ||
            (!IsEqualGUIDAligned(ClientDataRange->SubFormat,myDataRange->SubFormat) &&
             !IsEqualGUIDAligned(ClientDataRange->SubFormat,KSDATAFORMAT_SUBTYPE_WILDCARD)) || 
            (!IsEqualGUIDAligned(ClientDataRange->Specifier,myDataRange->Specifier) &&
             !IsEqualGUIDAligned(ClientDataRange->Specifier,KSDATAFORMAT_SPECIFIER_WILDCARD)))
        {
            continue;   //  no match and no WILDCARD, try the next one
        }
        
        //
        //  if not WILDCARD, then we ask the miniport to match this one exactly,
        //  else we manufacture a range and ask the miniport for a match from that.
        //
        aClientDataRange = ClientDataRange;  //  assume no WILDCARD for now, we ask the miniport to match this
        
        //
        // Handle the wildcards.
        //
        if (IsEqualGUIDAligned (ClientDataRange->MajorFormat,KSDATAFORMAT_TYPE_WILDCARD) ||
            IsEqualGUIDAligned (ClientDataRange->SubFormat,  KSDATAFORMAT_SUBTYPE_WILDCARD) ||
            IsEqualGUIDAligned (ClientDataRange->Specifier,  KSDATAFORMAT_SPECIFIER_WILDCARD))
        {
            //
            // We pass a faked datarange for the specifiers we know or we pass to the
            // miniport it's own datarange.
            //
            // We know the specifiers waveformatex and dsound.
            //
            if (IsEqualGUIDAligned (myDataRange->Specifier, KSDATAFORMAT_SPECIFIER_WAVEFORMATEX) ||
                IsEqualGUIDAligned (myDataRange->Specifier, KSDATAFORMAT_SPECIFIER_DSOUND))
            {
                //
                // Take the complete datarange from the driver.
                //
                dr.DataRange = *myDataRange;
                
                //
                // Fill in a HUGE data range (it asked for WILDCARD, after all!)
                //
                dr.MaximumChannels = 0x1FFF0;
                dr.MinimumBitsPerSample = 1;
                dr.MaximumBitsPerSample = 0x1FFF0;
                dr.MinimumSampleFrequency = 1;
                dr.MaximumSampleFrequency = 0x1FFFFFF0;
                
                aClientDataRange = (PKSDATARANGE)&dr;
            }
            else
            {
                //
                // We don't know this non-wave format (in the list of formats we supposedly support).
                // The miniport specified this format, so it is OK to pass it down.
                //
                aClientDataRange = myDataRange;
            }
        }

        //
        // Make sure KSDATARANGE_AUDIO is used, then see if there is a possible match.
        //
        if (IsEqualGUIDAligned (aClientDataRange->Specifier, KSDATAFORMAT_SPECIFIER_WAVEFORMATEX) ||
            IsEqualGUIDAligned (aClientDataRange->Specifier, KSDATAFORMAT_SPECIFIER_DSOUND))
        {
            if (aClientDataRange->FormatSize < sizeof (KSDATARANGE_AUDIO))
            {
                //
                // The datarange structure passed has no KSDATARANGE_AUDIO.
                //
                _DbgPrintF (DEBUGLVL_TERSE, ("Validating PCM ValidDataRange: size < KSDATARANGE_AUDIO!"));
                continue;   // not large enough, try the next one
            }

            //
            // Verify that we have an intersection with the specified format and 
            // our audio format dictated by our specific requirements.
            //
            _DbgPrintF (DEBUGLVL_VERBOSE, ("validating KSDATARANGE_AUDIO"));

            if ((((PKSDATARANGE_AUDIO)aClientDataRange)->MinimumSampleFrequency >
                 ((PKSDATARANGE_AUDIO)myDataRange)->MaximumSampleFrequency) ||
                (((PKSDATARANGE_AUDIO)aClientDataRange)->MaximumSampleFrequency <
                 ((PKSDATARANGE_AUDIO)myDataRange)->MinimumSampleFrequency) ||
                (((PKSDATARANGE_AUDIO)aClientDataRange)->MinimumBitsPerSample >
                 ((PKSDATARANGE_AUDIO)myDataRange)->MaximumBitsPerSample) ||
                (((PKSDATARANGE_AUDIO)aClientDataRange)->MaximumBitsPerSample <
                 ((PKSDATARANGE_AUDIO)myDataRange)->MinimumBitsPerSample))
            {
                continue;
            }
        }
        
        ntStatus = GenerateFormatFromRange (Irp, 
                                            PinId, 
                                            myDataRange, 
                                            aClientDataRange,
                                            OutputBufferLength,
                                            ResultantFormat,
                                            ResultantFormatLength);
        if ( NT_SUCCESS(ntStatus) 
          || (ntStatus == STATUS_BUFFER_OVERFLOW) 
          || (ntStatus == STATUS_BUFFER_TOO_SMALL)) 
        {
            break;  //  We found a good one, or we failed.
                    //  Either way we must leave.
        }
    }
    
    return ntStatus;
}

/*****************************************************************************
 * PinIntersectHandler()
 *****************************************************************************
 * This function is a data range callback for use with 
 * PropertyHandler_PinIntersection
 */
NTSTATUS
PinIntersectHandler
(
    IN      PIRP            Irp,
    IN      PKSP_PIN        Pin,
    IN      PKSDATARANGE    DataRange,
    OUT     PVOID           Data
)
{
    NTSTATUS    Status;
    ULONG       OutputBufferLength;
    
    PAGED_CODE();

    ASSERT(Irp);
    ASSERT(Pin);
    ASSERT(DataRange);

    PPROPERTY_CONTEXT pPropertyContext =
        PPROPERTY_CONTEXT(Irp->Tail.Overlay.DriverContext[3]);
    ASSERT(pPropertyContext);

    PSUBDEVICE_DESCRIPTOR pSubdeviceDescriptor =
        pPropertyContext->pSubdeviceDescriptor;
    ASSERT(pSubdeviceDescriptor);
    ASSERT(pSubdeviceDescriptor->PinDescriptors);
    ASSERT(Pin->PinId < pSubdeviceDescriptor->PinCount);

    PKSPIN_DESCRIPTOR pinDescriptor =
        &pSubdeviceDescriptor->PinDescriptors[Pin->PinId];

    ASSERT(pinDescriptor);

    _DbgPrintF(DEBUGLVL_VERBOSE,("[PinIntersectHandler]"));
    
    OutputBufferLength = 
        IoGetCurrentIrpStackLocation( Irp )->
            Parameters.DeviceIoControl.OutputBufferLength;

    Status = 
        ValidateTypeAndSpecifier( 
            Irp,
            Pin->PinId,
            DataRange,
            pinDescriptor->DataRangesCount,
            pinDescriptor->DataRanges,
            OutputBufferLength,
            Data,
            PULONG(&Irp->IoStatus.Information) );
    
    if (!NT_SUCCESS( Status )) {
        _DbgPrintF( 
            DEBUGLVL_VERBOSE, 
            ("ValidateTypeAndSpecifier() returned %08x", Status) );
    }
    
    return Status;
}

/*****************************************************************************
 * PinPhysicalConnection()
 *****************************************************************************
 * Handles physical connection property access for the pin.
 */
static
NTSTATUS
PinPhysicalConnection
(
    IN      PIRP            Irp,
    IN      PKSP_PIN        Pin,
    OUT     PVOID           Data
)
{
    PAGED_CODE();

    ASSERT(Irp);
    ASSERT(Pin);

    PPROPERTY_CONTEXT pPropertyContext =
        PPROPERTY_CONTEXT(Irp->Tail.Overlay.DriverContext[3]);
    ASSERT(pPropertyContext);

    PSUBDEVICE Subdevice =
        pPropertyContext->pSubdevice;
    ASSERT(Subdevice);

    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp);
    ASSERT(irpSp);
    ASSERT(irpSp->DeviceObject);

    PDEVICE_CONTEXT deviceContext = 
        PDEVICE_CONTEXT(irpSp->DeviceObject->DeviceExtension);

    NTSTATUS        ntStatus = STATUS_NOT_FOUND;
    ULONG           outPin = 0;
    PUNICODE_STRING outUnicodeString = NULL;

    KeWaitForSingleObject(&deviceContext->ListProtect,Executive,KernelMode,FALSE,NULL);
    for
    (
        PLIST_ENTRY entry = deviceContext->PhysicalConnectionList.Flink;
        entry != &deviceContext->PhysicalConnectionList;
        entry = entry->Flink
    )
    {
        PPHYSICALCONNECTION connection = PPHYSICALCONNECTION(entry);

        if  (   (connection->FromSubdevice == Subdevice)
            &&  (connection->FromPin == Pin->PinId)
            )
        {
            outPin = connection->ToPin;

            if (connection->ToString)
            {
                outUnicodeString = connection->ToString;
            }
            else
            {
                ULONG ulIndex = 
                    SubdeviceIndex(irpSp->DeviceObject,connection->ToSubdevice);

                if (ulIndex != ULONG(-1))
                {
                    ntStatus = STATUS_SUCCESS;
                    outUnicodeString = &deviceContext->SymbolicLinkNames[ulIndex];
                }
            }
            break;
        }
        else
        if  (   (connection->ToSubdevice == Subdevice)
            &&  (connection->ToPin == Pin->PinId)
            )
        {
            outPin = connection->FromPin;

            if (connection->FromString)
            {
                outUnicodeString = connection->FromString;
            }
            else
            {
                ULONG ulIndex = 
                    SubdeviceIndex(irpSp->DeviceObject,connection->FromSubdevice);

                if (ulIndex != ULONG(-1))
                {
                    ntStatus = STATUS_SUCCESS;
                    outUnicodeString = &deviceContext->SymbolicLinkNames[ulIndex];
                }
            }
            break;
        }
    }
    KeReleaseMutex(&deviceContext->ListProtect,FALSE);

    if (!outUnicodeString)
    {
        ntStatus = STATUS_NOT_FOUND;
    }

    if (NT_SUCCESS(ntStatus))
    {
        ULONG outSize;
        outSize = FIELD_OFFSET(KSPIN_PHYSICALCONNECTION,SymbolicLinkName[0]);
        outSize += (outUnicodeString->Length + sizeof(UNICODE_NULL));

        //
        // Validate return buffer size.
        //
        ULONG outputBufferLength =
            IoGetCurrentIrpStackLocation(Irp)->
                Parameters.DeviceIoControl.OutputBufferLength;

        if (!outputBufferLength)
        {
            Irp->IoStatus.Information = outSize;
            ntStatus = STATUS_BUFFER_OVERFLOW;
        }
        else
        if (outputBufferLength < outSize)
        {
            ntStatus = STATUS_BUFFER_TOO_SMALL;
        }
        else
        {
            PKSPIN_PHYSICALCONNECTION out = PKSPIN_PHYSICALCONNECTION(Data);

            out->Size = outSize;
            out->Pin  = outPin;
            RtlCopyMemory
            (
                out->SymbolicLinkName,
                outUnicodeString->Buffer,
                outUnicodeString->Length
            );
            out->SymbolicLinkName[outUnicodeString->Length / sizeof(WCHAR)] = 0;
            Irp->IoStatus.Information = outSize;
        }
    }

    return ntStatus;
}

/*****************************************************************************
 * PinCountHandler()
 *****************************************************************************
 * Handles pin count access for the pin.
 */
void PinCountHandler
(   IN      PPROPERTY_CONTEXT   pPropertyContext,
    IN      ULONG               pinId
)
{
    PAGED_CODE();

    ASSERT(pPropertyContext);

    PSUBDEVICE_DESCRIPTOR pSubdeviceDescriptor = pPropertyContext->pSubdeviceDescriptor;
    ASSERT(pSubdeviceDescriptor);

    PSUBDEVICE Subdevice = pPropertyContext->pSubdevice;
    ASSERT(Subdevice);

    Subdevice->PinCount( pinId, &(pSubdeviceDescriptor->PinInstances[pinId].FilterNecessary),
                                &(pPropertyContext->pulPinInstanceCounts[pinId]),
                                &(pSubdeviceDescriptor->PinInstances[pinId].FilterPossible),
                                &(pSubdeviceDescriptor->PinInstances[pinId].GlobalCurrent),
                                &(pSubdeviceDescriptor->PinInstances[pinId].GlobalPossible) );
}

/*****************************************************************************
 * PinNameHandlerBasicSupport()
 *****************************************************************************
 * Handles pin name basic support query for the pin.
 */
NTSTATUS PinNameHandlerBasicSupport(IN PIRP pIrp,
                                    IN PKSP_PIN pKsPPin,
                                    IN PKSPROPERTY Property,
                                    IN OUT PVOID pvData,
                                    IN ULONG DescriptorsCount,
                                    IN const KSPIN_DESCRIPTOR* Descriptor)
{
    NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;

    PAGED_CODE();

    ASSERT(pIrp);
    ASSERT(pKsPPin);

    PPROPERTY_CONTEXT pPropertyContext =
        PPROPERTY_CONTEXT(pIrp->Tail.Overlay.DriverContext[3]);
    ASSERT(pPropertyContext);

    PSUBDEVICE pSubdevice = pPropertyContext->pSubdevice;
    ASSERT(pSubdevice);

    ULONG ulBufferLen = IoGetCurrentIrpStackLocation(pIrp)->Parameters.DeviceIoControl.OutputBufferLength;

    if(!ulBufferLen)
    {
        pIrp->IoStatus.Information = sizeof(ULONG);
        ntStatus = STATUS_BUFFER_OVERFLOW;
    }
    else if(ulBufferLen < sizeof(ULONG))
    {
        pIrp->IoStatus.Information = 0;
        ntStatus = STATUS_BUFFER_TOO_SMALL;
    }
    else
    {
        pIrp->IoStatus.Information = sizeof(ULONG);
        PKSPROPERTY_DESCRIPTION pPropDesc = (PKSPROPERTY_DESCRIPTION)pvData;
        pPropDesc->AccessFlags = KSPROPERTY_TYPE_BASICSUPPORT;

        if (pKsPPin->PinId >= DescriptorsCount)
        {
            ntStatus = STATUS_INVALID_PARAMETER;
        }
        else
        {
            const KSPIN_DESCRIPTOR *pPinDescriptor = (const KSPIN_DESCRIPTOR *)(((PUCHAR)Descriptor) + 
                sizeof(*Descriptor) * pKsPPin->PinId);

            if (pPinDescriptor->Name != NULL ||
               (pPinDescriptor->Category != NULL && 
                    !IsEqualGUIDAligned(KSCATEGORY_AUDIO, *pPinDescriptor->Category) && 
                    !IsEqualGUIDAligned(PINNAME_CAPTURE, *pPinDescriptor->Category)) ||
               pSubdevice->IsPinNameSupported())
            {
                pPropDesc->AccessFlags |= KSPROPERTY_TYPE_GET;
            }

            ntStatus = STATUS_SUCCESS;
        }
    }

    return ntStatus;
}

/*****************************************************************************
 * PinNameHandler()
 *****************************************************************************
 * Handles pin name property query for the pin.
 */
NTSTATUS PinNameHandler(IN PIRP pIrp,
                        IN PKSP_PIN pKsPPin,
                        IN PKSPROPERTY Property,
                        IN OUT PVOID pvData,
                        IN ULONG DescriptorsCount,
                        IN const KSPIN_DESCRIPTOR* Descriptor)
{
    NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;

    PAGED_CODE();

    ASSERT(pIrp);
    ASSERT(pKsPPin);

    if (KSPROPERTY_TYPE_BASICSUPPORT & Property->Flags)
    {
        return PinNameHandlerBasicSupport(pIrp, pKsPPin, Property, pvData, DescriptorsCount, Descriptor);
    }

    PPROPERTY_CONTEXT pPropertyContext =
        PPROPERTY_CONTEXT(pIrp->Tail.Overlay.DriverContext[3]);
    ASSERT(pPropertyContext);

    PSUBDEVICE_DESCRIPTOR pSubdeviceDescriptor =
        pPropertyContext->pSubdeviceDescriptor;
    ASSERT(pSubdeviceDescriptor);

    PSUBDEVICE pSubdevice = pPropertyContext->pSubdevice;
    ASSERT(pSubdevice);

    ntStatus = pSubdevice->GetPinName(  pIrp, 
                                        pKsPPin,
                                        pvData);
    if (STATUS_NOT_SUPPORTED == ntStatus)
    {
        ntStatus = KsPinPropertyHandler(pIrp,
                                        PKSPROPERTY(pKsPPin),
                                        pvData,
                                        pSubdeviceDescriptor->PinCount,
                                        pSubdeviceDescriptor->PinDescriptors);
        if (!NT_SUCCESS(ntStatus) &&
            STATUS_BUFFER_OVERFLOW != ntStatus &&
            STATUS_BUFFER_TOO_SMALL != ntStatus)
        {
            ntStatus = STATUS_UNSUCCESSFUL;
        }
    }
    return ntStatus;
}
/*****************************************************************************
 * PcPinPropertyHandler()
 *****************************************************************************
 * Property handler for pin properties on the filter.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcPinPropertyHandler
(   IN      PIRP                    pIrp,
    IN      PKSP_PIN                pKsPPin,
    IN OUT  PVOID                   pvData
)
{
    PAGED_CODE();

    ASSERT(pIrp);
    ASSERT(pKsPPin);

    PPROPERTY_CONTEXT pPropertyContext =
        PPROPERTY_CONTEXT(pIrp->Tail.Overlay.DriverContext[3]);
    ASSERT(pPropertyContext);

    PSUBDEVICE_DESCRIPTOR pSubdeviceDescriptor =
        pPropertyContext->pSubdeviceDescriptor;
    ASSERT(pSubdeviceDescriptor);

    NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;

    if 
    (   (pKsPPin->Property.Id != KSPROPERTY_PIN_CTYPES)
    &&  (pKsPPin->PinId >= pSubdeviceDescriptor->PinCount)
    )
    {
        ntStatus = STATUS_INVALID_PARAMETER;
    }
    else
    {
        switch (pKsPPin->Property.Id)
        {
        case KSPROPERTY_PIN_CTYPES:
        case KSPROPERTY_PIN_DATAFLOW:
        case KSPROPERTY_PIN_DATARANGES:
        case KSPROPERTY_PIN_INTERFACES:
        case KSPROPERTY_PIN_MEDIUMS:
        case KSPROPERTY_PIN_COMMUNICATION:
        case KSPROPERTY_PIN_CATEGORY:
            ntStatus =
                KsPinPropertyHandler
                (
                    pIrp,
                    PKSPROPERTY(pKsPPin),
                    pvData,
                    pSubdeviceDescriptor->PinCount,
                    pSubdeviceDescriptor->PinDescriptors
                );
            break;
        case KSPROPERTY_PIN_NAME:
            ntStatus =
                PinNameHandler
                (
                    pIrp,
                    pKsPPin,
                    PKSPROPERTY(pKsPPin),
                    pvData,
                    pSubdeviceDescriptor->PinCount,
                    pSubdeviceDescriptor->PinDescriptors
                );
            break;
        case KSPROPERTY_PIN_DATAINTERSECTION:
            ntStatus =
                KsPinDataIntersection
                ( 
                    pIrp,
                    pKsPPin,
                    pvData,
                    pSubdeviceDescriptor->PinCount,
                    pSubdeviceDescriptor->PinDescriptors,
                    PinIntersectHandler 
                );
            break;

        case KSPROPERTY_PIN_CINSTANCES:
            if (pPropertyContext->pulPinInstanceCounts)
            {
                PinCountHandler(pPropertyContext,pKsPPin->PinId);

                PKSPIN_CINSTANCES(pvData)->PossibleCount = 
                    pSubdeviceDescriptor->PinInstances[pKsPPin->PinId].FilterPossible;

                PKSPIN_CINSTANCES(pvData)->CurrentCount = 
                    pPropertyContext->pulPinInstanceCounts[pKsPPin->PinId];

                pIrp->IoStatus.Information = sizeof(KSPIN_CINSTANCES);

                ntStatus = STATUS_SUCCESS;
            }
            break;

        case KSPROPERTY_PIN_GLOBALCINSTANCES:
            if (pPropertyContext->pulPinInstanceCounts)
            {
                PinCountHandler(pPropertyContext,pKsPPin->PinId);
            }
            
            PKSPIN_CINSTANCES(pvData)->PossibleCount = 
                pSubdeviceDescriptor->PinInstances[pKsPPin->PinId].GlobalPossible;

            PKSPIN_CINSTANCES(pvData)->CurrentCount = 
                pSubdeviceDescriptor->PinInstances[pKsPPin->PinId].GlobalCurrent;

            pIrp->IoStatus.Information = sizeof(KSPIN_CINSTANCES);

            ntStatus = STATUS_SUCCESS;
            break;

        case KSPROPERTY_PIN_NECESSARYINSTANCES:
            if (pPropertyContext->pulPinInstanceCounts)
            {
                PinCountHandler(pPropertyContext,pKsPPin->PinId);

                *PULONG(pvData) = pSubdeviceDescriptor->PinInstances[pKsPPin->PinId].FilterNecessary;

                pIrp->IoStatus.Information = sizeof(ULONG);

                ntStatus = STATUS_SUCCESS;
            }
            break;

        case KSPROPERTY_PIN_PHYSICALCONNECTION:
            ntStatus =
                PinPhysicalConnection
                (
                    pIrp,
                    pKsPPin,
                    pvData
                );
            break;

        default:
            ntStatus = STATUS_NOT_FOUND;
            break;
        }
    }

    return ntStatus;
}
