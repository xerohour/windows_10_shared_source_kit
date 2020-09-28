/*****************************************************************************
 * port.cpp - DirectMusic port driver
 *****************************************************************************
 * Copyright (c) Microsoft.  All rights reserved.
 *



 */

#include "private.h"
#include "tracelogging.h"

#define STR_MODULENAME "DMus:Port: "

/*****************************************************************************
 * Factory
 */

#pragma code_seg("PAGE")
/*****************************************************************************
 * CreatePortDMus()
 *****************************************************************************
 * Creates a DirectMusic port driver.
 */
NTSTATUS
CreatePortDMus
(
    OUT     PUNKNOWN *  Unknown,
    IN      REFCLSID,
    IN      PUNKNOWN    UnknownOuter    OPTIONAL,
    IN      POOL_TYPE   PoolType
)
{
    PAGED_CODE();

    _DbgPrintF(DEBUGLVL_LIFETIME, ("Creating DMUS Port"));
    ASSERT(Unknown);

    STD_CREATE_BODY_( CPortDMus,
                      Unknown,
                      UnknownOuter,
                      PoolType,
                      PPORTDMUS);
}

/*****************************************************************************
 * PortDriverDMus
 *****************************************************************************
 * Port driver descriptor.  Referenced extern in porttbl.c.
 */
PORT_DRIVER
PortDriverDMus =
{
    &CLSID_PortDMus,
    CreatePortDMus
};

/*****************************************************************************
 * CreatePortMidi()
 *****************************************************************************
 * Creates a midi port driver.
 */
NTSTATUS
CreatePortMidi
(
    OUT     PUNKNOWN *  Unknown,
    IN      REFCLSID,
    IN      PUNKNOWN    UnknownOuter    OPTIONAL,
    IN      POOL_TYPE   PoolType
)
{
    PAGED_CODE();

    ASSERT(Unknown);

    _DbgPrintF(DEBUGLVL_LIFETIME,("Creating MIDI Port"));

    //
    // Support for Midi Miniports. The PPORTMIDI and PPORTDMUS interfaces
    // are exactly the same. Therefore it is OK to create CPortDMus for
    // PPORTMIDI request.
    //
    NTSTATUS ntStatus;
    CPortDMus *p = new(PoolType,'mDcP') CPortDMus(UnknownOuter);
    if (p)
    {
        *Unknown = PUNKNOWN(PPORTMIDI(p));
        (*Unknown)->AddRef();
        ntStatus = STATUS_SUCCESS;
    }
    else
    {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }
    return ntStatus;
}

/*****************************************************************************
 * PortDriverMidi
 *****************************************************************************
 * Port driver descriptor.  Referenced extern in porttbl.c.
 */
PORT_DRIVER
PortDriverMidi =
{
    &CLSID_PortMidi,
    CreatePortMidi
};

/*****************************************************************************

#pragma code_seg()


/*****************************************************************************
 * Member functions
 */

#pragma code_seg()
REFERENCE_TIME DMusicDefaultGetTime(void)
{
    LARGE_INTEGER   liFrequency, liTime,keQPCTime;

    //  total VTD ticks since system booted
    liTime = KeQueryPerformanceCounter(&liFrequency);

#ifndef UNDER_NT











    liFrequency.QuadPart /= 1000;           //  drop the precision on the floor
    liFrequency.QuadPart *= 1000;           //  drop the precision on the floor

#endif  //  !UNDER_NT

    //
    //  Convert ticks to 100ns units.
    //
    keQPCTime.QuadPart = KSCONVERT_PERFORMANCE_TIME(liFrequency.QuadPart,liTime);

    return keQPCTime.QuadPart;
}


 /*****************************************************************************
 * CPortPinDMus::GetTime()
 *****************************************************************************
 * Implementation of get time based on IReferenceClock
 */
#pragma code_seg()
NTSTATUS CPortDMus::GetTime(__out REFERENCE_TIME *pTime)
{
    *pTime = DMusicDefaultGetTime();
    
    return STATUS_SUCCESS;
}

/*****************************************************************************
. * CPortDMus::Notify()
 *****************************************************************************
 * Lower-edge function to notify port driver of notification interrupt.
 */
