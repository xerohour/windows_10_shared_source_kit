/*****************************************************************************
 * porthelp.cpp - WDM Streaming port class driver port helper functions
 *****************************************************************************
 * Copyright (c) Microsoft.  All rights reserved.
 */

#include "private.h"




/*****************************************************************************
 * Functions.
 */

#pragma code_seg("PAGE")

static
KSPIN_MEDIUM PinMediums[] =
{
   {
      STATICGUIDOF(KSMEDIUMSETID_Standard),
      KSMEDIUM_STANDARD_DEVIO,
      0
   }
};


#define UPTOQUAD(x) (((x)+7)&~7)

/*****************************************************************************
 * PrivateHeap
 *****************************************************************************
 * Class for managing a private heap.
 */
class PrivateHeap
{
private:
    __field_bcount_opt(m_ulSize) PBYTE   m_pbTop;
    PBYTE   m_pbCurrent;
    ULONG   m_ulSize;

public:
    PrivateHeap(void) : m_pbTop(NULL),
                        m_pbCurrent(NULL),
                        m_ulSize(NULL)
    {
        PAGED_CODE();
    }

    //
    // Increase the number of bytes that will be allocated for the heap.
    //
    ULONG Reserve(ULONG ulBytes)
    {
        PAGED_CODE();

        ASSERT(! m_pbTop);
        ASSERT(! m_pbCurrent);

        m_ulSize += UPTOQUAD(ulBytes);

        return m_ulSize;
    }

    //
    // Allocate memory for the private heap from a pool.
    //
    NTSTATUS AllocateFromPool(POOL_TYPE poolType,ULONG ulTag)
    {
        PAGED_CODE();

        ASSERT(! m_pbTop);
        ASSERT(! m_pbCurrent);
        ASSERT(m_ulSize);

        m_pbTop = new(poolType,ulTag) BYTE[m_ulSize];

        if (! m_pbTop)
        {
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        m_pbCurrent = m_pbTop;

        return STATUS_SUCCESS;
    }

    //
    // Allocate memory from the heap.
    //
    PVOID Alloc(ULONG ulSize)
    {
        PAGED_CODE();

        ASSERT(ulSize);
        ASSERT(m_pbTop);
        ASSERT(m_pbCurrent);
        ASSERT(m_pbCurrent + UPTOQUAD(ulSize) <= m_pbTop + m_ulSize);

        PVOID pvResult = PVOID(m_pbCurrent);

        m_pbCurrent += UPTOQUAD(ulSize);

        return pvResult;
    }

    //
    // Determine the amount of space remaining in the heap.
    //
    ULONG BytesRemaining(void)
    {
        PAGED_CODE();

        ASSERT(m_pbTop);
        ASSERT(m_pbCurrent);
        ASSERT(m_pbCurrent <= m_pbTop + m_ulSize);

        return ULONG((m_pbTop + m_ulSize) - m_pbCurrent);
    }
};

/*****************************************************************************
 * ::new()
 *****************************************************************************
 * New function for creating objects with private heap.
 */
inline PVOID operator new
(
    size_t    iSize,
    PrivateHeap&    privateHeap
)
{
    PAGED_CODE();

    return privateHeap.Alloc(ULONG(iSize));
}

/*****************************************************************************
 * PcCreateSubdeviceDescriptor()
 *****************************************************************************
 * Creates a subdevice descriptor.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcCreateSubdeviceDescriptor
(
    IN      PPCFILTER_DESCRIPTOR    pPcFilterDescriptor,
    IN      ULONG                   CategoriesCount,
    IN      GUID *                  Categories,
    IN      ULONG                   StreamInterfacesCount,
    IN      PKSPIN_INTERFACE        StreamInterfaces,
    IN      ULONG                   FilterPropertySetCount,
    IN      PKSPROPERTY_SET         FilterPropertySets,
    IN      ULONG                   FilterEventSetCount,
    IN      PKSEVENT_SET            FilterEventSets,
    IN      ULONG                   PinPropertySetCount,
    IN      PKSPROPERTY_SET         PinPropertySets,
    IN      ULONG                   PinEventSetCount,
    IN      PKSEVENT_SET            PinEventSets,
    OUT     PSUBDEVICE_DESCRIPTOR * OutDescriptor
)
{
    PAGED_CODE();

    ASSERT(pPcFilterDescriptor);
    ASSERT(OutDescriptor);

    NTSTATUS ntStatus = STATUS_SUCCESS;

    //
    // Calculate how much memory we will need.
    //
    PrivateHeap privateHeap;
    privateHeap.Reserve(sizeof(SUBDEVICE_DESCRIPTOR));
    privateHeap.Reserve(sizeof(KSTOPOLOGY));
    privateHeap.Reserve(sizeof(KSPIN_DESCRIPTOR) * pPcFilterDescriptor->PinCount);
    privateHeap.Reserve(sizeof(PIN_CINSTANCES)   * pPcFilterDescriptor->PinCount);
    privateHeap.Reserve(sizeof(PROPERTY_TABLE)   * pPcFilterDescriptor->PinCount);
    privateHeap.Reserve(sizeof(EVENT_TABLE)      * pPcFilterDescriptor->PinCount);

    if (pPcFilterDescriptor->NodeCount)
    {
        privateHeap.Reserve(sizeof(GUID) * pPcFilterDescriptor->NodeCount);
        privateHeap.Reserve(sizeof(GUID) * pPcFilterDescriptor->NodeCount);
        privateHeap.Reserve(sizeof(PROPERTY_TABLE)   * pPcFilterDescriptor->NodeCount);
        privateHeap.Reserve(sizeof(EVENT_TABLE)      * pPcFilterDescriptor->NodeCount);
    }

    const PCPIN_DESCRIPTOR *pPcPinDescriptor = pPcFilterDescriptor->Pins;
    for (ULONG ul = pPcFilterDescriptor->PinCount; ul--; )
    {
        if (pPcPinDescriptor->KsPinDescriptor.DataRanges)
        {
            pPcPinDescriptor = PPCPIN_DESCRIPTOR(   PBYTE(pPcPinDescriptor) + pPcFilterDescriptor->PinSize );
        }
        else
        {
            ntStatus = STATUS_RANGE_NOT_FOUND;  //  DataRanges field is NULL
            break;                              //  Don't even bother, just exit
        }
    }

    if (NT_SUCCESS(ntStatus))   //  if fail above, fall through the rest
    {
        //
        // Allocate the required memory.
        //
        ntStatus = privateHeap.AllocateFromPool(PagedPool,'pFcP');
    }

    if (NT_SUCCESS(ntStatus))
    {
        PSUBDEVICE_DESCRIPTOR descr = new(privateHeap) SUBDEVICE_DESCRIPTOR;

        //
        // Initialize the SUBDEVICE_DESCRIPTOR structure.
        //
        descr->PinCount             = pPcFilterDescriptor->PinCount;
        descr->NodeCount            = pPcFilterDescriptor->NodeCount;

        descr->Topology             = new(privateHeap) KSTOPOLOGY;
        descr->PinDescriptors       = new(privateHeap) KSPIN_DESCRIPTOR[descr->PinCount];
        descr->PinInstances         = new(privateHeap) PIN_CINSTANCES[descr->PinCount];
        descr->PinPropertyTables    = new(privateHeap) PROPERTY_TABLE[descr->PinCount];
        descr->PinEventTables       = new(privateHeap) EVENT_TABLE[descr->PinCount];

        if (descr->NodeCount)
        {
            descr->Topology->TopologyNodes = new(privateHeap) GUID[descr->NodeCount];
            descr->Topology->TopologyNodesNames = new(privateHeap) GUID[descr->NodeCount];
            descr->NodePropertyTables = new(privateHeap) PROPERTY_TABLE[descr->NodeCount];
            descr->NodeEventTables = new(privateHeap) EVENT_TABLE[descr->NodeCount];
        }
        else
        {
            descr->Topology->TopologyNodes = NULL;
            descr->Topology->TopologyNodesNames = NULL;
            descr->NodePropertyTables = NULL;
            descr->NodeEventTables = NULL;
        }

        //
        // If the audio driver provides any categories they will replace
        // portcls's default categories.
        //
        if (pPcFilterDescriptor->CategoryCount != 0)
        {
            descr->Topology->CategoriesCount    = pPcFilterDescriptor->CategoryCount;
            descr->Topology->Categories         = pPcFilterDescriptor->Categories;
        }
        else
        {
            descr->Topology->CategoriesCount    = CategoriesCount;
            descr->Topology->Categories         = Categories;
        }

        //
        // KSTOPOLOGY comes completely from the driver.
        //
        descr->Topology->TopologyNodesCount         = pPcFilterDescriptor->NodeCount;
        descr->Topology->TopologyConnectionsCount   = pPcFilterDescriptor->ConnectionCount;
        descr->Topology->TopologyConnections        = pPcFilterDescriptor->Connections;

        //
        // Initialize the filter property tables with portcls's default
        // properties.
        //
        descr->FilterPropertyTable.PropertySetCount = FilterPropertySetCount;
        descr->FilterPropertyTable.PropertySets     = FilterPropertySets;
        descr->FilterPropertyTable.StaticSets       = TRUE;

        //
        // Do the same for the events.
        //
        descr->FilterEventTable.EventSetCount       = FilterEventSetCount;
        descr->FilterEventTable.EventSets           = FilterEventSets;
        descr->FilterEventTable.StaticSets          = TRUE;

        //
        // Copy node type and name GUIDs and merge a specific node's properties
        // and events into a KS1-style list of property and event sets
        // (KSPROPERTY_SET & KSEVENT_SET).
        //
        const PCNODE_DESCRIPTOR *pPcNodeDescriptor = pPcFilterDescriptor->Nodes;
        GUID *pGuidType = (GUID *) descr->Topology->TopologyNodes;
        GUID *pGuidName = (GUID *) descr->Topology->TopologyNodesNames;
        PPROPERTY_TABLE nodePropertyTable = descr->NodePropertyTables;
        PEVENT_TABLE    nodeEventTable = descr->NodeEventTables;
        for (ULONG node = 0; node < pPcFilterDescriptor->NodeCount; node++)
        {
            *pGuidType++ = *pPcNodeDescriptor->Type;
            if (pPcNodeDescriptor->Name)
            {
                *pGuidName++ = *pPcNodeDescriptor->Name;
            }
            else
            {
                *pGuidName++ = *pPcNodeDescriptor->Type;
            }

            //
            // Before we add the node properties, check if the same property
            // sets were on an already processed node. In this case we just set
            // the pointers to the already merged KSPROPERTY_SET.
            //
            PPROPERTY_TABLE twinPt = descr->NodePropertyTables;
            PPCNODE_DESCRIPTOR pPcNodeDescriptorTwin =
                PPCNODE_DESCRIPTOR(pPcFilterDescriptor->Nodes);
            ULONG twinNode;
            for
            (
                twinNode = 0;
                twinNode < node;
                twinNode++, twinPt++
            )
            {
                if  (   (   pPcNodeDescriptor->AutomationTable
                        ==  pPcNodeDescriptorTwin->AutomationTable
                        )
                    ||  (   pPcNodeDescriptor->AutomationTable
                        &&  pPcNodeDescriptorTwin->AutomationTable
                        &&  (   pPcNodeDescriptor->AutomationTable->PropertyCount
                            ==  pPcNodeDescriptorTwin->AutomationTable->PropertyCount
                            )
                        &&  (   pPcNodeDescriptor->AutomationTable->Properties
                            ==  pPcNodeDescriptorTwin->AutomationTable->Properties
                            )
                        &&  (   pPcNodeDescriptor->AutomationTable->PropertyItemSize
                            ==  pPcNodeDescriptorTwin->AutomationTable->PropertyItemSize
                            )
                        )
                    )
                {
                    *nodePropertyTable = *twinPt;
                    break;
                }

                pPcNodeDescriptorTwin =
                    PPCNODE_DESCRIPTOR
                    (   PBYTE(pPcNodeDescriptorTwin) + pPcFilterDescriptor->NodeSize
                    );
            }

            //
            // Create a new table if we have to.
            //
            if (twinNode == node)
            {
                nodePropertyTable->PropertySetCount = 0;
                nodePropertyTable->PropertySets = NULL;
                nodePropertyTable->StaticSets = TRUE;

                if  (   (pPcNodeDescriptor->AutomationTable)
                    &&  (pPcNodeDescriptor->AutomationTable->PropertyCount)
                    )
                {
                    PcAddToPropertyTable
                    (
                        nodePropertyTable,
                        pPcNodeDescriptor->AutomationTable->PropertyCount,
                        pPcNodeDescriptor->AutomationTable->Properties,
                        pPcNodeDescriptor->AutomationTable->PropertyItemSize
                    );
                }
            }

            //
            // Before we add the node events, check if the same event
            // sets were on an already processed node. In this case we just set
            // the pointers to the already merged KSEVENT_SET.
            //
            PEVENT_TABLE twinEt = descr->NodeEventTables;
            pPcNodeDescriptorTwin = PPCNODE_DESCRIPTOR(pPcFilterDescriptor->Nodes);
            ULONG twinENode;
            for
            (
                twinENode = 0;
                twinENode < node;
                twinENode++, twinEt++
            )
            {
                if  (   (   pPcNodeDescriptor->AutomationTable
                        ==  pPcNodeDescriptorTwin->AutomationTable
                        )
                    ||  (   pPcNodeDescriptor->AutomationTable
                        &&  pPcNodeDescriptorTwin->AutomationTable
                        &&  (   pPcNodeDescriptor->AutomationTable->EventCount
                            ==  pPcNodeDescriptorTwin->AutomationTable->EventCount
                            )
                        &&  (   pPcNodeDescriptor->AutomationTable->Events
                            ==  pPcNodeDescriptorTwin->AutomationTable->Events
                            )
                        &&  (   pPcNodeDescriptor->AutomationTable->EventItemSize
                            ==  pPcNodeDescriptorTwin->AutomationTable->EventItemSize
                            )
                        )
                    )
                {
                    *nodeEventTable = *twinEt;
                    break;
                }

                pPcNodeDescriptorTwin =
                    PPCNODE_DESCRIPTOR
                    (   PBYTE(pPcNodeDescriptorTwin) + pPcFilterDescriptor->NodeSize
                    );
            }

            //
            // Create a new table if we have to.
            //
            if  (twinENode == node)
            {
                nodeEventTable->EventSetCount = 0;
                nodeEventTable->EventSets = NULL;
                nodeEventTable->StaticSets = TRUE;

                if  (   (pPcNodeDescriptor->AutomationTable)
                    &&  (pPcNodeDescriptor->AutomationTable->EventCount)
                    )
                {
                    PcAddToEventTable
                    (
                        nodeEventTable,
                        pPcNodeDescriptor->AutomationTable->EventCount,
                        pPcNodeDescriptor->AutomationTable->Events,
                        pPcNodeDescriptor->AutomationTable->EventItemSize
                    );
                }
            }

            pPcNodeDescriptor =
                PPCNODE_DESCRIPTOR
                (   PBYTE(pPcNodeDescriptor) + pPcFilterDescriptor->NodeSize
                );

            //
            // Advance the node and event table pointers.
            //
            nodeEventTable++;
            nodePropertyTable++;
        }

        //
        // Merge filter properties from the driver into portcls's default filter
        // properties.
        //
        if  (   (pPcFilterDescriptor->AutomationTable)
            &&  (pPcFilterDescriptor->AutomationTable->PropertyCount)
            )
        {
            PcAddToPropertyTable
            (
                &descr->FilterPropertyTable,
                pPcFilterDescriptor->AutomationTable->PropertyCount,
                pPcFilterDescriptor->AutomationTable->Properties,
                pPcFilterDescriptor->AutomationTable->PropertyItemSize
            );
        }

        //
        // Merge filter events from the driver into portcls's default filter
        // events.
        //
        if  (   (pPcFilterDescriptor->AutomationTable)
            &&  (pPcFilterDescriptor->AutomationTable->EventCount)
            )
        {
            PcAddToEventTable
            (
                &descr->FilterEventTable,
                pPcFilterDescriptor->AutomationTable->EventCount,
                pPcFilterDescriptor->AutomationTable->Events,
                pPcFilterDescriptor->AutomationTable->EventItemSize
            );
        }

        //
        // Do per-pin stuff.
        //
        PPROPERTY_TABLE     pt  = descr->PinPropertyTables;
        PEVENT_TABLE        et  = descr->PinEventTables;
        PKSPIN_DESCRIPTOR   p   = descr->PinDescriptors;
        PPIN_CINSTANCES     i   = descr->PinInstances;

        pPcPinDescriptor = PPCPIN_DESCRIPTOR(pPcFilterDescriptor->Pins);
        for
        (
            ULONG pin = 0;
            pin < pPcFilterDescriptor->PinCount;
            pin++
        )
        {
            //
            // Find a pin that has the same property set.
            //
            PPROPERTY_TABLE twinPt = descr->PinPropertyTables;
            PPCPIN_DESCRIPTOR pPcPinDescriptorTwin =
                PPCPIN_DESCRIPTOR(pPcFilterDescriptor->Pins);
            ULONG twinPin;
            for
            (
                twinPin = 0;
                twinPin < pin;
                twinPin++, twinPt++
            )
            {
                if  (   (   pPcPinDescriptor->AutomationTable
                        ==  pPcPinDescriptorTwin->AutomationTable
                        )
                    ||  (   pPcPinDescriptor->AutomationTable
                        &&  pPcPinDescriptorTwin->AutomationTable
                        &&  (   pPcPinDescriptor->AutomationTable->PropertyCount
                            ==  pPcPinDescriptorTwin->AutomationTable->PropertyCount
                            )
                        &&  (   pPcPinDescriptor->AutomationTable->Properties
                            ==  pPcPinDescriptorTwin->AutomationTable->Properties
                            )
                        &&  (   pPcPinDescriptor->AutomationTable->PropertyItemSize
                            ==  pPcPinDescriptorTwin->AutomationTable->PropertyItemSize
                            )
                        )
                    )
                {
                    *pt = *twinPt;
                    break;
                }

                pPcPinDescriptorTwin =
                    PPCPIN_DESCRIPTOR
                    (   PBYTE(pPcPinDescriptorTwin) + pPcFilterDescriptor->PinSize
                    );
            }

            //
            // Create a new table if we have to.
            //
            if (twinPin == pin)
            {
                pt->PropertySetCount = PinPropertySetCount;
                pt->PropertySets     = PinPropertySets;
                pt->StaticSets       = TRUE;

                if  (   (pPcPinDescriptor->AutomationTable)
                    &&  (pPcPinDescriptor->AutomationTable->PropertyCount)
                    )
                {
                    PcAddToPropertyTable
                    (
                        pt,
                        pPcPinDescriptor->AutomationTable->PropertyCount,
                        pPcPinDescriptor->AutomationTable->Properties,
                        pPcPinDescriptor->AutomationTable->PropertyItemSize
                    );
                }
            }
            pt++;

            //
            // Find a pin that has the same event set.
            //
            PEVENT_TABLE twinEt = descr->PinEventTables;
            pPcPinDescriptorTwin = PPCPIN_DESCRIPTOR(pPcFilterDescriptor->Pins);
            ULONG twinEPin;
            for
            (
                twinEPin = 0;
                twinEPin < pin;
                twinEPin++, twinEt++
            )
            {
                if  (   (   pPcPinDescriptor->AutomationTable
                        ==  pPcPinDescriptorTwin->AutomationTable
                        )
                    ||  (   pPcPinDescriptor->AutomationTable
                        &&  pPcPinDescriptorTwin->AutomationTable
                        &&  (   pPcPinDescriptor->AutomationTable->EventCount
                            ==  pPcPinDescriptorTwin->AutomationTable->EventCount
                            )
                        &&  (   pPcPinDescriptor->AutomationTable->Events
                            ==  pPcPinDescriptorTwin->AutomationTable->Events
                            )
                        &&  (   pPcPinDescriptor->AutomationTable->EventItemSize
                            ==  pPcPinDescriptorTwin->AutomationTable->EventItemSize
                            )
                        )
                    )
                {
                    *et = *twinEt;
                    break;
                }

                pPcPinDescriptorTwin =
                    PPCPIN_DESCRIPTOR
                    (   PBYTE(pPcPinDescriptorTwin) + pPcFilterDescriptor->PinSize
                    );
            }

            //
            // Create a new table if we have to.
            //
            if  (twinEPin == pin)
            {
                et->EventSetCount = PinEventSetCount;
                et->EventSets     = PinEventSets;
                et->StaticSets    = TRUE;

                if  (   (pPcPinDescriptor->AutomationTable)
                    &&  (pPcPinDescriptor->AutomationTable->EventCount)
                    )
                {
                    PcAddToEventTable
                    (
                        et,
                        pPcPinDescriptor->AutomationTable->EventCount,
                        pPcPinDescriptor->AutomationTable->Events,
                        pPcPinDescriptor->AutomationTable->EventItemSize
                    );
                }
            }
            et++;

            //
            // Copy the KS descriptor.
            //
            *p = pPcPinDescriptor->KsPinDescriptor;

            //
            // Provide default mediums if necessary.
            //
            if (p->Mediums == NULL)
            {
                p->MediumsCount = SIZEOF_ARRAY(PinMediums);
                p->Mediums      = PinMediums;
            }

            //
            // Massage the data ranges.
            //
            p->DataRangesCount = pPcPinDescriptor->KsPinDescriptor.DataRangesCount;
            p->DataRanges = pPcPinDescriptor->KsPinDescriptor.DataRanges;

            //
            // Provide default interfaces if necessary.
            //
            if  (   (p->Communication & KSPIN_COMMUNICATION_BOTH)
                &&  (p->Interfaces == NULL)
                )
            {
                p->InterfacesCount  = StreamInterfacesCount;
                p->Interfaces       = StreamInterfaces;
            }
            p++;

            i->FilterPossible   = pPcPinDescriptor->MaxFilterInstanceCount;
            i->FilterNecessary  = pPcPinDescriptor->MinFilterInstanceCount;
            i->GlobalPossible   = pPcPinDescriptor->MaxGlobalInstanceCount;
            i->GlobalCurrent    = 0;
            i++;

            pPcPinDescriptor =
                PPCPIN_DESCRIPTOR
                (   PBYTE(pPcPinDescriptor) + pPcFilterDescriptor->PinSize
                );
        }

        *OutDescriptor = descr;

        ASSERT(privateHeap.BytesRemaining() == 0);
    }

    return ntStatus;
}

/*****************************************************************************
 * PcDeleteSubdeviceDescriptor()
 *****************************************************************************
 * Deletes a subdevice descriptor.
 */
PORTCLASSAPI
void
NTAPI
PcDeleteSubdeviceDescriptor
(
    IN      PSUBDEVICE_DESCRIPTOR   pSubdeviceDescriptor
)
{
    PAGED_CODE();

    //
    // Free allocated memory for filter property and event tables.
    //
    PcFreePropertyTable(&pSubdeviceDescriptor->FilterPropertyTable);
    PcFreeEventTable(&pSubdeviceDescriptor->FilterEventTable);

    //
    // Free allocated memory for node property tables.
    //
    PPROPERTY_TABLE pPropertyTable = pSubdeviceDescriptor->NodePropertyTables;
    for (ULONG ul = pSubdeviceDescriptor->NodeCount; ul--; pPropertyTable++)
    {
        //
        // Find and clear any references to the same property set.
        //
        for
        (   PPROPERTY_TABLE pPropertyTableTwin =
            (   pSubdeviceDescriptor->NodePropertyTables
            +   (   pSubdeviceDescriptor->NodeCount
                -   1
                )
            )
        ;   pPropertyTableTwin != pPropertyTable
        ;   pPropertyTableTwin--
        )
        {
            if
            (   pPropertyTableTwin->PropertySets
            ==  pPropertyTable->PropertySets
            )
            {
                pPropertyTableTwin->PropertySetCount    = 0;
                pPropertyTableTwin->PropertySets        = NULL;
                pPropertyTableTwin->StaticSets          = TRUE;
                pPropertyTableTwin->StaticItems         = NULL;
            }
        }

        PcFreePropertyTable(pPropertyTable);
    }

    //
    // Free allocated memory for node event tables.
    //
    PEVENT_TABLE pEventTable = pSubdeviceDescriptor->NodeEventTables;
    for (ULONG ul = pSubdeviceDescriptor->NodeCount; ul--; pEventTable++)
    {
        //
        // Find and clear any references to the same event set.
        //
        for
        (   PEVENT_TABLE pEventTableTwin =
            (   pSubdeviceDescriptor->NodeEventTables
            +   (   pSubdeviceDescriptor->NodeCount
                -   1
                )
            )
        ;   pEventTableTwin != pEventTable
        ;   pEventTableTwin--
        )
        {
            if
            (   pEventTableTwin->EventSets
            ==  pEventTable->EventSets
            )
            {
                pEventTableTwin->EventSetCount      = 0;
                pEventTableTwin->EventSets          = NULL;
                pEventTableTwin->StaticSets         = TRUE;
                pEventTableTwin->StaticItems        = NULL;
            }
        }

        PcFreeEventTable(pEventTable);
    }

    //
    // Free allocated memory for pin property tables.
    //
    pPropertyTable = pSubdeviceDescriptor->PinPropertyTables;
    for (ULONG ul = pSubdeviceDescriptor->PinCount; ul--; pPropertyTable++)
    {
        //
        // Find and clear any references to the same property set.
        //
        for
        (   PPROPERTY_TABLE pPropertyTableTwin =
            (   pSubdeviceDescriptor->PinPropertyTables
            +   (   pSubdeviceDescriptor->PinCount
                -   1
                )
            )
        ;   pPropertyTableTwin != pPropertyTable
        ;   pPropertyTableTwin--
        )
        {
            if
            (   pPropertyTableTwin->PropertySets
            ==  pPropertyTable->PropertySets
            )
            {
                pPropertyTableTwin->PropertySetCount    = 0;
                pPropertyTableTwin->PropertySets        = NULL;
                pPropertyTableTwin->StaticSets          = TRUE;
                pPropertyTableTwin->StaticItems         = NULL;
            }
        }

        PcFreePropertyTable(pPropertyTable);
    }

    //
    // Free allocated memory for pin event tables.
    //
    pEventTable = pSubdeviceDescriptor->PinEventTables;
    for (ULONG ul = pSubdeviceDescriptor->PinCount; ul--; pEventTable++)
    {
        //
        // Find and clear any references to the same event set.
        //
        for
        (   PEVENT_TABLE pEventTableTwin =
            (   pSubdeviceDescriptor->PinEventTables
            +   (   pSubdeviceDescriptor->PinCount
                -   1
                )
            )
        ;   pEventTableTwin != pEventTable
        ;   pEventTableTwin--
        )
        {
            if
            (   pEventTableTwin->EventSets
            ==  pEventTable->EventSets
            )
            {
                pEventTableTwin->EventSetCount      = 0;
                pEventTableTwin->EventSets          = NULL;
                pEventTableTwin->StaticSets         = TRUE;
                pEventTableTwin->StaticItems        = NULL;
            }
        }

        PcFreeEventTable(pEventTable);
    }

    //
    // The rest is one big chunk.
    //
    delete [] PBYTE(pSubdeviceDescriptor);
}

/*****************************************************************************
 * PcValidateConnectRequest()
 *****************************************************************************
 * Validate attempt to create a pin.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcValidateConnectRequest
(   IN      PIRP                    pIrp
,   IN      PSUBDEVICE_DESCRIPTOR   pSubdeviceDescriptor
,   OUT     PKSPIN_CONNECT *        ppKsPinConnect
)
{
    PAGED_CODE();

    ASSERT(pIrp);
    ASSERT(pSubdeviceDescriptor);
    ASSERT(ppKsPinConnect);

    NTSTATUS ntStatus =
        KsValidateConnectRequest
        (   pIrp
        ,   pSubdeviceDescriptor->PinCount
        ,   pSubdeviceDescriptor->PinDescriptors
        ,   ppKsPinConnect
        );

    //  Now, add additional format validations (borrowed from Sysaudio)

    if (NT_SUCCESS(ntStatus))
    {
        ntStatus = ValidateDataFormat((PKSDATAFORMAT)(*ppKsPinConnect + 1));
    }

    return ntStatus;
}

/*****************************************************************************
 * PcValidatePinCount()
 *****************************************************************************
 * Validate pin count.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcValidatePinCount
(   IN      ULONG                   ulPinId
,   IN      PSUBDEVICE_DESCRIPTOR   pSubdeviceDescriptor
,   IN      PULONG                  pulPinInstanceCounts
)
{
    PAGED_CODE();

    ASSERT(pSubdeviceDescriptor);
    ASSERT(pulPinInstanceCounts);

    NTSTATUS ntStatus = STATUS_SUCCESS;

    if
    (   (   pSubdeviceDescriptor->PinInstances[ulPinId].GlobalCurrent
        <   pSubdeviceDescriptor->PinInstances[ulPinId].GlobalPossible
        )
    ||  (   pulPinInstanceCounts[ulPinId]
        <   pSubdeviceDescriptor->PinInstances[ulPinId].FilterPossible
        )
    )
    {
        pSubdeviceDescriptor->PinInstances[ulPinId].GlobalCurrent++;
        pulPinInstanceCounts[ulPinId]++;

        _DbgPrintF( DEBUGLVL_VERBOSE,
        (   "Create pin %d:  global=%d  local=%d"
        ,   ulPinId
        ,   pSubdeviceDescriptor->PinInstances[ulPinId].GlobalCurrent
        ,   pulPinInstanceCounts[ulPinId]
        ));
    }
    else
    {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    return ntStatus;
}

/*****************************************************************************
 * PcValidateDeviceContext()
 *****************************************************************************
 * Probes DeviceContext for writing.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcValidateDeviceContext
(   IN      PDEVICE_CONTEXT         pDeviceContext,
    IN      PIRP                    pIrp
)
{
    PAGED_CODE();

    NTSTATUS ntStatus = STATUS_SUCCESS;

    if (NULL == pDeviceContext)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcValidateDeviceContext : pDeviceContext = NULL"));
        return STATUS_INVALID_PARAMETER;
    }

























    if (NT_SUCCESS(ntStatus))
    {
        if (PORTCLS_DEVICE_EXTENSION_SIGNATURE != pDeviceContext->Signature )
        {
            ntStatus = STATUS_INVALID_PARAMETER;
            _DbgPrintF(DEBUGLVL_TERSE, ("PcValidateDeviceContext : Invalid Extension Signature"));
        }
    }

    return ntStatus;
} // PcValidateDeviceContext

/*****************************************************************************
 * PcTerminateConnection()
 *****************************************************************************
 * Decrement instance counts associated with a pin.
 */
PORTCLASSAPI
void
NTAPI
PcTerminateConnection
(   IN      PSUBDEVICE_DESCRIPTOR   pSubdeviceDescriptor
,   IN      PULONG                  pulPinInstanceCounts
,   IN      ULONG                   ulPinId
)
{
    PAGED_CODE();

    ASSERT(pSubdeviceDescriptor);
    ASSERT(pulPinInstanceCounts);
    ASSERT(ulPinId <= pSubdeviceDescriptor->PinCount);

    pSubdeviceDescriptor->PinInstances[ulPinId].GlobalCurrent--;
    pulPinInstanceCounts[ulPinId]--;

    _DbgPrintF( DEBUGLVL_VERBOSE,
    (   "Delete pin %d:  global=%d  local=%d"
    ,   ulPinId
    ,   pSubdeviceDescriptor->PinInstances[ulPinId].GlobalCurrent
    ,   pulPinInstanceCounts[ulPinId]
    ));
}

/*****************************************************************************
 * PcVerifyFilterIsReady()
 *****************************************************************************
 * Verify necessary pins are connected.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcVerifyFilterIsReady
(   IN      PSUBDEVICE_DESCRIPTOR   pSubdeviceDescriptor
,   IN      PULONG                  pulPinInstanceCounts
)
{
    PAGED_CODE();

    ASSERT(pSubdeviceDescriptor);
    ASSERT(pulPinInstanceCounts);

    NTSTATUS ntStatus = STATUS_SUCCESS;
    for
    (   ULONG ulPinId = 0
    ;   ulPinId < pSubdeviceDescriptor->PinCount
    ;   ulPinId++
    )
    {
        if
        (   pulPinInstanceCounts[ulPinId]
        <   pSubdeviceDescriptor->PinInstances[ulPinId].FilterNecessary
        )
        {



            ntStatus = STATUS_UNSUCCESSFUL;
            break;
        }
    }

    return ntStatus;
}

#define END_NONE 0
#define END_FROM 1
#define END_TO   2
#define END_BOTH 3

/*****************************************************************************
 * FindConnectionToPin()
 *****************************************************************************
 * Find a connection that connects to a given node or filter pin.
 *
 *      ulNode          - node number of KSFILTER_NODE
 *      ulConnection    - in: connection to start with
 *                        out: found connection or ULONG(-1) if not found
 *
 *      return          - 0 for no connection found
 *                        END_FROM for outgoing connection
 *                        END_TO for incoming connection
 */
ULONG
FindConnectionToPin
(
    IN      ULONG                       ulNode,
    IN      ULONG                       ulPin,
    IN      PKSTOPOLOGY                 pKsTopology,
    __deref_inout_range(-1, pKsTopology->TopologyConnectionsCount - 1)  PULONG ulConnection,
    OUT     PKSTOPOLOGY_CONNECTION *    ppKsTopologyConnection  OPTIONAL
)
{
    PAGED_CODE();

    ASSERT(pKsTopology);
    ASSERT(ulConnection);
    ASSERT(*ulConnection < pKsTopology->TopologyConnectionsCount);

    ULONG ulEnd;

    PKSTOPOLOGY_CONNECTION pKsTopologyConnection =
        PKSTOPOLOGY_CONNECTION
        (
            &pKsTopology->TopologyConnections[*ulConnection]
        );

    while (1)
    {
        ASSERT(*ulConnection <= pKsTopology->TopologyConnectionsCount);

        if (*ulConnection == pKsTopology->TopologyConnectionsCount)
        {
            ulEnd = END_NONE;
            *ulConnection = ULONG(-1);
            pKsTopologyConnection = NULL;
            break;
        }
        else
        if  (   (pKsTopologyConnection->FromNode == ulNode)
            &&  (pKsTopologyConnection->FromNodePin == ulPin)
            )
        {
            ulEnd = END_FROM;
            break;
        }
        else
        if  (   (pKsTopologyConnection->ToNode == ulNode)
            &&  (pKsTopologyConnection->ToNodePin == ulPin)
            )
        {
            ulEnd = END_TO;
            break;
        }

        (*ulConnection)++;
        pKsTopologyConnection++;
    }

    if (ppKsTopologyConnection)
    {
        *ppKsTopologyConnection = pKsTopologyConnection;
    }

    return ulEnd;
}

/*****************************************************************************
 * FindConnectionToNode()
 *****************************************************************************
 * Find a connection that connects to a given node or to the filter.
 *
 *      ulNode          - node number of KSFILTER_NODE
 *      ulEnd           - 0 for any direction
 *                        END_FROM for outgoing connection
 *                        END_TO for incoming connection
 *      ulConnection    - in: connection to start with
 *                        out: found connection or ULONG(-1) if not found
 *
 *      return          - 0 for no connection found
 *                        END_FROM for outgoing connection
 *                        END_TO for incoming connection
 */
ULONG
FindConnectionToNode
(
    IN      ULONG                       ulNode,
    IN      ULONG                       ulEnd,
    IN      PKSTOPOLOGY                 pKsTopology,
    __deref_inout_range(-1, pKsTopology->TopologyConnectionsCount - 1)  PULONG ulConnection,
    OUT     PKSTOPOLOGY_CONNECTION *    ppKsTopologyConnection  OPTIONAL
)
{
    PAGED_CODE();

    ASSERT(pKsTopology);
    ASSERT
    (   (ulNode == KSFILTER_NODE)
    ||  (ulNode < pKsTopology->TopologyNodesCount)
    );
    ASSERT(ulConnection);
    ASSERT(*ulConnection < pKsTopology->TopologyConnectionsCount);

    PKSTOPOLOGY_CONNECTION pKsTopologyConnection =
        PKSTOPOLOGY_CONNECTION
        (
            &pKsTopology->TopologyConnections[*ulConnection]
        );

    while (1)
    {
        ASSERT(*ulConnection <= pKsTopology->TopologyConnectionsCount);

        if (*ulConnection == pKsTopology->TopologyConnectionsCount)
        {
            ulEnd = END_NONE;
            *ulConnection = ULONG(-1);
            pKsTopologyConnection = NULL;
            break;
        }
        else
        if  (   (pKsTopologyConnection->FromNode == ulNode)
            &&  (ulEnd != END_TO)
            )
        {
            ulEnd = END_FROM;
            break;
        }
        else
        if  (   (pKsTopologyConnection->ToNode == ulNode)
            &&  (ulEnd != END_FROM)
            )
        {
            ulEnd = END_TO;
            break;
        }

        (*ulConnection)++;
        pKsTopologyConnection++;
    }

    if (ppKsTopologyConnection)
    {
        *ppKsTopologyConnection = pKsTopologyConnection;
    }

    return ulEnd;
}

/*****************************************************************************
 * NodeIsTransform()
 *****************************************************************************
 * Determines if a node is a transform.  KSFILTER_NODE is handled (FALSE).
 */
BOOLEAN
NodeIsTransform
(
    IN      ULONG       ulNode,
    IN      PKSTOPOLOGY pKsTopology
)
{
    PAGED_CODE();

    ASSERT(pKsTopology);
    ASSERT((ulNode == KSFILTER_NODE) || (ulNode < pKsTopology->TopologyNodesCount));

    ULONG ulEnd = END_NONE;

    if (ulNode != KSFILTER_NODE)
    {
        PKSTOPOLOGY_CONNECTION pKsTopologyConnection =
            PKSTOPOLOGY_CONNECTION(pKsTopology->TopologyConnections);

        for
        (
            ULONG ul = pKsTopology->TopologyConnectionsCount;
            ul--;
            pKsTopologyConnection++
        )
        {
            if (pKsTopologyConnection->FromNode == ulNode)
            {
                ulEnd += END_FROM;

                if  (   (ulEnd != END_FROM)
                    &&  (ulEnd != END_BOTH)
                    )
                {
                    break;
                }
            }
            if (pKsTopologyConnection->ToNode == ulNode)
            {
                ulEnd += END_TO;

                if  (   (ulEnd != END_TO)
                    &&  (ulEnd != END_BOTH)
                    )
                {
                    break;
                }
            }
        }
    }

    return ulEnd == END_BOTH;
}

/*****************************************************************************
 * NodeAtThisEnd()
 *****************************************************************************
 * Node at indicated end of the connection.
 */
inline
ULONG
NodeAtThisEnd
(
    IN      ULONG                   ulEnd,
    IN      PKSTOPOLOGY_CONNECTION  pKsTopologyConnection
)
{
    PAGED_CODE();

    return ((ulEnd == END_FROM) ? pKsTopologyConnection->FromNode : pKsTopologyConnection->ToNode);
}

/*****************************************************************************
 * NodeAtOtherEnd()
 *****************************************************************************
 * Node at opposite end of the connection.
 */
inline
ULONG
NodeAtOtherEnd
(
    IN      ULONG                   ulEnd,
    IN      PKSTOPOLOGY_CONNECTION  pKsTopologyConnection
)
{
    PAGED_CODE();

    return ((ulEnd == END_FROM) ? pKsTopologyConnection->ToNode : pKsTopologyConnection->FromNode);
}

/*****************************************************************************
 * PcCaptureFormat()
 *****************************************************************************
 * Capture a data format in an allocated buffer, possibly changing offensive
 * formats.
 *
 * Note attributes are not supported for KSDATAFORMAT_SPECIFIER_DSOUND.
 *
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcCaptureFormat
(
    OUT     PKSDATAFORMAT *         ppKsDataFormatOut,
    IN      PKSDATAFORMAT           pKsDataFormatIn,
    IN      ULONG                   ulKsDataFormatIn, // Real length of the pKsDataFormatIn buffer
    IN      PSUBDEVICE_DESCRIPTOR   pSubdeviceDescriptor,
    IN      ULONG                   ulPinId
)
{
    PAGED_CODE();

    ASSERT(ppKsDataFormatOut);
    ASSERT(pKsDataFormatIn);
    ASSERT(pSubdeviceDescriptor);
    ASSERT(ulPinId < pSubdeviceDescriptor->PinCount);

    NTSTATUS ntStatus = ValidateDataFormat(pKsDataFormatIn);

    if  (!NT_SUCCESS(ntStatus))
    {
        return  ntStatus;
    }

    if( (pKsDataFormatIn->FormatSize >= sizeof(KSDATAFORMAT_DSOUND)) &&
        IsEqualGUIDAligned( pKsDataFormatIn->MajorFormat, KSDATAFORMAT_TYPE_AUDIO ) &&
        IsEqualGUIDAligned( pKsDataFormatIn->Specifier, KSDATAFORMAT_SPECIFIER_DSOUND ) )
    {
        //
        // This is the dead DSound format.  Check to see if we have the
        // required topology and convert to WaveFormatEx if we do.
        //
        // Note: DSound format does not have to be PCM....
        //
        PKSDATAFORMAT_DSOUND pKsDataFormatDSound =
            PKSDATAFORMAT_DSOUND(pKsDataFormatIn);

        //
        // Fail if the client has asked for a software buffer.
        //
        if  (   pKsDataFormatDSound->BufferDesc.Flags
            &   KSDSOUND_BUFFER_LOCSOFTWARE
            )
        {
            _DbgPrintF(DEBUGLVL_TERSE,("PcCaptureFormat  Failed because client requested software buffer."));
            return STATUS_INVALID_PARAMETER;
        }

        //
        // Find a connection involving the filter pin.
        //
        ULONG ulConnection = 0;
        PKSTOPOLOGY_CONNECTION pKsTopologyConnection;
        ULONG ulEnd =
            FindConnectionToPin
            (
                KSFILTER_NODE,
                ulPinId,
                pSubdeviceDescriptor->Topology,
                &ulConnection,
                &pKsTopologyConnection
            );

        //
        // Trace the topology until we find a non-transform or all the
        // required nodes have been found.  Position notification is
        // always supported.
        //
        ULONG ulMissing =
            (   pKsDataFormatDSound->BufferDesc.Control
            &   ~KSDSOUND_BUFFER_CTRL_POSITIONNOTIFY
            );

        while (ulMissing && ulEnd)
        {
            //
            // Found a connection.  Follow the topology.
            //
            ULONG ulNode = NodeAtOtherEnd(ulEnd,pKsTopologyConnection);

            if (! NodeIsTransform(ulNode,pSubdeviceDescriptor->Topology))
            {
                //
                // The new node is not a simple transform (1 in, 1 out).
                //
                break;
            }

            //
            // Drop 'missing' bits as appropriate based on the node GUID.
            //
            ASSERT(ulNode < pSubdeviceDescriptor->Topology->TopologyNodesCount);
            const GUID *pGuid = &pSubdeviceDescriptor->Topology->TopologyNodes[ulNode];
            if (IsEqualGUIDAligned(*pGuid,KSNODETYPE_3D_EFFECTS))
            {
                ulMissing &=~ KSDSOUND_BUFFER_CTRL_3D;
            }
            else
            if (IsEqualGUIDAligned(*pGuid,KSNODETYPE_SRC))
            {
                ulMissing &=~ KSDSOUND_BUFFER_CTRL_FREQUENCY;
            }
            else
            if  (   IsEqualGUIDAligned(*pGuid,KSNODETYPE_SUPERMIX)
                ||  IsEqualGUIDAligned(*pGuid,KSNODETYPE_VOLUME)
                )
            {
                ulMissing &=~ KSDSOUND_BUFFER_CTRL_PAN;
                ulMissing &=~ KSDSOUND_BUFFER_CTRL_VOLUME;
            }

            //
            // Find the next connection in line.
            //
            ulConnection = 0;
            ulEnd =
                FindConnectionToNode
                (
                    ulNode,
                    ulEnd,
                    pSubdeviceDescriptor->Topology,
                    &ulConnection,
                    &pKsTopologyConnection
                );
        }

        //
        // Make sure no nodes were missing.
        //
        if (! ulMissing)
        {
            //
            // We have the capabilities required.  Build the new format.
            //
            ULONG ulSize =
                (   sizeof(KSDATAFORMAT_WAVEFORMATEX)
                +   (   pKsDataFormatIn->FormatSize
                    -   sizeof(KSDATAFORMAT_DSOUND)
                    )
                );
            *ppKsDataFormatOut =
                PKSDATAFORMAT
                (
                    ExAllocatePoolWithTag
                    (
                        NonPagedPoolNx,   //  May be freed in DPC
                        ulSize,
                        'fDcP'
                    )
                );

            if (*ppKsDataFormatOut)
            {
                //
                // Copy KSDATAFORMAT part.
                //
                RtlCopyMemory
                (
                    *ppKsDataFormatOut,
                    pKsDataFormatIn,
                    sizeof(KSDATAFORMAT)
                );

                //
                // Copy WAVEFORMATEX part including appended stuff.
                //
                RtlCopyMemory
                (
                    *ppKsDataFormatOut + 1,
                    &pKsDataFormatDSound->BufferDesc.WaveFormatEx,
                    ulSize - sizeof(KSDATAFORMAT)
                );

                //
                // Adjust size and specifier.
                //
                (*ppKsDataFormatOut)->FormatSize = ulSize;
                (*ppKsDataFormatOut)->Specifier =
                    KSDATAFORMAT_SPECIFIER_WAVEFORMATEX;
            }
            else
            {
                _DbgPrintF(DEBUGLVL_TERSE,("PcCaptureFormat  Failed to allocate memory for format."));
                ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            }
        }
        else
        {
            _DbgPrintF(DEBUGLVL_VERBOSE,("PcCaptureFormat  Failed due to lack of feature support (0x%08x).",ulMissing));
            //
            // Don't have the required nodes...fail.
            //
            ntStatus = STATUS_INVALID_PARAMETER;
        }
    }
    else
    {
        _DbgPrintF(DEBUGLVL_VERBOSE,("PcCaptureFormat  Format captured as-is."));

        //
        // Some other format.  Just capture it.
        //

        PKSMULTIPLE_ITEM pKsAttributes = NULL;
        ULONG cbAttributes = 0;
        ULONG cbDataFormat = 0;

        // did the caller pass in an input buffer that's bigger then zero?  We check
        // this because pKsDataFormatIn->FormatSize is the first DWORD. We're going
        // to reference that much in the comparison below.

        if( ulKsDataFormatIn < sizeof(DWORD) )
        {
            _DbgPrintF(DEBUGLVL_TERSE,("PcCaptureFormat  Failed because client requested zero length buffer."));
            return STATUS_INVALID_PARAMETER;
        }

        // If the caller tells us that the output buffer is smaller then the FormatSize
        // of the block that they are querying about, we're going to return an error
        // saying that the buffer is to small. ulKsDataFormatIn is the buffer that is
        // to small. That is the buffer that we're going to read from thus we would
        // perform a buffer over read.

        if( pKsDataFormatIn->FormatSize > ulKsDataFormatIn )
        {
            _DbgPrintF(DEBUGLVL_TERSE,("PcCaptureFormat  FormatSize out of bounds for input buffer."));
            return STATUS_BUFFER_TOO_SMALL;
        }

        //
        // Some other format.  Just capture it.
        //

        // If attributes were specified, align the data format size and calculate attributes
        // pointer. Portcls and KS will have validated the read buffer size and consistent
        // attribute buffer sizes prior to calling this function. See PcValidateConnectRequest
        // and KsValidateConnectRequest.
        cbDataFormat = pKsDataFormatIn->FormatSize;
        if (pKsDataFormatIn->Flags & KSDATAFORMAT_ATTRIBUTES)
        {
            cbDataFormat = (cbDataFormat + FILE_QUAD_ALIGNMENT) & ~FILE_QUAD_ALIGNMENT;
            pKsAttributes = (PKSMULTIPLE_ITEM)((PBYTE)pKsDataFormatIn + cbDataFormat);
            cbAttributes = pKsAttributes->Size;
        }

        *ppKsDataFormatOut = PKSDATAFORMAT(ExAllocatePoolWithTag(NonPagedPoolNx,
                                                                 cbDataFormat + cbAttributes,
                                                                 'fDcP'));
        if (*ppKsDataFormatOut)
        {
            // Don't disclose anything in the heap
            RtlZeroMemory( *ppKsDataFormatOut, cbDataFormat + cbAttributes );

            RtlCopyMemory
            (
                *ppKsDataFormatOut,
                pKsDataFormatIn,
                cbDataFormat + cbAttributes
            );
        }
        else
        {
            _DbgPrintF(DEBUGLVL_TERSE,("PcCaptureFormat  Failed to allocate memory for format."));
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    // check to verify SampleSize is set properly on waveformatex formats
    if( NT_SUCCESS(ntStatus) &&
        (pKsDataFormatIn->FormatSize >= sizeof(KSDATAFORMAT_WAVEFORMATEX)) &&
        IsEqualGUIDAligned((*ppKsDataFormatOut)->MajorFormat,KSDATAFORMAT_TYPE_AUDIO) &&
        IsEqualGUIDAligned((*ppKsDataFormatOut)->Specifier,  KSDATAFORMAT_SPECIFIER_WAVEFORMATEX))
    {
        PKSDATAFORMAT_WAVEFORMATEX pWaveFormat = PKSDATAFORMAT_WAVEFORMATEX(*ppKsDataFormatOut);

        if( 0 == pWaveFormat->DataFormat.SampleSize )
        {
            pWaveFormat->DataFormat.SampleSize = pWaveFormat->WaveFormatEx.nBlockAlign;
        }
    }

    return ntStatus;
}

/*****************************************************************************
 * PcAcquireFormatResources()
 *****************************************************************************
 * Acquire resources specified in a format.
 */
PORTCLASSAPI
void
NTAPI
PcAcquireFormatResources
(
    IN      PKSDATAFORMAT           pKsDataFormatIn,
    IN      PSUBDEVICE_DESCRIPTOR   pSubdeviceDescriptor,
    IN      ULONG                   ulPinId,
    IN      PPROPERTY_CONTEXT       pPropertyContext
)
{
    PAGED_CODE();

    ASSERT(pKsDataFormatIn);
    ASSERT(pSubdeviceDescriptor);
    ASSERT(ulPinId < pSubdeviceDescriptor->PinCount);
    ASSERT(pPropertyContext);

    KSP_NODE ksPNode;
    ksPNode.Property.Set    = KSPROPSETID_TopologyNode;
    ksPNode.Property.Id     = KSPROPERTY_TOPOLOGYNODE_ENABLE;
    ksPNode.Property.Flags  = KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_TOPOLOGY;
    ksPNode.NodeId          = 0;    // Fill in later
    ksPNode.Reserved        = 0;

    if  (   (pKsDataFormatIn->FormatSize >= sizeof(KSDATAFORMAT_DSOUND))
        &&  IsEqualGUIDAligned
            (   pKsDataFormatIn->MajorFormat,
                KSDATAFORMAT_TYPE_AUDIO
            )
        &&  IsEqualGUIDAligned
            (   pKsDataFormatIn->Specifier,
                KSDATAFORMAT_SPECIFIER_DSOUND
            )
        )
    {
        //
        // This is the dreaded DSound format.  Turn on all the nodes
        // that are specified in the caps bits.
        //
        PKSDATAFORMAT_DSOUND pKsDataFormatDSound =
            PKSDATAFORMAT_DSOUND(pKsDataFormatIn);

        //
        // Find a connection involving the filter pin.
        //
        ULONG ulConnection = 0;
        PKSTOPOLOGY_CONNECTION pKsTopologyConnection;
        ULONG ulEnd =
            FindConnectionToPin
            (
                KSFILTER_NODE,
                ulPinId,
                pSubdeviceDescriptor->Topology,
                &ulConnection,
                &pKsTopologyConnection
            );

        //
        // Trace the topology until we find a non-transform or all the
        // required nodes have been found.  Position notification is
        // always supported.
        //
        ULONG ulMissing =
            (   pKsDataFormatDSound->BufferDesc.Control
            &   (   KSDSOUND_BUFFER_CTRL_3D
                |   KSDSOUND_BUFFER_CTRL_FREQUENCY
                )
            );

        while (ulMissing && ulEnd)
        {
            //
            // Found a connection.  Follow the topology.
            //
            ULONG ulNode = NodeAtOtherEnd(ulEnd,pKsTopologyConnection);

            if (! NodeIsTransform(ulNode,pSubdeviceDescriptor->Topology))
            {
                //
                // The new node is not a simple transform (1 in, 1 out).
                //
                break;
            }

            //
            // Turn on nodes as appropriate based on the node GUID.
            //
            ASSERT(ulNode < pSubdeviceDescriptor->Topology->TopologyNodesCount);
            const GUID *pGuid = &pSubdeviceDescriptor->Topology->TopologyNodes[ulNode];
            if (IsEqualGUIDAligned(*pGuid,KSNODETYPE_3D_EFFECTS))
            {
                if (ulMissing & KSDSOUND_BUFFER_CTRL_3D)
                {
                    //
                    // Turn on the 3D node.
                    //
                    ULONG ulPropertyValue = TRUE;
                    ULONG ulPropertyValueSize = sizeof(ULONG);
                    ksPNode.NodeId = ulNode;

                    PcDispatchProperty
                    (   NULL    // pIrp
                    ,   pPropertyContext
                    ,   NULL    // pKsPropertySet
                    ,   sizeof(KSP_NODE)
                    ,   &ksPNode.Property
                    ,   &ulPropertyValueSize
                    ,   &ulPropertyValue
                    );

                    ulMissing &=~ KSDSOUND_BUFFER_CTRL_3D;
                }
            }
            else
            if (IsEqualGUIDAligned(*pGuid,KSNODETYPE_SRC))
            {
                if (ulMissing & KSDSOUND_BUFFER_CTRL_FREQUENCY)
                {
                    //
                    // Turn on the SRC node.
                    //
                    ULONG ulPropertyValue = TRUE;
                    ULONG ulPropertyValueSize = sizeof(ULONG);
                    ksPNode.NodeId = ulNode;

                    PcDispatchProperty
                    (   NULL    // pIrp
                    ,   pPropertyContext
                    ,   NULL    // pKsPropertySet
                    ,   sizeof(KSP_NODE)
                    ,   &ksPNode.Property
                    ,   &ulPropertyValueSize
                    ,   &ulPropertyValue
                    );

                    ulMissing &=~ KSDSOUND_BUFFER_CTRL_FREQUENCY;
                }
            }

            //
            // Find the next connection in line.
            //
            ulConnection = 0;
            ulEnd =
                FindConnectionToNode
                (
                    ulNode,
                    ulEnd,
                    pSubdeviceDescriptor->Topology,
                    &ulConnection,
                    &pKsTopologyConnection
                );
        }
    }
}

#pragma code_seg()

/*****************************************************************************
 * PcRegisterIoTimeout()
 *****************************************************************************
 * Registers an IoTimeout callback.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcRegisterIoTimeout
(
    __in    PDEVICE_OBJECT      pDeviceObject,
    __in    PIO_TIMER_ROUTINE   pTimerRoutine,
    __in    PVOID               pContext
)
{
#ifdef ARM
    return STATUS_NOT_SUPPORTED;
#else
    KIRQL               OldIrql;
    PTIMEOUTCALLBACK    TimeoutCallback;
    NTSTATUS            ntStatus = STATUS_SUCCESS;

    ASSERT(pDeviceObject);
    ASSERT(pTimerRoutine);
    ASSERT( PASSIVE_LEVEL == KeGetCurrentIrql() );

    //
    // Validate Parameters.
    //
    if (NULL == pDeviceObject ||
        NULL == pTimerRoutine ||
        NULL == pDeviceObject->DeviceExtension)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcRegisterIoTimeout : Invalid Parameter"));
        return STATUS_INVALID_PARAMETER;
    }

    // get the device context
    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    // allocate a timeout callback structure -- 'PcTc'
    TimeoutCallback = PTIMEOUTCALLBACK(ExAllocatePoolWithTag( NonPagedPoolNx, sizeof(TIMEOUTCALLBACK),'cTcP' ));
    if( TimeoutCallback )
    {
        // initialize the entry
        TimeoutCallback->TimerRoutine = pTimerRoutine;
        TimeoutCallback->Context = pContext;

        // grab the list spin lock
        KeAcquireSpinLock( &(pDeviceContext->TimeoutLock), &OldIrql );

        // walk the list to see if the entry is already registered
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
                if( (pCallback->TimerRoutine == pTimerRoutine) &&
                    (pCallback->Context == pContext) )
                {
                    // entry already exists in the list, so fail this request
                    ntStatus = STATUS_UNSUCCESSFUL;
                }
            }
        }

        if( NT_SUCCESS(ntStatus) )
        {
            // add the entry to the list
            InsertTailList( &(pDeviceContext->TimeoutList), &(TimeoutCallback->ListEntry) );

            // restart the timer if needed
            ResetTimerState(pDeviceObject);            
        }
        else
        {
            // free the entry
            ExFreePool( TimeoutCallback );
        }

        // release the spin lock
        KeReleaseSpinLock( &(pDeviceContext->TimeoutLock), OldIrql );
    }
    else
    {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    return ntStatus;
#endif // ARM
}

/*****************************************************************************
 * PcUnregisterIoTimeout()
 *****************************************************************************
 * Unregisters an IoTimeout callback.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcUnregisterIoTimeout
(
    __in    PDEVICE_OBJECT      pDeviceObject,
    __in    PIO_TIMER_ROUTINE   pTimerRoutine,
    __in    PVOID               pContext
)
{
#ifdef ARM
    return STATUS_NOT_SUPPORTED;
#else
    KIRQL       OldIrql;

    ASSERT(pDeviceObject);
    ASSERT(pTimerRoutine);
    ASSERT( PASSIVE_LEVEL == KeGetCurrentIrql() );

    //
    // Validate Parameters.
    //
    if (NULL == pDeviceObject ||
        NULL == pTimerRoutine ||
        NULL == pDeviceObject->DeviceExtension)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcUnregisterIoTimeout : Invalid Parameter"));
        return STATUS_INVALID_PARAMETER;
    }

    // get the device context
    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    // grab the spin lock
    KeAcquireSpinLock( &(pDeviceContext->TimeoutLock), &OldIrql );

    // walk the list
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
            if( (pCallback->TimerRoutine == pTimerRoutine) &&
                (pCallback->Context == pContext) )
            {
                RemoveEntryList(ListEntry);

                // pause the timer if needed
                ResetTimerState(pDeviceObject);

                KeReleaseSpinLock( &(pDeviceContext->TimeoutLock), OldIrql );
                ExFreePool(pCallback);

                return STATUS_SUCCESS;
            }
        }
    }

    // release the spinlock
    KeReleaseSpinLock( &(pDeviceContext->TimeoutLock), OldIrql );

    return STATUS_NOT_FOUND;
#endif // ARM
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * iUpdatePinDescriptor
 *****************************************************************************
 * Internal helper that updates the descriptor for the specified pin on the 
 * specified subdevice.
 */
NTSTATUS
iUpdatePinDescriptor
(
    _In_    PDEVICE_OBJECT          _pDeviceObject,
    _In_    PSUBDEVICE_DESCRIPTOR   _pSubdeviceDescriptor,
    _In_    ULONG                   _ulPinId,
    _In_    ULONG                   _ulFlags,
    _In_    PPCPIN_DESCRIPTOR       _pPinDescriptor
)
{
    PDEVICE_CONTEXT pDeviceContext  = NULL;
    NTSTATUS        ntStatus        = STATUS_UNSUCCESSFUL;

    PAGED_CODE();

    if((_pPinDescriptor == NULL) ||
       (_pSubdeviceDescriptor == NULL) ||
       (_ulPinId >= _pSubdeviceDescriptor->PinCount))
    {
        ntStatus = STATUS_INVALID_PARAMETER;
        goto Done;
    }
    
    if (_pDeviceObject)
    {
        pDeviceContext = PDEVICE_CONTEXT(_pDeviceObject->DeviceExtension);
    }

    if (pDeviceContext)
    {
        // Lock out Creates/Ioctl processing.
        AcquireDevice(pDeviceContext);
    }

    if(_ulFlags & PCUPDATE_PIN_DESC_FLAG_DATARANGES)
    {
        PKSPIN_DESCRIPTOR pKSPinDesc = &(_pSubdeviceDescriptor->PinDescriptors[_ulPinId]);
        if(pKSPinDesc == NULL)
        {
            ntStatus = STATUS_UNSUCCESSFUL;
            goto Done;
        }

        pKSPinDesc->DataRangesCount = _pPinDescriptor->KsPinDescriptor.DataRangesCount;
        pKSPinDesc->DataRanges = _pPinDescriptor->KsPinDescriptor.DataRanges;
    }
    else
    {
        ntStatus = STATUS_NOT_IMPLEMENTED;
        goto Done;
    }

    ntStatus = STATUS_SUCCESS;

Done:
    
    if (pDeviceContext)
    {
        ReleaseDevice(pDeviceContext);
    }
    
    return ntStatus;
}

//-----------------------------------------------------------------------------
#pragma code_seg("PAGE")
NTSTATUS                        
PropertyHandler_BasicSupport
(
    IN PPCPROPERTY_REQUEST         PropertyRequest,
    IN ULONG                       Flags,
    IN DWORD                       PropTypeSetId
)
/*++

Routine Description:

  Default basic support handler. Basic processing depends on the size of data.
  For ULONG it only returns Flags. For KSPROPERTY_DESCRIPTION, the structure   
  is filled.

Arguments:

  PropertyRequest - 

  Flags - Support flags.

  PropTypeSetId - PropTypeSetId

Return Value:
    
    NT status code.

--*/
{
    PAGED_CODE();

    ASSERT(Flags & KSPROPERTY_TYPE_BASICSUPPORT);

    NTSTATUS                    ntStatus = STATUS_INVALID_PARAMETER;

    if (PropertyRequest->ValueSize >= sizeof(KSPROPERTY_DESCRIPTION))
    {
        // if return buffer can hold a KSPROPERTY_DESCRIPTION, return it
        //
        PKSPROPERTY_DESCRIPTION PropDesc = 
            PKSPROPERTY_DESCRIPTION(PropertyRequest->Value);

        PropDesc->AccessFlags       = Flags;
        PropDesc->DescriptionSize   = sizeof(KSPROPERTY_DESCRIPTION);
        if  (VT_ILLEGAL != PropTypeSetId)
        {
            PropDesc->PropTypeSet.Set   = KSPROPTYPESETID_General;
            PropDesc->PropTypeSet.Id    = PropTypeSetId;
        }
        else
        {
            PropDesc->PropTypeSet.Set   = GUID_NULL;
            PropDesc->PropTypeSet.Id    = 0;
        }
        PropDesc->PropTypeSet.Flags = 0;
        PropDesc->MembersListCount  = 0;
        PropDesc->Reserved          = 0;

        PropertyRequest->ValueSize = sizeof(KSPROPERTY_DESCRIPTION);
        ntStatus = STATUS_SUCCESS;
    } 
    else if (PropertyRequest->ValueSize >= sizeof(ULONG))
    {
        // if return buffer can hold a ULONG, return the access flags
        //
        *(PULONG(PropertyRequest->Value)) = Flags;

        PropertyRequest->ValueSize = sizeof(ULONG);
        ntStatus = STATUS_SUCCESS;                    
    }
    else
    {
        PropertyRequest->ValueSize = 0;
        ntStatus = STATUS_BUFFER_TOO_SMALL;
    }

    return ntStatus;
} // PropertyHandler_BasicSupport

#pragma code_seg("PAGE")
NTSTATUS PropertyHandlerAudioSignalProcessingModes
(
    IN PMINIPORTAudioSignalProcessing _pIAudioSignalProcessing,
    IN PPCPROPERTY_REQUEST PropertyRequest
)
/*++

Routine Description:

  Property handler for KSPROPERTY_AUDIOSIGNALPROCESSING_MODES

Arguments:

  _pIAudioSignalProcessing -

  PropertyRequest - 

Return Value:
    
    NT status code.

Remarks:

    This property is GET only.

--*/
{
    PAGED_CODE();

    NTSTATUS    ntStatus = STATUS_INVALID_DEVICE_REQUEST;
    PKSP_PIN    pKspPin;
    ULONG       numModes = 0;

    // This is a pin property on the filter, reject any query via pin, which has its instance
    // pointer stored in PropertyRequest->MinorTarget
    if (!((PropertyRequest->MajorTarget != NULL) && (PropertyRequest->MinorTarget == NULL)))
    {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    // The property data is a KSP_PIN. Verify the instance data size is large enough to store
    // the part of KSP_PIN beyond its KSPROPERTY header.
    if (PropertyRequest->InstanceSize < (sizeof(KSP_PIN) - RTL_SIZEOF_THROUGH_FIELD(KSP_PIN, Property)))
    {
        return STATUS_BUFFER_TOO_SMALL;
    }
    
    // Compute pointer to property data.
    pKspPin = CONTAINING_RECORD(PropertyRequest->Instance, KSP_PIN, PinId);
    
    // Verify the reserved field.
    if ((pKspPin == NULL) || (pKspPin->Reserved != 0))
    {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

     // Get only number of modes from miniport and verify caller's value buffer is large enough
    ntStatus = _pIAudioSignalProcessing->GetModes(pKspPin->PinId, nullptr, &numModes);
    if (!NT_SUCCESS(ntStatus))
    {
        return ntStatus;
    }
    // Pin doesn't support the property if numModes is zero.
    if (numModes == 0) 
    {
        // zero returned from miniport driver for GetModes indicates something bad happened in 
        // driver. 
        ASSERTMSG("PropertyHandlerAudioSignalProcessingModes: IMiniportAudioSignalProcessing->GetModes() "
                  "succeeded with no modes returned", FALSE);
        return STATUS_NOT_SUPPORTED;
    }
   
    if (PropertyRequest->Verb & KSPROPERTY_TYPE_BASICSUPPORT)
    {
        return PropertyHandler_BasicSupport(PropertyRequest, KSPROPERTY_TYPE_BASICSUPPORT | KSPROPERTY_TYPE_GET, VT_ILLEGAL);
    }

    if (PropertyRequest->Verb & KSPROPERTY_TYPE_GET)
    {
        PKSMULTIPLE_ITEM ksMultipleItem;
        LPGUID modes;
        ULONG valueBufferSizeRequired;

        valueBufferSizeRequired = sizeof(KSMULTIPLE_ITEM) + numModes * sizeof(GUID);

        if (PropertyRequest->ValueSize < valueBufferSizeRequired)
        {
            ntStatus = PropertyRequest->ValueSize == 0 ? STATUS_BUFFER_OVERFLOW : STATUS_BUFFER_TOO_SMALL;
            PropertyRequest->ValueSize = valueBufferSizeRequired;
            return ntStatus;
        }

        // Value is a KSMULTIPLE_ITEM followed by list of GUIDs.
        ksMultipleItem = (PKSMULTIPLE_ITEM)PropertyRequest->Value;
        modes = (LPGUID)(ksMultipleItem + 1);

        // Get list of modes from miniport
        ntStatus = _pIAudioSignalProcessing->GetModes(pKspPin->PinId, modes, &numModes);
        if (!NT_SUCCESS(ntStatus))
        {
            return ntStatus;
        }

        // Miniport filled in the list of GUIDs. Fill in the KSMULTIPLE_ITEM header.
        ksMultipleItem->Size = sizeof(KSMULTIPLE_ITEM) + numModes * sizeof(GUID);
        ksMultipleItem->Count = numModes;

        PropertyRequest->ValueSize = ksMultipleItem->Size;
        return STATUS_SUCCESS;
    }

    return STATUS_INVALID_DEVICE_REQUEST;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * PropertyHandler_AudioSignalProcessing()
 *****************************************************************************
 * Property handler for KSPROPSETID_AudioSignalProcessing.
 */
NTSTATUS
PropertyHandler_AudioSignalProcessing
( 
    IN PPCPROPERTY_REQUEST      PropertyRequest 
)
{
    PAGED_CODE();

    NTSTATUS ntStatus;
    PMINIPORTAudioSignalProcessing pIAudioSignalProcessing = NULL;

    // This property set is on filters, not streams (at least for all currently defined properties in this set).
    if (PropertyRequest->MinorTarget)
    {
        ntStatus = STATUS_INVALID_DEVICE_REQUEST;
        goto Exit;
    }

    ntStatus = PropertyRequest->MajorTarget->QueryInterface(IID_IMiniportAudioSignalProcessing, reinterpret_cast<PVOID*>(&pIAudioSignalProcessing));
    if (!NT_SUCCESS(ntStatus))
    {
        goto Exit;
    }

    if (!IsEqualGUIDAligned(*PropertyRequest->PropertyItem->Set, KSPROPSETID_AudioSignalProcessing))
    {
        ntStatus = STATUS_INVALID_DEVICE_REQUEST;
        goto Exit;
    }

    switch (PropertyRequest->PropertyItem->Id)
    {
        case KSPROPERTY_AUDIOSIGNALPROCESSING_MODES:
            ntStatus = PropertyHandlerAudioSignalProcessingModes(pIAudioSignalProcessing, PropertyRequest);
            break;
        default:
            ntStatus = STATUS_INVALID_DEVICE_REQUEST;
            break;
    }

    // ntStatus is set above.

Exit:

    if (pIAudioSignalProcessing != NULL)
    {
        pIAudioSignalProcessing->Release();
    }
    return ntStatus;
}

/*****************************************************************************
 * AudioSignalProcessingPinProperties
 *****************************************************************************
 */
PCPROPERTY_ITEM AudioSignalProcessingPinProperties[] = 
{
	{
		&KSPROPSETID_AudioSignalProcessing,
		KSPROPERTY_AUDIOSIGNALPROCESSING_MODES,
		KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_BASICSUPPORT,
		PropertyHandler_AudioSignalProcessing
	}
};