STDMETHODIMP_(void)
CPortDMus::Notify(__in_opt PSERVICEGROUP ServiceGroup)
{
    _DbgPrintF(DEBUGLVL_BLAB, ("Notify"));
    if (ServiceGroup)
    {
        ServiceGroup->RequestService();
    }
    else
    {
        if (m_MiniportServiceGroup)
        {
            m_MiniportServiceGroup->RequestService();
        }

        for (ULONG pIndex = 0; pIndex < m_PinEntriesUsed; pIndex++)
        {
            if (m_Pins[pIndex] && m_Pins[pIndex]->m_ServiceGroup)
            {
                m_Pins[pIndex]->m_ServiceGroup->RequestService();
            }
        }
    }
}

/*****************************************************************************
 * CPortDMus::RequestService()
 *****************************************************************************
 * 
 */
STDMETHODIMP_(void)
CPortDMus::RequestService(void)
{
    if (m_Miniport)
    {
        TRACELOGGING_FUNC(TRACE_LEVEL_VERBOSE, m_Miniport->Service());
    }
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * CPortDMus::~CPortDMus()
 *****************************************************************************
 * Destructor.
 */
CPortDMus::~CPortDMus()
{
    PAGED_CODE();

    _DbgPrintF(DEBUGLVL_LIFETIME,("Destroying DMUS Port (0x%08x)", this));

    if (m_pSubdeviceDescriptor)
    {
        PcDeleteSubdeviceDescriptor(m_pSubdeviceDescriptor);
    }
    ULONG ulRefCount;
    if (m_MPPinCountI)
    {
        ulRefCount = m_MPPinCountI->Release();
        m_MPPinCountI = NULL;
    }
    if (m_Miniport)
    {
        TRACELOGGING_REFCOUNT(TRACE_LEVEL_VERBOSE, ulRefCount = m_Miniport->Release());
        ASSERT(0 == ulRefCount);
        m_Miniport = NULL;
    }
    if (m_MiniportServiceGroup)
    {
        m_MiniportServiceGroup->RemoveMember(PSERVICESINK(this));
        ulRefCount = m_MiniportServiceGroup->Release();
        ASSERT(0 == ulRefCount);
        m_MiniportServiceGroup = NULL;
    }
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * CPortDMus::NonDelegatingQueryInterface()
 *****************************************************************************
 * Obtains an interface.
 */
STDMETHODIMP_(NTSTATUS)
CPortDMus::NonDelegatingQueryInterface(__in REFIID Interface, __out PVOID * Object)
{
    PAGED_CODE();

    _DbgPrintF(DEBUGLVL_BLAB, ("NonDelegatingQueryInterface"));

    ASSERT(Object);

    if (IsEqualGUIDAligned(Interface,IID_IUnknown))
    {
        *Object = PVOID(PUNKNOWN(PPORT(this)));
    }
    else if (IsEqualGUIDAligned(Interface,IID_IPort))
    {
        *Object = PVOID(PPORT(this));
    }
    else if (IsEqualGUIDAligned(Interface,IID_IPortDMus))
    {
        *Object = PVOID(PPORTDMUS(this));
    }
    else if (IsEqualGUIDAligned(Interface,IID_IIrpTargetFactory))
    {
        *Object = PVOID(PIRPTARGETFACTORY(this));
    }
    else if (IsEqualGUIDAligned(Interface,IID_ISubdevice))
    {
        *Object = PVOID(PSUBDEVICE(this));
    }
    else if (IsEqualGUIDAligned(Interface,IID_IUnregisterSubdevice))
    {
        *Object = PVOID(PUNREGISTERSUBDEVICE(this));
    }
    else if (IsEqualGUIDAligned(Interface,IID_IUnregisterPhysicalConnection))
    {
        *Object = PVOID(PUNREGISTERPHYSICALCONNECTION(this));
    }
    else if (IsEqualGUIDAligned(Interface,IID_IServiceSink))
    {
        *Object = PVOID(PSERVICESINK(this));
    }
    else if (IsEqualGUIDAligned(Interface,IID_IPortEvents))
    {
        *Object = PVOID(PPORTEVENTS(this));
    }












    else if (IsEqualGUIDAligned(Interface,IID_IPortClsVersion))
    {
        *Object = PVOID(PPORTCLSVERSION(this));
    }
    else if (IsEqualGUIDAligned(Interface,IID_IPortClsSubdeviceEx))
    {
        *Object = PVOID(PPORTCLSSubdeviceEx(this));
    }
    else if (IsEqualGUIDAligned(Interface,IID_IPortClsPower))
    {
        *Object = PVOID(PPORTCLSPOWER(this));
    }
    else if (IsEqualGUIDAligned(Interface,IID_IPortWMIRegistration))
    {
        *Object = PVOID(PPORTWMIREGISTRATION(this));
    }
    else if (IsEqualGUIDAligned(Interface,IID_IPortMidi))
    {
        // PPORTDMUS and PPORTMIDI interfaces are the same.
        // Return PPORTDMUS for IID_IPortMidi.
        *Object = PVOID(PPORTDMUS(this));
    }
    else
    {
        *Object = NULL;
    }

    if (*Object)
    {
        PUNKNOWN(*Object)->AddRef();
        return STATUS_SUCCESS;
    }

    return STATUS_INVALID_PARAMETER;
}

static
GUID TopologyCategories[] =
{
    STATICGUIDOF(KSCATEGORY_AUDIO),
    STATICGUIDOF(KSCATEGORY_RENDER),
    STATICGUIDOF(KSCATEGORY_CAPTURE)
};

static
KSPIN_INTERFACE PinInterfacesStream[] =
{
   {
      STATICGUIDOF(KSINTERFACESETID_Standard),
      KSINTERFACE_STANDARD_STREAMING,
      0
   }
};

#pragma code_seg("PAGE")
/*****************************************************************************
 * CPortDMus::Init()
 *****************************************************************************
 * Initializes the port.
 */
STDMETHODIMP_(NTSTATUS)
CPortDMus::Init
(
    __in        PDEVICE_OBJECT  DeviceObjectIn,
    __in_opt    PIRP            Irp,
    __in        PUNKNOWN        UnknownMiniport,
    __in_opt    PUNKNOWN        UnknownAdapter,
    __in        PRESOURCELIST   ResourceList
)
{
    int i;
    PAGED_CODE();

    ASSERT(DeviceObjectIn);
    ASSERT(UnknownMiniport);

    _DbgPrintF(DEBUGLVL_LIFETIME, ("Initializing DMUS Port (0x%08x)", this));

    m_DeviceObject = DeviceObjectIn;
    m_MiniportMidi = NULL;

    KeInitializeMutex(&m_ControlMutex,1);
    KeInitializeEvent(&m_RemovalEvent, NotificationEvent, FALSE);

    KeInitializeSpinLock(&(m_EventList.ListLock));
    InitializeListHead(&(m_EventList.List));
    for(i=0;i<DEFERRED_EVENT_COUNT;i++)
    {
        m_EventContext[i].ContextInUse = FALSE;
        KeInitializeDpc( &m_EventDpc[i],
                     PKDEFERRED_ROUTINE(PcGenerateEventDeferredRoutine),
                         PVOID(&m_EventContext[i]) );
    }

    PSERVICEGROUP pServiceGroup = NULL;

    NTSTATUS ntStatus =
        UnknownMiniport->QueryInterface( IID_IMiniportDMus,
                                         (PVOID *) &m_Miniport);
    if (!NT_SUCCESS(ntStatus))
    {
        // If the miniport does not support IID_IMiniportDMus,
        // the miniport is IID_IMiniportMidi. 
        //
        ntStatus = 
            UnknownMiniport->QueryInterface
            ( 
                IID_IMiniportMidi,
                (PVOID *) &m_MiniportMidi
            );
        if (NT_SUCCESS(ntStatus))
        {
            m_Miniport = (PMINIPORTDMUS) m_MiniportMidi;
        }
    }

    if (NT_SUCCESS(ntStatus))
    {
        TRACELOGGING_FUNC(TRACE_LEVEL_INFORMATION, ntStatus = m_Miniport->Init( UnknownAdapter,
                                     ResourceList,
                                     PPORTDMUS(this),
                                     &pServiceGroup));
    }
    
    if (NT_SUCCESS(ntStatus))
    {
        TRACELOGGING_FUNC(TRACE_LEVEL_INFORMATION, ntStatus = m_Miniport->GetDescription(&m_pPcFilterDescriptor));
    }
    
    if (NT_SUCCESS(ntStatus))
    {
        ntStatus = PcCreateSubdeviceDescriptor
                    (
                        m_pPcFilterDescriptor,
                        SIZEOF_ARRAY(TopologyCategories),
                        TopologyCategories,
                        SIZEOF_ARRAY(PinInterfacesStream),
                        PinInterfacesStream,
                        SIZEOF_ARRAY(PropertyTable_FilterDMus),
                        PropertyTable_FilterDMus,
                        0,      // FilterEventSetCount
                        NULL,   // FilterEventSets
                        SIZEOF_ARRAY(PropertyTable_PinDMus),
                        PropertyTable_PinDMus,
                        SIZEOF_ARRAY(EventTable_PinDMus),
                        EventTable_PinDMus,
                        &m_pSubdeviceDescriptor
                    );
    }
    if (NT_SUCCESS(ntStatus) && pServiceGroup)
    {
        //
        // The miniport supplied a service group.
        //
        if (m_MiniportServiceGroup)
        {
        //
        // We got it already from RegisterServiceGroup().
        // Do a release because we don't need the new ref.
        //
            ASSERT(m_MiniportServiceGroup == pServiceGroup);
            pServiceGroup->Release();
            pServiceGroup = NULL;
        }
        else
        {
            //
            // RegisterServiceGroup() was not called.  We need
            // to add a member.  There is already a reference
            // added by the miniport's Init().
            //
            m_MiniportServiceGroup = pServiceGroup;
            m_MiniportServiceGroup->AddMember(PSERVICESINK(this));
        }
    }

    if (NT_SUCCESS(ntStatus))
    {
        m_Miniport->QueryInterface( IID_IPinCount,(PVOID *)&m_MPPinCountI);
        //  Not checking return value because a failure here is not fatal.
        //  It just means the miniport doesn't support this interface.
    }

#if 0 //DEBUG
    if (NT_SUCCESS(ntStatus))
    {
        PKSPIN_DESCRIPTOR pKsPinDescriptor = m_pSubdeviceDescriptor->PinDescriptors;
        for (ULONG ulPinId = 0; ulPinId < m_pSubdeviceDescriptor->PinCount; ulPinId++, pKsPinDescriptor++)
        {
            if (  (pKsPinDescriptor->Communication == KSPIN_COMMUNICATION_SINK)
               && (pKsPinDescriptor->DataFlow == KSPIN_DATAFLOW_OUT))
            {
                _DbgPrintF(DEBUGLVL_TERSE,("CPortDMus::Init converting pin %d to KSPIN_COMMUNICATION_BOTH",ulPinId));
                pKsPinDescriptor->Communication = KSPIN_COMMUNICATION_BOTH;
            }
        }
    }
#endif //DEBUG

    if(!NT_SUCCESS(ntStatus))
    {
        if (pServiceGroup)
        {
            pServiceGroup->Release();
        }
        
        if (m_MiniportServiceGroup)
        {
            m_MiniportServiceGroup->RemoveMember(PSERVICESINK(this));
            m_MiniportServiceGroup->Release();
            m_MiniportServiceGroup = NULL;
        }

        if (m_MPPinCountI)
        {
            TRACELOGGING_REFCOUNT(TRACE_LEVEL_VERBOSE, m_MPPinCountI->Release());
            m_MPPinCountI = NULL;
        }

        if (m_Miniport)
        {
            TRACELOGGING_REFCOUNT(TRACE_LEVEL_VERBOSE, m_Miniport->Release());
            m_Miniport = NULL;
        }
    }

    _DbgPrintF(DEBUGLVL_BLAB, ("DMusic Port Init done w/status %x",ntStatus));

    return ntStatus;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * CPortDMus::RegisterServiceGroup()
 *****************************************************************************
 * Early registration of the service group to handle interrupts during Init().
 */
STDMETHODIMP_(void)
CPortDMus::RegisterServiceGroup(__in PSERVICEGROUP pServiceGroup)
{
    PAGED_CODE();

    _DbgPrintF(DEBUGLVL_BLAB, ("RegisterServiceGroup"));
    ASSERT(pServiceGroup);
    ASSERT(!m_MiniportServiceGroup);

    m_MiniportServiceGroup = pServiceGroup;
    m_MiniportServiceGroup->AddRef();
    m_MiniportServiceGroup->AddMember(PSERVICESINK(this));
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * CPortDMus::GetRemovalEvent()
 *****************************************************************************
 * Returns removal event for IRP suspension during power down
 */
STDMETHODIMP_(PKEVENT)
CPortDMus::GetRemovalEvent
(
)
{
    PAGED_CODE();

    _DbgPrintF(DEBUGLVL_BLAB, ("GetRemovalEvent"));
    return &m_RemovalEvent;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * CPortDMus::GetDeviceProperty()
 *****************************************************************************
 * Gets device properties from the registry for PnP devices.
 */
STDMETHODIMP_(NTSTATUS)
CPortDMus::GetDeviceProperty
(
    __in    DEVICE_REGISTRY_PROPERTY    DeviceProperty,
    __in    ULONG                       BufferLength,
    __out   PVOID                       PropertyBuffer,
    __out   PULONG                      ResultLength
)
{
    PAGED_CODE();

    _DbgPrintF(DEBUGLVL_BLAB, ("GetDeviceProperty"));
    return ::PcGetDeviceProperty( PVOID(m_DeviceObject),
                                  DeviceProperty,
                                  BufferLength,
                                  PropertyBuffer,
                                  ResultLength );
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * CPortDMus::NewRegistryKey()
 *****************************************************************************
 * Opens/creates a registry key object.
 */
STDMETHODIMP_(NTSTATUS)
CPortDMus::NewRegistryKey
(
    __out       PREGISTRYKEY *      OutRegistryKey,
    __in_opt    PUNKNOWN            OuterUnknown,
    __in        ULONG               RegistryKeyType,
    __in        ACCESS_MASK         DesiredAccess,
    __in_opt    POBJECT_ATTRIBUTES  ObjectAttributes,
    __in_opt    ULONG               CreateOptions,
    __out_opt   PULONG              Disposition
)
{
    PAGED_CODE();

    _DbgPrintF(DEBUGLVL_BLAB, ("NewRegistryKey"));
    return ::PcNewRegistryKey( OutRegistryKey,
                               OuterUnknown,
                               RegistryKeyType,
                               DesiredAccess,
                               PVOID(m_DeviceObject),
                               PVOID(PSUBDEVICE(this)),
                               ObjectAttributes,
                               CreateOptions,
                               Disposition);
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * CPortDMus::ReleaseChildren()
 *****************************************************************************
 * Release child objects.
 */
STDMETHODIMP_(void) CPortDMus::ReleaseChildren(void)
{
    PAGED_CODE();

    _DbgPrintF(DEBUGLVL_LIFETIME, ("ReleaseChildren of DMUS Port (0x%08x)", this));

    if (m_MPPinCountI)
    {
        TRACELOGGING_REFCOUNT(TRACE_LEVEL_VERBOSE, m_MPPinCountI->Release());
        m_MPPinCountI = NULL;
    }

    if (m_Miniport)
    {
        TRACELOGGING_REFCOUNT(TRACE_LEVEL_VERBOSE, m_Miniport->Release());
        m_Miniport = NULL;
    }

    if (m_MiniportServiceGroup)
    {
        m_MiniportServiceGroup->RemoveMember(PSERVICESINK(this));
        m_MiniportServiceGroup->Release();
        m_MiniportServiceGroup = NULL;
    }
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * CPortDMus::GetDescriptor()
 *****************************************************************************
 * Return the descriptor for this port
 */
STDMETHODIMP_(NTSTATUS) 
CPortDMus::GetDescriptor
(
    OUT     const SUBDEVICE_DESCRIPTOR **   ppSubdeviceDescriptor
)
{
    PAGED_CODE();

    _DbgPrintF(DEBUGLVL_BLAB, ("GetDescriptor"));
    ASSERT(ppSubdeviceDescriptor);

    *ppSubdeviceDescriptor = m_pSubdeviceDescriptor;

    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * CPortDMus::DataRangeIntersection()
 *****************************************************************************
 * Generate a format which is the intersection of two data ranges.
 */
STDMETHODIMP_(NTSTATUS)
CPortDMus::DataRangeIntersection
(   
    IN      ULONG           PinId,
    IN      PKSDATARANGE    DataRange,
    IN      PKSDATARANGE    MatchingDataRange,
    IN      ULONG           OutputBufferLength,
    OUT     PVOID           ResultantFormat     OPTIONAL,
    OUT     PULONG          ResultantFormatLength
)
{
    PAGED_CODE();

    _DbgPrintF(DEBUGLVL_BLAB, ("DataRangeIntersection"));
    ASSERT(DataRange);
    ASSERT(MatchingDataRange);
    ASSERT(ResultantFormatLength);

    NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;

    if( m_Miniport )
    {
        TRACELOGGING_FUNC(TRACE_LEVEL_INFORMATION, ntStatus = m_Miniport->DataRangeIntersection( PinId,
                                                  DataRange,
                                                  MatchingDataRange,
                                                  OutputBufferLength,
                                                  ResultantFormat,
                                                  ResultantFormatLength));
    }

    return ntStatus;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * CPortDMus::PowerChangeNotify()
 *****************************************************************************
 * Called by portcls to notify the port/miniport of a device power
 * state change.
 */
STDMETHODIMP_(void)
CPortDMus::PowerChangeNotify(POWER_STATE PowerState)
{
    PAGED_CODE();

    _DbgPrintF(DEBUGLVL_BLAB, ("PowerChangeNotify"));
    PPOWERNOTIFY pPowerNotify;

    if (m_Miniport)
    {
        // QI for the miniport notification interface
        NTSTATUS ntStatus = m_Miniport->QueryInterface( IID_IPowerNotify,
                                                        (PVOID *)&pPowerNotify);
        if(NT_SUCCESS(ntStatus))
        {
            // check if we're powering up
            if (PowerState.DeviceState == PowerDeviceD0)
            {
                // notify the miniport
                if (NT_SUCCESS(ntStatus))
                {
                    TRACELOGGING_FUNC(TRACE_LEVEL_INFORMATION, pPowerNotify->PowerChangeNotify(PowerState));

                    TRACELOGGING_REFCOUNT(TRACE_LEVEL_VERBOSE, pPowerNotify->Release());
                }

                // notify each port pin
                for (ULONG index=0; index < MAX_PINS; index++)
                {
                    if (m_Pins[index])
                    {
                        m_Pins[index]->PowerNotify(PowerState);
                    }
                }
            } 
            else  // we're powering down
            {
                // notify each port pin
                for (ULONG index=0; index < MAX_PINS; index++)
                {
                    if (m_Pins[index])
                    {
                        m_Pins[index]->PowerNotify(PowerState);
                    }
                }

                // notify the miniport
                if (NT_SUCCESS(ntStatus))
                {
                    TRACELOGGING_FUNC(TRACE_LEVEL_INFORMATION, pPowerNotify->PowerChangeNotify(PowerState));

                    TRACELOGGING_REFCOUNT(TRACE_LEVEL_VERBOSE, pPowerNotify->Release());
                }
            }
        }
    }
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * CPortDMus::PinCount()
 *****************************************************************************
 * Called by portcls to give the port\miniport a chance 
 * to override the default pin counts for this pin ID.
 */
STDMETHODIMP_(void)
CPortDMus::PinCount
(
    IN      ULONG   PinId,
    IN  OUT PULONG  FilterNecessary,
    IN  OUT PULONG  FilterCurrent,
    IN  OUT PULONG  FilterPossible,
    IN  OUT PULONG  GlobalCurrent,
    IN  OUT PULONG  GlobalPossible
)
{
    PAGED_CODE();

    _DbgPrintF( DEBUGLVL_BLAB, 
                ("PinCount PID:0x%08x FN(0x%08x):%d FC(0x%08x):%d FP(0x%08x):%d GC(0x%08x):%d GP(0x%08x):%d",
                  PinId,                           FilterNecessary,*FilterNecessary,
                  FilterCurrent,  *FilterCurrent,  FilterPossible, *FilterPossible, 
                  GlobalCurrent,  *GlobalCurrent,  GlobalPossible, *GlobalPossible ) );

    if (m_MPPinCountI)
    {
        TRACELOGGING_FUNC(TRACE_LEVEL_INFORMATION,
            m_MPPinCountI->PinCount(PinId,FilterNecessary,FilterCurrent,FilterPossible,GlobalCurrent,GlobalPossible));

        _DbgPrintF( DEBUGLVL_BLAB, 
                    ("Post-PinCount PID:0x%08x FN(0x%08x):%d FC(0x%08x):%d FP(0x%08x):%d GC(0x%08x):%d GP(0x%08x):%d",
                      PinId,                           FilterNecessary,*FilterNecessary,
                      FilterCurrent,  *FilterCurrent,  FilterPossible, *FilterPossible, 
                      GlobalCurrent,  *GlobalCurrent,  GlobalPossible, *GlobalPossible ) );
    }
}
#pragma code_seg("PAGE")
/*****************************************************************************
 * CPortDMus::GetPinName()
 *****************************************************************************
 * Called by portcls to give the port\miniport a chance
 * to override the default pin name for this pin ID.
 */

STDMETHODIMP_(NTSTATUS)
CPortDMus::GetPinName(IN PIRP _pIrp, 
                      IN PKSP_PIN _pKsPPin,
                      IN OUT PVOID _pvData)
{
    PAGED_CODE();
    return STATUS_NOT_SUPPORTED;
}

STDMETHODIMP_(BOOL)
CPortDMus::IsPinNameSupported()
{
    PAGED_CODE();

    return false;
}
/*****************************************************************************
 * PinTypeName
 *****************************************************************************
 * The name of the pin object type.
 */
static const WCHAR PinTypeName[] = KSSTRING_Pin;

/*****************************************************************************
 * CreateTable
 *****************************************************************************
 * Create dispatch table.
 */
static KSOBJECT_CREATE_ITEM CreateTable[] =
{
    DEFINE_KSCREATE_ITEM(KsoDispatchCreateWithGenericFactory,PinTypeName,0)
};

#pragma code_seg("PAGE")
/*****************************************************************************
 * CPortDMus::NewIrpTarget()
 *****************************************************************************
 * Creates and initializes a filter object.
 */
STDMETHODIMP_(NTSTATUS)
CPortDMus::NewIrpTarget
(
    OUT     PIRPTARGET *        IrpTarget,
    OUT     BOOLEAN *           ReferenceParent,
    IN      PUNKNOWN            OuterUnknown,
    IN      POOL_TYPE           PoolType,
    IN      PDEVICE_OBJECT      DeviceObject,
    IN      PIRP                Irp,
    OUT     PKSOBJECT_CREATE    ObjectCreate
)
{
    PAGED_CODE();

    ASSERT(IrpTarget);
    ASSERT(DeviceObject);
    ASSERT(Irp);
    ASSERT(ObjectCreate);

    _DbgPrintF(DEBUGLVL_BLAB, ("NewIrpTarget"));

    ObjectCreate->CreateItemsCount  = SIZEOF_ARRAY(CreateTable);
    ObjectCreate->CreateItemsList   = CreateTable;

    PUNKNOWN filterUnknown;
    NTSTATUS ntStatus = CreatePortFilterDMus( &filterUnknown,
                                              GUID_NULL,
                                              OuterUnknown,
                                              PoolType);

    if (NT_SUCCESS(ntStatus))
    {
        PPORTFILTERDMUS filterDMus;

        ntStatus = filterUnknown->QueryInterface( IID_IIrpTarget,
                                                  (PVOID *) &filterDMus);
        if (NT_SUCCESS(ntStatus))
        {
            // The QI for IIrpTarget actually gets IPortFilterDMus.
            ntStatus = filterDMus->Init(this);
            if (NT_SUCCESS(ntStatus))
            {
                *ReferenceParent = TRUE;
                *IrpTarget = filterDMus;
            }
            else
            {
                filterDMus->Release();
            }
        }
        filterUnknown->Release();
    }
    return ntStatus;
}

#pragma code_seg()
/*****************************************************************************
 * CPortDMus::AddEventToEventList()
 *****************************************************************************
 * Adds an event to the port's event list.
 */
STDMETHODIMP_(void)
CPortDMus::AddEventToEventList(__in PKSEVENT_ENTRY EventEntry)
{
    ASSERT(EventEntry);

    _DbgPrintF(DEBUGLVL_BLAB,("AddEventToEventList"));

    KIRQL   oldIrql;

    if (EventEntry)
    {
        // grab the event list spin lock
        KeAcquireSpinLock(&(m_EventList.ListLock), &oldIrql);

        // add the event to the list tail
        InsertTailList(&(m_EventList.List),
                        (PLIST_ENTRY)((PVOID)EventEntry));

        // release the event list spin lock
        KeReleaseSpinLock(&(m_EventList.ListLock), oldIrql);
    }
}

/*****************************************************************************
 * CPortDMus::GenerateEventList()
 *****************************************************************************
 * Wraps KsGenerateEventList for miniports.
 */
STDMETHODIMP_(void)
CPortDMus::GenerateEventList
(
    __in_opt    GUID*   Set,
    __in        ULONG   EventId,
    __in        BOOL    PinEvent,
    __in        ULONG   PinId,
    __in        BOOL    NodeEvent,
    __in        ULONG   NodeId
)
{
    if (KeGetCurrentIrql() > DISPATCH_LEVEL)
    {
        int i;
        BOOL bFound = FALSE;









        // Need to search for an open location in our array of DEFERRED_EVENT_COUNT number
        // of entries.
        for(i=0;i<DEFERRED_EVENT_COUNT;i++)
        {
            // This reads: If ContextInUse is FALSE, set it TRUE and return the original
            // value of ContextInUse. If that original Value was FALSE we've claimed an open
            // spot to fill - thus fill it. Or, if ContextInUse does not equal FALSE, do 
            // nothing and return the original value of ContextInUse. It will be TRUE, thus we
            // loop leaving bFound FALSE for the next time through.
            if (InterlockedCompareExchange((LONG*)&m_EventContext[i].ContextInUse, TRUE, FALSE) == FALSE) 
            {
                // It wasn't in use and now it's marked in use, schedule the event.
                m_EventContext[i].Set = Set;
                m_EventContext[i].EventId = EventId;
                m_EventContext[i].PinEvent = PinEvent;
                m_EventContext[i].PinId = PinId;
                m_EventContext[i].NodeEvent = NodeEvent;
                m_EventContext[i].NodeId = NodeId;

                KeInsertQueueDpc( &m_EventDpc[i],
                                  PVOID(&m_EventList),
                                  NULL );
                // We found a location, thus we don't need to look any more.
                bFound = TRUE;
                break;
            }
        }

        if(!bFound)
        {
            // It was in use. We're going to drop this event.
            ASSERT("Dropping Event");
        }
    }
    else
    {
        PcGenerateEventList( &m_EventList,
                             Set,
                             EventId,
                             PinEvent,
                             PinId,
                             NodeEvent,
                             NodeId);
    }
}

/*****************************************************************************
 * CPortDMus::UnregisterSubdevice()
 *****************************************************************************
 */
STDMETHODIMP_(NTSTATUS)
CPortDMus::
UnregisterSubdevice
(
    __in    PDEVICE_OBJECT  DeviceObject,
    __in    PUNKNOWN        Unknown
)
{
    return PcUnregisterSubdevice(DeviceObject, Unknown);
}

/*****************************************************************************
 * CPortDMus::UnregisterPhysicalConnection()
 *****************************************************************************
 */
STDMETHODIMP_(NTSTATUS)
CPortDMus::
UnregisterPhysicalConnection
(
    __in    PDEVICE_OBJECT  pDeviceObject,
    __in    PUNKNOWN        pUnknownFrom,
    __in    ULONG           ulFromPin,
    __in    PUNKNOWN        pUnknownTo,
    __in    ULONG           ulToPin
)
{
    return PcUnregisterPhysicalConnection(pDeviceObject,
                                          pUnknownFrom,
                                          ulFromPin,
                                          pUnknownTo,
                                          ulToPin);
}

/*****************************************************************************
 * CPortDMus::UnregisterPhysicalConnectionToExternal()
 *****************************************************************************
 */
STDMETHODIMP_(NTSTATUS)
CPortDMus::
UnregisterPhysicalConnectionToExternal
(
    __in    PDEVICE_OBJECT  pDeviceObject,
    __in    PUNKNOWN        pUnknownFrom,
    __in    ULONG           ulFromPin,
    __in    PUNICODE_STRING pUnicodeStringTo,
    __in    ULONG           ulToPin
)
{
    return PcUnregisterPhysicalConnectionToExternal(pDeviceObject,
                                                    pUnknownFrom,
                                                    ulFromPin,
                                                    pUnicodeStringTo,
                                                    ulToPin);
}

/*****************************************************************************
 * CPortDMus::UnregisterPhysicalConnectionFromExternal()
 *****************************************************************************
 */
STDMETHODIMP_(NTSTATUS)
CPortDMus::
UnregisterPhysicalConnectionFromExternal
(
    __in    PDEVICE_OBJECT  pDeviceObject,
    __in    PUNICODE_STRING pUnicodeStringFrom,
    __in    ULONG           ulFromPin,
    __in    PUNKNOWN        pUnknownTo,
    __in    ULONG           ulToPin
)
{
    return PcUnregisterPhysicalConnectionFromExternal(pDeviceObject,
                                                      pUnicodeStringFrom,
                                                      ulFromPin,
                                                      pUnknownTo,
                                                      ulToPin);
}













































































